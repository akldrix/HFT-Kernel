#include "pci.h"
#include "console.h"
#include "io.h"
#include <stdint.h>

uint32_t read_pci_config_32(uint8_t bus, uint8_t slot, uint8_t func,
                            uint8_t offset) {
  uint32_t address;

  address = (uint32_t)((bus << 16) | (slot << 11) | (func << 8) |
                       (offset & 0xFC) | ((uint32_t)0x80000000));

  outl(0xCF8, address);
  return inl(0xCFC);
}

void pci_scan() {
  console_print("[PCI] Scanning bus for devices\n");
  for (uint16_t bus = 0; bus < 256; bus++) {
    for (uint8_t slot = 0; slot < 32; slot++) {
      uint32_t vendor_device = read_pci_config_32(bus, slot, 0, 0);

      if (vendor_device == 0xFFFFFFFF) {
        continue;
      }
      uint16_t vendor = vendor_device & 0xFFFF;
      uint16_t device = vendor_device >> 16;

      if (vendor == 0x8086 && device == 0x100E) {
        console_print("Successfuly found Intel e1000\n");
        console_print("     Bus:");
        console_print_hex(bus);
        console_print(" | Slot: ");
        console_print_hex(slot);
        console_print("\n");
        return;
      }
    }
  }
  console_print("[PCI] ERROR! Cannot find the e1000 NIC");
}
