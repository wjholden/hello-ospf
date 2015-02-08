#include <netinet/ip.h>     /* struct iphdr */

#include "ospf.h"

void printIP (struct iphdr * ip);

void printOSPFHeader (struct OSPFheader * header);

void printOSPFHello (struct OSPFhello * hello, uint32_t * neighbors, uint8_t neighborc);

void printLLSDataBlock (struct lls_data_block * lls);
