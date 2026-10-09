#ifndef DHCP_H
#define DHCP_H

#include <stdint.h>

#define DHCP_CHADDR_LEN 16
#define DHCP_SNAME_LEN 64
#define DHCP_FILE_LEN 128
#define DHCP_OPTIONS_LEN 312
#define DHCP_MAGIC_COOKIE 0x63825363

typedef struct __attribute__((packed)) {
  uint8_t op;
  uint8_t htype;
  uint8_t hlen;
  uint8_t hops;
  uint32_t xid;
  uint16_t secs;
  uint16_t flags;
  uint32_t ciaddr;
  uint32_t yiaddr;
  uint32_t siaddr;
  uint32_t giaddr;
  uint8_t chaddr[DHCP_CHADDR_LEN];
  uint8_t sname[DHCP_SNAME_LEN];
  uint8_t file[DHCP_FILE_LEN];
  uint32_t magic_cookie;
  uint8_t options[DHCP_OPTIONS_LEN];
} dhcp_packet_t;

void dhcp_send_discover(void);

#endif // !DHCP_H
