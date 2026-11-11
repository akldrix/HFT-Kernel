#ifndef STATS_H
#define STATS_H
#include <stdint.h>

void stats_add(uint32_t proc, uint32_t door);
void stats_dump(void);

#endif // !STATS_H
