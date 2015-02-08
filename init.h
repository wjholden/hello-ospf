#include "ospf.h"

void initIP (struct iphdr * ip, uint32_t address, uint8_t ttl);

void initOSPFHeader(struct OSPFheader * header, uint32_t address, uint32_t area);

void initOSPFHello(struct OSPFhello * hello, uint32_t address, uint32_t netmask, uint16_t helloInterval, uint32_t deadInterval, uint8_t priority, uint8_t options);
