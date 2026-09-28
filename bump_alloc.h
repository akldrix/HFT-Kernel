#ifndef BUMP_ALLOC
#define BUMP_ALLOC

#include <stddef.h>
#include <stdint.h>

typedef struct {
  uintptr_t start;
  uintptr_t end;
  uintptr_t next;
} BumpAllocator;

void bump_init(BumpAllocator *alloc, uintptr_t start_addr, size_t size);
void *bump_alloc(BumpAllocator *alloc, size_t size, size_t alignment);
void bump_clear(BumpAllocator *alloc);
#endif // BUMP_ALLOC
