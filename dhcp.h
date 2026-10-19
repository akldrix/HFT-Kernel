#ifndef DHCP_H
#define DHCP_H

#include <stdint.h>

#define DHCP_CHADDR_LEN 16
#define DHCP_SNAME_LEN 64
#define DHCP_FILE_LEN 128
#define DHCP_OPTIONS_LEN 312
#define DHCP_MAGIC_COOKIE 0x63825363
#define DHCP_XID 0x12345678

#define DHCP_SERVER_PORT 67
#define DHCP_CLIENT_PORT 68

#define DHCP_MSGTYPE_DISCOVER 1
#define DHCP_MSGTYPE_OFFER 2
#define DHCP_MSGTYPE_REQUEST 3
#define DHCP_MSGTYPE_ACK 5
#define DHCP_MSGTYPE_NAK 6

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

typedef enum {
  DHCP_IDLE,
  DHCP_DISCOVER_SENT,
  DHCP_REQUEST_SENT,
  DHCP_BOUND
} dhcp_state_t;

extern uint8_t my_ip[4];
extern uint8_t net_mask[4];
extern uint8_t gateway_ip[4];
extern uint8_t dns_ip[4];

extern dhcp_state_t dhcp_state;

void dhcp_handle_packet(const dhcp_packet_t *packet, uint16_t payload_len);

void dhcp_send_discover(void);
uint8_t dhcp_get_msg_type(const dhcp_packet_t *packet, uint16_t payload_len);

#endif // !DHCP_H
