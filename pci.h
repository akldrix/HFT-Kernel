#ifndef PCI_H
#define PCI_H

#include <stdint.h>

uint32_t read_pci_config_32(uint8_t bus, uint8_t slot, uint8_t func,
                            uint8_t offset);
void pci_scan(void);

#endif // !PCI_H
