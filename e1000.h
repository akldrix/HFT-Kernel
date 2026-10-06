#ifndef E_1000_H
#define E_1000_H
#include "bump_alloc.h"
#include <stdint.h>

#define ETHERTYPE_IPV4 0x0800
#define ETHERTYPE_ARP 0x0806
#define PROTOCOL_UDP 17

#define ARP_OP_REQUEST 1
#define ARP_OP_REPLY 2

#define E1000_REG_CTRL 0x0000
#define E1000_REG_STATUS 0x0008
#define E1000_REG_IMS 0x00D0
#define E1000_REG_IMC 0x00D8
#define E1000_REG_RCTL 0x0100
#define E1000_REG_RDBAL 0x2800
#define E1000_REG_RDBAH 0x2804
#define E1000_REG_RDLEN 0x2808
#define E1000_REG_RDH 0x2810
#define E1000_REG_RDT 0x2818

#define E1000_RCTL_EN (1 << 1)
#define E1000_RCTL_SBP (1 << 2)
#define E1000_RCTL_UPE (1 << 3)
#define E1000_RCTL_MPE (1 << 4)
#define E1000_RCTL_LPE (1 << 5)
#define E1000_RCTL_BAM (1 << 15)
#define E1000_RCTL_SECRC (1 << 26)

#define NUM_RX_DESCRIPTORS 256

#define E1000_REG_TCTL 0x04000
#define E1000_REG_TDBAL 0x03800
#define E1000_REG_TDBAH 0x03804
#define E1000_REG_TDLEN 0x03808
#define E1000_REG_TDH 0x03810
#define E1000_REG_TDT 0x03818

#define NUM_TX_DESCRIPTORS 256

#define E1000_TX_CMD_EOP (1 << 0)
#define E1000_TX_CMD_IFCS (1 << 1)
#define E1000_TX_CMD_RS (1 << 3)

#define E1000_TCTL_EN (1 << 1)
#define E1000_TCTL_PSP (1 << 3)

typedef struct __attribute__((packed)) {
  uint64_t buffer_addr;
  uint16_t length;
  uint16_t checksum;
  volatile uint8_t status;
  uint8_t errors;
  uint16_t special;
} e1000_rx_desc;

typedef struct __attribute__((packed)) {
  uint64_t buffer_addr;
  uint16_t length;
  uint8_t cso;
  uint8_t cmd;
  uint8_t status;
  uint8_t css;
  uint16_t special;
} e1000_tx_desc;

typedef struct __attribute__((packed)) {
  uint8_t dest_mac[6];
  uint8_t src_mac[6];
  uint16_t ethertype;
} ethernet_header_t;

typedef struct __attribute__((packed)) {
  uint8_t ver_ihl;
  uint8_t type_o_ser;
  uint16_t total_len;
  uint16_t up_id;
  uint16_t flags_fragoff;
  uint8_t ttl;
  uint8_t protocol;
  uint16_t checksum;
  uint32_t src_ip;
  uint32_t dest_ip;

} ipv4_header_t;

typedef struct __attribute__((packed)) {
  uint16_t hw_type;
  uint16_t protocol_type;
  uint8_t hw_len;
  uint8_t protocol_len;
  uint16_t operation;
  uint8_t sender_mac[6];
  uint8_t sender_ip[4];

  uint8_t dest_mac[6];
  uint8_t dest_ip[4];

} arp_header_t;

typedef struct __attribute__((packed)) {
  uint16_t src_port;
  uint16_t dest_port;
  uint16_t len;
  uint16_t checksum;
} udp_header_t;

void e1000_write_reg(uint16_t reg, uint32_t value);

uint32_t e1000_read_reg(uint16_t reg);

void e1000_init(uintptr_t bar0, BumpAllocator *alloc);

void e1000_poll_rx(void);

void e1000_send_packet(const void *data, uint16_t len);
#endif // !E_1000_H
