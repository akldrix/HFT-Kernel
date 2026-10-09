#include "dhcp.h"
#include "console.h"
#include "e1000.h"
#include "net.h"
#include <stdint.h>

void dhcp_send_discover(void) {
  uint8_t packet[512] = {0};
  ethernet_header_t *eth = (ethernet_header_t *)packet;
  ipv4_header_t *ip = (ipv4_header_t *)(packet + sizeof(ethernet_header_t));
  udp_header_t *udp = (udp_header_t *)((uint8_t *)ip + sizeof(ipv4_header_t));
  dhcp_packet_t *dhcp =
      (dhcp_packet_t *)((uint8_t *)udp + sizeof(udp_header_t));

  uint8_t my_mac[6];
  e1000_get_mac(my_mac);

  for (int i = 0; i < 6; i++) {
    eth->dest_mac[i] = 0xFF;
    eth->src_mac[i] = my_mac[i];
  }
  eth->ethertype = htons(ETHERTYPE_IPV4);

  ip->ver_ihl = 0x45;
  ip->type_o_ser = 0;
  ip->total_len = htons(sizeof(ipv4_header_t) + sizeof(udp_header_t) +
                        sizeof(dhcp_packet_t));
  ip->up_id = htons(1);
  ip->flags_fragoff = 0;
  ip->ttl = 64;
  ip->protocol = PROTOCOL_UDP;
  ip->src_ip = 0;
  ip->dest_ip = 0xFFFFFFFF;
  ip->checksum = 0;
  ip->checksum = ipv4_checksum((uint16_t *)ip, sizeof(ipv4_header_t));

  udp->src_port = htons(68);
  udp->dest_port = htons(67);
  udp->len = htons(sizeof(udp_header_t) + sizeof(dhcp_packet_t));
  udp->checksum = 0;

  dhcp->op = 1;
  dhcp->htype = 1;
  dhcp->hlen = 6;
  dhcp->xid = htonl(0x12345678);
  dhcp->flags = htons(0x8000);

  for (int i = 0; i < 6; i++) {
    dhcp->chaddr[i] = my_mac[i];
  }
  dhcp->magic_cookie = htonl(DHCP_MAGIC_COOKIE);

  uint8_t *opt = dhcp->options;

  *opt++ = 53;
  *opt++ = 1;
  *opt++ = 1;

  *opt++ = 55;
  *opt++ = 3;
  *opt++ = 1;
  *opt++ = 3;
  *opt++ = 6;

  *opt++ = 255;

  uint16_t total_len = sizeof(ethernet_header_t) + sizeof(ipv4_header_t) +
                       sizeof(udp_header_t) + sizeof(dhcp_packet_t);

  e1000_send_packet(packet, total_len);

  console_print("[DHCP] Discover broadcast sent\n");
}
