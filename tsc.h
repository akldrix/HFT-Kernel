#ifndef TSC_H
#define TSC_H
#include <stdint.h>

static inline uint64_t rdtsc() {
  uint32_t hi, lo;
  __asm__ volatile("lfence\n\t"
                   "rdtsc"
                   : "=a"(lo), "=d"(hi)
                   :
                   : "memory");
  return ((uint64_t)hi << 32) | lo;
}
#endif // !TSC_H
