#include <stdio.h>
#include <stdlib.h>         /* calloc */
#include <netinet/ip.h>     /* struct iphdr */
#include <time.h>           /* needed to generate random numbers */
#include <string.h>         /* strerror */
#include <arpa/inet.h>      /* inet_pton */
#include <netdb.h>          /* /etc/protocols */

#include "init.h"

uint16_t randomIdentifier ()
{
  srand(time(NULL));
  return (uint16_t) rand();
}

void initIP (struct iphdr * ip, uint32_t address, uint8_t ttl)
{
  int status;

  ip->version = 4;                 /* IPv4 */
  ip->ihl     = 5;                 /* No options, header length = 20 bytes */
  ip->tos     = IPTOS_CLASS_CS6;   /* from netinet/ip.h */
  ip->tot_len = 0;                 /* set before calculating chksum */
  ip->id = randomIdentifier();     /* Linux will automatically set this field */
  ip->frag_off = 0;                /* fragmentation is so not cool anymore */
  
  /* From http://tools.ietf.org/html/rfc2328#appendix-A.1
     "To ensure that these packets will not travel multiple hops,
     their IP TTL must be set to 1." */
  ip->ttl = ttl;

  /* http://www.iana.org/assignments/protocol-numbers */
  ip->protocol = getprotobyname("ospf")->p_proto; /* I know it's 89... */

  ip->check = 0;                   /* Must be zero during checksum calculation */
  ip->saddr = address;

  /* Destination IP Address */
  if ((status = inet_pton (AF_INET, "224.0.0.5", &(ip->daddr))) != 1)
    {
      fprintf(stderr, "inet_pton error: %s\n", strerror(status));
      exit(EXIT_FAILURE);
    }
}

void initOSPFHeader(struct OSPFheader * header, uint32_t address, uint32_t area)
{
  header->version = 2;
  header->msgtype = 1;
  /* Skip length */
  header->rid = address;
  header->areaid  = area;
  header->chksum  = 0;
  header->autype  = 0;
  header->authen  = 0;
}

void initOSPFHello(struct OSPFhello * hello, uint32_t address, uint32_t netmask, uint16_t helloInterval, uint32_t deadInterval, uint8_t priority, uint8_t options)
{
  hello->netmask = netmask;
  hello->hellointerval = htobe16(helloInterval);
  hello->options = options;
  hello->priority = priority;
  hello->deadint  = htobe32(deadInterval);
  hello->dr = address;
  hello->bdr = 0;
}
