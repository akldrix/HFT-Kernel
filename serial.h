#ifndef SERIAL_H
#define SERIAL_H
#include <stdint.h>

void serial_init(void);
void serial_putc(char c);
void serial_printl(const char *s);
void serial_print_hex(uint32_t val);
void serial_print_ip(uint8_t ip[4]);
void serial_print_dec(uint32_t val);

#endif // !SERIAL_H
