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

void printIP (struct iphdr * ip)
{
  char saddr [INET_ADDRSTRLEN], daddr [INET_ADDRSTRLEN];
  inet_ntop (AF_INET, &(ip->saddr), saddr, INET_ADDRSTRLEN);
  inet_ntop (AF_INET, &(ip->daddr), daddr, INET_ADDRSTRLEN);

  printf("*** Internet Protocol Version 4 ***\n");
  printf("Version: %d\n", ip->version);
  printf("Header Length: %d bytes\n", ip->ihl * 4);
  printf("Differentiated Services Field: %#02x\n", ip->tos);
  printf("Total Length: %d\n", be16toh(ip->tot_len));
  printf("Identification: %#04x\n", be16toh(ip->id));
  printf("Flags: %#02x\n", ip->frag_off);
  printf("Fragment Offset: %d\n", ip->frag_off);
  printf("Time to live: %d\n", ip->ttl);
  printf("Protocol: %s (%d)\n", getprotobynumber(ip->protocol)->p_aliases[0],
	 ip->protocol);
  printf("Header checksum: %#04x\n", be16toh(ip->check));
  printf("Source: %s\n", saddr);
  printf("Destination: %s\n", daddr);
}
