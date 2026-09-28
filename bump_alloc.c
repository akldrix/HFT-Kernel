#include "bump_alloc.h"
#include <stdint.h>

void bump_init(BumpAllocator *alloc, uintptr_t start_addr, size_t size) {
  alloc->start = start_addr;
  alloc->next = start_addr;
  alloc->end = start_addr + size;
}

void *bump_alloc(BumpAllocator *alloc, size_t size, size_t alignment) {
  if (alignment == 0) {
    alignment = 1;
  }

  if ((alignment & (alignment - 1)) != 0)
    return NULL;

  uintptr_t current = alloc->next;
  uintptr_t aligned = (current + (alignment - 1)) & ~(alignment - 1);
  if (aligned + size > alloc->end || aligned + size < current) {
    return NULL;
  }
  alloc->next = aligned + size;
  return (void *)aligned;
}

void bump_clear(BumpAllocator *alloc) { alloc->next = alloc->start; }
