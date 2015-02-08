#include <stdio.h>
#include <netinet/in.h>     
#include <netinet/ip.h>
#include <arpa/inet.h>      /* inet_ntop */
#include <netinet/ip.h>     /* struct iphdr */
#include <netdb.h>          /* /etc/protocols */
#include <endian.h>

#include "print.h"
#include "authtype.h"
#include "msgtype.h"
#include "options.h"
#include "tlv.h"
#include "chksum.h"
#include "verbose.h"

void printOSPFHelloOptions(uint8_t options);

void printOSPFHello(struct OSPFhello * hello, uint32_t * neighbors, uint8_t neighborc)
{
  char mask [INET_ADDRSTRLEN], dr [INET_ADDRSTRLEN], bdr [INET_ADDRSTRLEN], neighbor [INET_ADDRSTRLEN];
  inet_ntop (AF_INET, &(hello->netmask), mask, INET_ADDRSTRLEN);
  inet_ntop (AF_INET, &(hello->dr), dr, INET_ADDRSTRLEN);
  inet_ntop (AF_INET, &(hello->bdr), bdr, INET_ADDRSTRLEN);

  printf("\n*** OSPF Hello Packet ***\n");
  printf("Network Mask: %s\n", mask);
  printf("Hello Interval [sec]: %d\n", be16toh(hello->hellointerval));
  printf("Options: 0x%02x\n", hello->options);
  printOSPFHelloOptions(hello->options);
  printf("Router Priority: %d\n", hello->priority);
  printf("Router Dead Interval [sec]: %d\n", be32toh(hello->deadint));
  printf("Designated Router: %s\n", dr);
  printf("Backup Designated Router: %s\n", bdr);

  for (int i = 0; i < neighborc; i++)
    {
      inet_ntop (AF_INET, &neighbors[i], neighbor, INET_ADDRSTRLEN);
      printf("Active Neighbor: %s\n", neighbor);
    }
}

void printOSPFHelloOptions(uint8_t options)
{
  uint8_t dn, o, dc, l, np, mc, e, mt;

  dn = (options & OSPF_OPTION_DN) >> 7;
  o  = (options & OSPF_OPTION_O ) >> 6;
  dc = (options & OSPF_OPTION_DC) >> 5;
  l  = (options & OSPF_OPTION_L ) >> 4;
  np = (options & OSPF_OPTION_NP) >> 3;
  mc = (options & OSPF_OPTION_MC) >> 2;
  e  = (options & OSPF_OPTION_E ) >> 1;
  mt = options & OSPF_OPTION_MT;

  
  printf("  %d... .... = DN: %s\n", dn, dn ? "Set" : "Not set");
  printf("  .%d.. .... = O: %s\n", o, o ? "Set" : "Not set");
  printf("  ..%d. .... = DC: Demand Circuits are %ssupported\n", dc, dc ? "" : "NOT ");
  printf("  ...%d .... = L: This packet does %scontain LLS data block\n", l, l ? "" : "NOT ");
  printf("  .... %d... = NP: NSSA is %ssupported\n", np, np ? "" : "NOT ");
  printf("  .... .%d.. = MC: %sMulticast Capable\n", mc, mc ? "" : "NOT ");
  printf("  .... ..%d. = E: %sExternal Routing Capability\n", e, e ? "" : "NO ");
  printf("  .... ...%d = MT: %sMulti-Topology Routing\n", mt, mt ? "" : "NO ");
}
