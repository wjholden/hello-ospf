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

void printOSPFHeader(struct OSPFheader * header)
{
  char rid [INET_ADDRSTRLEN], areaid [INET_ADDRSTRLEN];
  inet_ntop (AF_INET, &(header->rid), rid, INET_ADDRSTRLEN);
  inet_ntop (AF_INET, &(header->areaid), areaid, INET_ADDRSTRLEN);
  
  printf("\n*** Open Shortest Path First ***\n");
  printf("Version: %d\n", header->version);
  printf("Message Type: %s (%d)\n", getOSPFMessageType(header->msgtype), header->msgtype);
  printf("Packet Length: %d\n", be16toh(header->length));
  printf("Source OSPF Router: %s (%s)\n", rid, rid);
  printf("Area ID: %s (%s)%s\n", areaid, areaid, header->areaid == 0 ? " (Backbone)" : "");
  printf("Checksum: %#04x\n", be16toh(header->chksum));
  printf("Auth Type: %s (%d)\n", getOSPFAuthType(be16toh(header->autype)),
	 be16toh(header->autype));
  printf("Auth Data%s: %016llx\n", header->authen == 0 ? " (none)" : "",
	 (long long unsigned int) be64toh(header->authen));
}
