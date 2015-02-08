/* This algorithm is derived from Wright & Stevens "TCP/IP Illustrated Vol 2" (1995).
   Values summed are left in network-byte order. Thanks to an interesting property
   of one's complement arithmetic, this doesn't matter. Do not change the byte order
   of the result. See http://www.netfor2.com/checksum.html for more insight. */
uint16_t chksum (void * buffer, uint16_t bytes);
