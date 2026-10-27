#ifndef SERIAL_H
#define SERIAL_H

void serial_init(void);
void serial_putc(char c);
void serial_printl(const char *s);

#endif // !SERIAL_H
