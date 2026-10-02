#include "e1000.h"
#include "bump_alloc.h"
#include "console.h"
#include <stdint.h>

#define ETHERTYPE_IPV4 0x0800
#define ETHERTYPE_ARP 0x0806

static inline uint16_t ntohs(uint16_t x) {
  return (uint16_t)(((x & 0xFF) << 8) | ((x >> 8) & 0xFF));
}

typedef struct __attribute__((__packed__)) {
  uint8_t dest_mac[6];
  uint8_t src_mac[6];
  uint16_t ethertype;
} ethernet_header_t;

static uintptr_t mmio_base = 0;
static e1000_rx_desc *rx_ring = 0;
static uint16_t rx_current_idx = 0;

void e1000_write_reg(uint16_t reg, uint32_t value) {
  *(volatile uint32_t *)(mmio_base + reg) = value;
}

uint32_t e1000_read_reg(uint16_t reg) {
  return *(volatile uint32_t *)(mmio_base + reg);
}

void e1000_init(uintptr_t bar0, BumpAllocator *alloc) {
  mmio_base = bar0;
  e1000_write_reg(E1000_REG_IMC, 0xFFFFFFFF);

  e1000_write_reg(E1000_REG_CTRL, e1000_read_reg(E1000_REG_CTRL) | (1 << 26));
  for (volatile int i = 0; i < 10000; i++)
    ;

  e1000_write_reg(E1000_REG_IMC, 0xFFFFFFFF);

  rx_ring = (e1000_rx_desc *)bump_alloc(
      alloc, sizeof(e1000_rx_desc) * NUM_RX_DESCRIPTORS, 16);

  for (int i = 0; i < NUM_RX_DESCRIPTORS; i++) {
    uint8_t *buffer = (uint8_t *)bump_alloc(alloc, 2048, 16);
    rx_ring[i].buffer_addr = (uint64_t)(uintptr_t)buffer;
    rx_ring[i].status = 0;
  }

  e1000_write_reg(E1000_REG_RDBAL, (uint32_t)(uintptr_t)rx_ring);
  e1000_write_reg(E1000_REG_RDBAH, 0);
  e1000_write_reg(E1000_REG_RDLEN, NUM_RX_DESCRIPTORS * sizeof(e1000_rx_desc));

  e1000_write_reg(E1000_REG_RDH, 0);
  e1000_write_reg(E1000_REG_RDT, NUM_RX_DESCRIPTORS - 1);

  uint32_t rctl = E1000_RCTL_EN | E1000_RCTL_BAM | E1000_RCTL_UPE |
                  E1000_RCTL_MPE | E1000_RCTL_SECRC;
  e1000_write_reg(E1000_REG_RCTL, rctl);
  if (e1000_read_reg(E1000_REG_RCTL))
    console_print("[e1000] Driver initialized!\n");
}

void e1000_poll_rx() {
  if (rx_ring[rx_current_idx].status & 0x01) {
    uint16_t len = rx_ring[rx_current_idx].length;

    uint8_t *packet_data =
        (uint8_t *)(uintptr_t)rx_ring[rx_current_idx].buffer_addr;

    console_print("[NET] Size of received packet: ");
    console_print_dec(len);
    console_print("\n");

    ethernet_header_t *eth = (ethernet_header_t *)packet_data;

    console_print("SRC MAC: ");
    for (int i = 0; i < 6; i++) {
      console_print_hex(eth->src_mac[i]);
      if (i < 5)
        console_print(":");
    }
    console_print("\n");

    uint16_t protocol = ntohs(eth->ethertype);
    if (protocol == ETHERTYPE_IPV4) {
      console_print("Protocol: IPv4\n");
    } else if (protocol == ETHERTYPE_ARP) {
      console_print("Protocol: ARP\n");
    } else {
      console_print("Unknown protocol: (");
      console_print_hex(protocol);
      console_print(")\n");
    }

    rx_ring[rx_current_idx].status = 0;

    uint16_t old_idx = rx_current_idx;

    rx_current_idx = (rx_current_idx + 1) % NUM_RX_DESCRIPTORS;

    e1000_write_reg(E1000_REG_RDT, old_idx);
  }
}
