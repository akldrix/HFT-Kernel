#include "serial.h"
#include "io.h"
#include <stdint.h>

#define COM1 0x3F8

void serial_init() {
  outb(COM1 + 1, 0x00);
  outb(COM1 + 3, 0x80);
  outb(COM1 + 0, 0x03);
  outb(COM1 + 1, 0x00);
  outb(COM1 + 3, 0x03);
  outb(COM1 + 2, 0xC7);
  outb(COM1 + 4, 0x0B);
}

static int serial_tx_empty() { return inb(COM1 + 5) & 0x20; }

void serial_putc(char c) {
  while (!serial_tx_empty())
    ;
  outb(COM1, c);
}

void serial_printl(const char *s) {
  for (int i = 0; s[i] != '\0'; ++i) {
    serial_putc(s[i]);
  }
}

void serial_print_dec(uint32_t val) {
  if (val == 0) {
    serial_printl("0");
    return;
  }

  char buffer[11];
  buffer[10] = '\0';
  int i = 9;

  while (val > 0 && i >= 0) {
    int remainder = val % 10;
    buffer[i] = '0' + remainder;

    val /= 10;
    i--;
  }
  serial_printl(&buffer[i + 1]);
}

void serial_print_ip(uint8_t ip[4]) {
  for (int i = 0; i < 4; i++) {
    serial_print_dec(ip[i]);
    if (i < 3) {
      serial_putc('.');
    }
  }
}

void serial_print_hex(uint32_t val) {
  serial_printl("0x");
  if (val == 0) {
    serial_printl("0");
    return;
  }

  char buffer[9];
  buffer[8] = '\0';
  int i = 7;

  while (val > 0 && i >= 0) {
    int ramainder = val % 16;
    if (ramainder < 10) {
      buffer[i] = '0' + ramainder;
    } else {
      buffer[i] = 'A' + (ramainder - 10);
    }
    val /= 16;
    i--;
  }
  serial_printl(&buffer[i + 1]);
}
