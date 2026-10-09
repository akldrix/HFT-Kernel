#include "console.h"
#include <stddef.h>
#include <stdint.h>
#define W_WIDTH 80
#define W_HEIGHT 25

volatile uint16_t *vga_buffer = (uint16_t *)0xB8000;
int cursor_x = 0;
int cursor_y = 0;

void console_clear() {
  for (int y = 0; y < W_HEIGHT; y++)
    for (int x = 0; x < W_WIDTH; x++)
      vga_buffer[y * W_WIDTH + x] = (uint16_t)' ' | (0x0F << 8);
  cursor_x = 0;
  cursor_y = 0;
}

void console_putchar(char c) {
  if (c == '\n') {
    cursor_x = 0;
    cursor_y++;
  } else {
    vga_buffer[cursor_y * W_WIDTH + cursor_x] = (uint16_t)c | (0x0F << 8);
    cursor_x++;
  }
  if (cursor_x >= W_WIDTH) {
    cursor_y++;
    cursor_x = 0;
  }

  if (cursor_y >= W_HEIGHT)
    cursor_y = 0;
}

void console_print(const char *str) {
  for (int i = 0; str[i] != '\0'; ++i) {
    console_putchar(str[i]);
  }
}

void console_print_hex(uint32_t val) {
  console_print("0x");
  if (val == 0) {
    console_print("0");
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
  console_print(&buffer[i + 1]);
}

void console_print_dec(uint32_t val) {
  if (val == 0) {
    console_print("0");
    return;
  }

  char buffer[8];
  buffer[7] = '\0';
  int i = 6;

  while (val > 0 && i >= 0) {
    int remainder = val % 10;
    buffer[i] = '0' + remainder;

    val /= 10;
    i--;
  }
  console_print(&buffer[i + 1]);
}

void console_print_char(const char c) { console_putchar(c); }

void console_print_ip(uint8_t ip[4]) {
  for (int i = 0; i < 4; i++) {
    console_print_dec(ip[i]);
    if (i < 3) {
      console_print_char('.');
    }
  }
}

void *memset(void *dest, int ch, size_t count) {
  unsigned char *p = dest;
  while (count--) {
    *p++ = (unsigned char)ch;
  }
  return dest;
}
