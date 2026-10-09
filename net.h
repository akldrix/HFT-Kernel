#ifndef NET_H
#define NET_H

#include <stdint.h>
static inline uint16_t ntohs(uint16_t x) {
  return (uint16_t)(((x & 0xFF) << 8) | ((x >> 8) & 0xFF));
}

static inline uint16_t htons(uint16_t x) { return ntohs(x); }

static inline uint16_t ntohl(uint32_t x) {
  return (uint32_t)((x & 0x000000FF) << 24) | ((x & 0x0000FF00) << 8) |
         ((x & 0x00FF0000)) | ((x & 0xFF000000));
}
static inline uint16_t htonl(uint32_t x) { return ntohl(x); }

static inline uint16_t ipv4_checksum(uint16_t *ptr, int nbytes) {
  uint32_t sum = 0;
  while (nbytes > 1) {
    sum += *ptr++;
    nbytes -= 2;
  }
  if (nbytes == 1) {
    sum += *(uint8_t *)ptr;
  }

  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);
  return (uint16_t)~sum;
}

#endif // !NET_H
