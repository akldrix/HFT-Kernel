#include "console.h"
#include "pci.h"
#include <stdbool.h>
#include <stddef.h>

#if defined(__linux__)
#error                                                                         \
    "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

void kernel_main(void) {
  console_clear();
  console_print("Kernel Prototype\n");
  console_print("--------------------------------------------------------------"
                "------------------");
  console_print_hex(0x8086);
  console_print("\n");
  pci_scan();

  while (1) {
  }
}
