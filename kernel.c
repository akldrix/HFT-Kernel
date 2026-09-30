#include "bump_alloc.h"
#include "console.h"
#include "e1000.h"
#include "pci.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(__linux__)
#error                                                                         \
    "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i386__)
#error "This kernel needs to be compiled with a ix86-elf compiler"
#endif

extern uint8_t _kernel_end;
BumpAllocator kernel_alloc;

void kernel_main(void) {

  uintptr_t heap_start = (uintptr_t)&_kernel_end;
  heap_start = (heap_start + 0xFFF) & ~0xFFF;

  bump_init(&kernel_alloc, heap_start, 4 * 1024 * 1024);

  console_clear();
  console_print("Kernel Prototype\n");
  console_print("--------------------------------------------------------------"
                "------------------");
  console_print_hex(0x8086);
  console_print("\n");

  // pci_scan();
  //
  // uint32_t *ptr1 = (uint32_t *)bump_alloc(&kernel_alloc, 64, 16);
  //
  // if (ptr1 == 0) {
  //   console_print("[ERR] Failed to allocate memory\n");
  //   return;
  // } else {
  //   console_print("[SUCCESS] Allocated 64 bytes successfuly!\n");
  // }
  uint8_t bus = 0;
  uint8_t slot = 3;

  uintptr_t bar0 = get_pci_bar0(bus, slot, 0);

  enable_pci_bus_mastering(bus, slot, 0);

  e1000_init(bar0, &kernel_alloc);

  while (1) {
    e1000_poll_rx();
  }
}
