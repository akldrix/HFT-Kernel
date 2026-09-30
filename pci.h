#ifndef PCI_H
#define PCI_H

#include <stdint.h>

uint32_t read_pci_config_32(uint8_t bus, uint8_t slot, uint8_t func,
                            uint8_t offset);
void write_pci_config_32(uint8_t bus, uint8_t slot, uint8_t func,
                         uint8_t offset, uint32_t val);
void enable_pci_bus_mastering(uint8_t bus, uint8_t slot, uint8_t func);

uintptr_t get_pci_bar0(uint8_t bus, uint8_t slot, uint8_t func);

void pci_scan(void);

#endif // !PCI_H
