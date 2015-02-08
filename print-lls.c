#include <stdio.h>
#include <endian.h>

#include "print.h"
#include "tlv.h"
#include "chksum.h"
#include "verbose.h"

/* https://tools.ietf.org/html/rfc5613 */
void printLLSDataBlock(struct lls_data_block * lls)
{
  struct tlv * tlv;
  uint32_t l = be16toh(lls->length) * 4;

  /* Think of this as an array index to read through the struct
     as a byte array. We skip past the first two values (checksum
     and length) and start evaluating each TLV. */
  uint16_t bytes_read = sizeof(struct lls_data_block);

  uint16_t sum = be16toh(lls->chksum);
  lls->chksum = 0;

  printf("\n*** OSPF LLS Data Block ***\n");
  printf("Checksum: %#04x (%scorrect)\n", sum,
	 sum == be16toh(chksum(lls, l)) ? "" : "in");
  printf("LLS Data Length: %d bytes\n", l);
  lls->chksum = be32toh(sum);

  tlv = (struct tlv *) ((uint8_t *) lls + bytes_read);
  eprintf("[print-lls] LLS block is %d bytes total.\tAddress = %p\n", l, (void *) lls);
while (bytes_read < l)
    {
      uint16_t tlv_type, tlv_length;
      uint32_t * tlv_value;

      tlv_type = be16toh(tlv->type);
      tlv_length = be16toh(tlv->length);
      bytes_read += sizeof(struct tlv);

      tlv_value = (uint32_t *) ((uint8_t *) lls + bytes_read);

      eprintf("[print-lls] TLV %d is %d bytes long.\t\tAddress = %p\n",
	      tlv_type, tlv_length, (void *) tlv);

      eprintf("[print-lls] TLV value is %#08x.\t\tAddress = %p\n",
	      be32toh(tlv_value[0]), (void *) tlv_value);

      switch (tlv_type)
	{
	case EOF_TLV: 
	  {
	    uint32_t tlv_value_32, lr, rs;
	    tlv_value_32 = be32toh(tlv_value[0]);
	    lr = tlv_value_32 & EOF_OPTION_LR;
	    rs = tlv_value_32 & EOF_OPTION_RS;
	    printf("Extended Options and Flags\n");
	    printf("  Type: %d\n", tlv_type);
	    printf("  Length: %d\n", tlv_length);
	    printf("  Options: %#08x\n", tlv_value_32);
	    printf("    .... .... .... .... .... .... .... ..%d. = RS: Restart Signal (RS-bit) is %s\n", rs >> 1, rs ? "SET" : "NOT set");
	    printf("    .... .... .... .... .... .... .... ...%d = LR: LSDB Resynchronization (LR-bit) is %s\n", lr , lr ? "SET" : "NOT set");
	    break;
	  }
	case CA_TLV:
	  {
	    uint32_t seq = be32toh(tlv_value[0]);
	    printf("Cryptographic Authentication+\n");
	    printf("  Type: %d\n", tlv_type);
	    printf("  Length: %d\n", tlv_length);
	    printf("  Sequence Number: %#08x\n", seq);
	    /* TODO */
	    printf("  *** Parse/Validate auth is a planned feature not yet implemented. ***\n");
	    break;
	  }
	default:
	  if (tlv_type >= 32768)
	    {
	      uint32_t entnum = be32toh(tlv_value[0]);
	      printf("Private TLV\n");
	      printf("  Type: %d\n", tlv_type);
	      printf("  Length: %d\n", tlv_length);
	      printf("  Private Enterprise Code: %d\n", entnum);
	      /* TODO */
	      printf("  *** Parse private TLV is a planned feature not yet implemented. ***\n");
	    }
	}

      bytes_read += tlv_length;
    }
}
