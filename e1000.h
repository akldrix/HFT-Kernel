#ifndef E_1000_H
#define E_1000_H
#include "bump_alloc.h"
#include <stdint.h>

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

typedef struct __attribute__((__packed__)) {
  uint64_t buffer_addr;
  uint16_t length;
  uint16_t checksum;
  volatile uint8_t status;
  uint8_t errors;
  uint16_t special;
} e1000_rx_desc;

void e1000_write_reg(uint16_t reg, uint32_t value);

uint32_t e1000_read_reg(uint16_t reg);

void e1000_init(uintptr_t bar0, BumpAllocator *alloc);

void e1000_poll_rx(void);
#endif // !E_1000_H
