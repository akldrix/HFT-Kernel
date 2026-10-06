#include "e1000.h"
#include "bump_alloc.h"
#include "console.h"
#include <stdint.h>

static inline uint16_t ntohs(uint16_t x) {
  return (uint16_t)(((x & 0xFF) << 8) | ((x >> 8) & 0xFF));
}

static inline uint16_t htons(uint16_t x) {
  return (uint16_t)(((x & 0xFF) << 8) | ((x >> 8) & 0xFF));
}

static inline uint16_t ntohl(uint32_t x) {
  return (uint32_t)((x & 0x000000FF) << 24) | ((x & 0x0000FF00) << 8) |
         ((x & 0x00FF0000)) | ((x & 0xFF000000));
}

static uintptr_t mmio_base = 0;

static e1000_rx_desc *rx_ring = 0;
static uint16_t rx_current_idx = 0;

static e1000_tx_desc *tx_ring = 0;
static uint16_t tx_current_idx = 0;

static uint8_t my_mac[6] = {0};

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

  uint32_t ral = e1000_read_reg(E1000_REG_RAL0);
  uint32_t rah = e1000_read_reg(E1000_REG_RAH0);

  my_mac[0] = (uint8_t)(ral & 0xFF);
  my_mac[1] = (uint8_t)((ral >> 8) & 0xFF);
  my_mac[2] = (uint8_t)((ral >> 16) & 0xFF);
  my_mac[3] = (uint8_t)((ral >> 24) & 0xFF);
  my_mac[4] = (uint8_t)(rah & 0xFF);
  my_mac[5] = (uint8_t)((rah >> 8) & 0xFF);

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

  tx_ring = (e1000_tx_desc *)bump_alloc(
      alloc, sizeof(e1000_tx_desc) * NUM_TX_DESCRIPTORS, 16);

  for (int i = 0; i < NUM_TX_DESCRIPTORS; i++) {
    uint8_t *buffer = (uint8_t *)bump_alloc(alloc, 2048, 16);
    tx_ring[i].buffer_addr = (uint64_t)(uintptr_t)buffer;
    tx_ring[i].cmd = 0;
    tx_ring[i].status = 0;
  }

  e1000_write_reg(E1000_REG_TDBAL, (uint32_t)(uintptr_t)tx_ring);
  e1000_write_reg(E1000_REG_TDBAH, 0);
  e1000_write_reg(E1000_REG_TDLEN, NUM_TX_DESCRIPTORS * sizeof(e1000_tx_desc));

  e1000_write_reg(E1000_REG_TDH, 0);
  e1000_write_reg(E1000_REG_TDT, 0);

  e1000_write_reg(E1000_REG_TCTL,
                  E1000_TCTL_EN | E1000_TCTL_PSP | (0x0F << 4) | (0x40 << 12));

  if (e1000_read_reg(E1000_REG_RCTL))
    console_print("[e1000] Driver initialized!\n");

  for (int i = 0; i < 6; ++i) {
    console_print_hex(my_mac[i]);
    if (i < 5)
      console_print(":");
  }
  console_print("\n");
}

void e1000_send_packet(const void *data, uint16_t len) {
  uint8_t *buffer = (uint8_t *)(uintptr_t)tx_ring[tx_current_idx].buffer_addr;
  const uint8_t *src = (const uint8_t *)data;
  uint16_t packet_len = len;

  if (packet_len < 60)
    packet_len = 60;
  for (uint16_t i = 0; i < len; ++i) {
    buffer[i] = src[i];
  }

  for (uint16_t i = len; i < packet_len; ++i) {
    buffer[i] = 0;
  }
  tx_ring[tx_current_idx].length = packet_len;
  tx_ring[tx_current_idx].cmd =
      E1000_TX_CMD_EOP | E1000_TX_CMD_IFCS | E1000_TX_CMD_RS;
  tx_ring[tx_current_idx].status = 0;

  uint16_t old_idx = tx_current_idx;

  for (uint16_t i = 0; i < len; ++i) {
    console_print_hex(buffer[i]);
    console_print(" ");
  }
  console_print("\n");
  console_print_dec(packet_len);
  console_print("\n");

  tx_current_idx = (tx_current_idx + 1) % NUM_TX_DESCRIPTORS;
  e1000_write_reg(E1000_REG_TDT, tx_current_idx);
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

    // console_print("SRC MAC: ");
    // for (int i = 0; i < 6; i++) {
    //   console_print_hex(eth->src_mac[i]);
    //   if (i < 5)
    //     console_print(":");
    // }
    // console_print("\n");

    // uint16_t protocol = ntohs(eth->ethertype);
    // if (protocol == ETHERTYPE_IPV4) {
    //   console_print("Protocol: IPv4\n");
    // } else if (protocol == ETHERTYPE_ARP) {
    //   console_print("Protocol: ARP\n");
    // } else {
    //   console_print("Unknown protocol: (");
    //   console_print_hex(protocol);
    //   console_print(")\n");
    // }

    if (ntohs(eth->ethertype) == ETHERTYPE_IPV4) {
      ipv4_header_t *ip =
          (ipv4_header_t *)(packet_data + sizeof(ethernet_header_t));

      if (ip->protocol == PROTOCOL_UDP) {
        uint8_t ip_header_len = (ip->ver_ihl & 0x0F) * 4;

        udp_header_t *udp = (udp_header_t *)((uint8_t *)ip + ip_header_len);
        char *payload = (char *)udp + sizeof(udp_header_t);
        uint16_t payload_len = ntohs(udp->len) - sizeof(udp_header_t);

        console_print("Incoming UDP Detectced\n");
        console_print("Port: ");
        console_print_dec(ntohs(udp->dest_port));
        console_print("\n");

        console_print("Payload: ");
        for (int i = 0; i < payload_len; ++i) {
          if (payload[i] >= 32 && payload[i] <= 126) {
            console_print_char(payload[i]);
          }
        }
        console_print("\n");
      }
    } else if (ntohs(eth->ethertype) == ETHERTYPE_ARP) {
      arp_header_t *arp =
          (arp_header_t *)(packet_data + sizeof(ethernet_header_t));

      uint16_t opcode = ntohs(arp->operation);

      if (opcode == ARP_OP_REQUEST) {
        console_print("[ARP]Who has ");
        console_print_ip(arp->dest_ip);
        console_print("? Tell ");
        console_print_ip(arp->sender_ip);
        console_print("\n");

        for (int i = 0; i < 6; ++i) {
          eth->dest_mac[i] = eth->src_mac[i];
          eth->src_mac[i] = my_mac[i];
        }
        arp->operation = htons(ARP_OP_REPLY);

        for (int i = 0; i < 6; ++i)
          arp->dest_mac[i] = arp->sender_mac[i];
        for (int i = 0; i < 4; ++i)
          arp->dest_ip[i] = arp->sender_ip[i];

        for (int i = 0; i < 6; ++i)
          arp->sender_mac[i] = my_mac[i];

        arp->sender_ip[0] = 10;
        arp->sender_ip[1] = 0;
        arp->sender_ip[2] = 2;
        arp->sender_ip[3] = 15;

        e1000_send_packet(packet_data,
                          sizeof(ethernet_header_t) + sizeof(arp_header_t));

        console_print("[ARP] Sent Reply\n");
      }
    }

    rx_ring[rx_current_idx].status = 0;

    uint16_t old_idx = rx_current_idx;

    rx_current_idx = (rx_current_idx + 1) % NUM_RX_DESCRIPTORS;

    e1000_write_reg(E1000_REG_RDT, old_idx);
  }
}
