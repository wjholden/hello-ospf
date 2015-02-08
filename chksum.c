#include <stdio.h>
#include <stdint.h>

#include "chksum.h"
#include "verbose.h"

/* See http://www.netfor2.com/checksum.html */
uint16_t chksum (void * buffer, uint16_t bytes)
{
  uint16_t * words = (uint16_t *) buffer;
  uint8_t wc = (uint8_t) (bytes / 2);
  uint32_t sum = 0;
  uint8_t i;

  eprintf("[chksum] bytes = %d\n", bytes);
  eprintf("[chksum] words = %d\n", wc);

 
  for (i = 0 ; i < wc ; i++)
    {
      eprintf("[chksum] sum (%#08x) + word[%2d] (0x%04x) = new sum (%#08x)\n",
	      sum, i, words[i], sum + words[i]);
      sum += words[i];
    }

  /* Catches odd bytes. This should not happen;
     IP headers are supposed to be padded. */
  if (bytes % 2 == 1)
    {
      eprintf("[chksum] sum (%#08x) + odd byte (0x%04x) = new sum (%#08x)\n",
	      sum, words[wc] & 0x00FF, sum + (words[wc] & 0x00FF));
      sum += words[wc] & 0x00FF;
    }
 
  /* Even a massive packet of 65535 bytes can only produce
     two bytes of overflow with all words set to 0xFFFF:
     (2^16-1) * (2^16-1) < 2^32. */
  eprintf("[chksum] sum (%#04x) + overflow (%#04x) = one's complement sum (%#04x)\n",
	  sum & 0xFFFF, (sum >> 16 & 0xFFFF), (sum >> 16) + (sum & 0xFFFF));
  sum = (sum >> 16) + (sum & 0xFFFF);


  eprintf("[chksum] One's complement of one's complement sum = %#04x\n", ~sum);
  return ~sum;
}
