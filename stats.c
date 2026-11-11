#include "stats.h"
#include "serial.h"
#include <stdint.h>

#define MAX_SAMPLES 10100

static uint32_t s_proc[MAX_SAMPLES];
static uint32_t s_door[MAX_SAMPLES];
static uint32_t s_count = 0;

void stats_add(uint32_t proc, uint32_t door) {
  if (s_count >= MAX_SAMPLES) {
    stats_dump();
    return;
  }
  s_proc[s_count] = proc;
  s_door[s_count] = door;

  if (++s_count >= MAX_SAMPLES) {
    stats_dump();
  }
}

void stats_dump(void) {
  serial_printl("#BEGIN proc, door (cycles)\n");
  for (uint32_t i = 0; i < s_count; ++i) {
    serial_print_dec(s_proc[i]);
    serial_putc(',');
    serial_print_dec(s_door[i]);
    serial_putc('\n');
  }
  serial_printl("#END\n");
}
