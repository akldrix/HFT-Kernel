#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <stdint.h>
void console_clear(void);
void console_putchar(char);
void console_print(const char *);
void console_print_hex(uint32_t);
void console_print_dec(uint32_t);

#endif
