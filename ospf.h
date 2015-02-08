#pragma once

#include <stdint.h>

#ifndef _OSPF_H
#define _OSPF_H

#define IPPROTO_OSPF 89

/* http://tools.ietf.org/html/rfc2328#appendix-A.3.1 */
struct OSPFheader
{
  /* This program sends OSPFv2 hello packets. */
  uint8_t version;

  /* 1 = Hello
     2 = Database Description
     3 = Link State Request
     4 = Link State Update
     5 = Link State Acknowledgement */
  uint8_t msgtype;

  /* The length of the OSPF packet in bytes,
     including header. */
  uint16_t length;

  /* Router ID of the packet's source. */
  uint32_t rid;

  /* OSPF Area ID. */
  uint32_t areaid;

  /* Copied from RFC 2328:
"The standard IP checksum of the entire contents of the packet, starting with the OSPF packet header but excluding the 64-bit authentication field. This checksum is calculated as the 16-bit one's complement of the one's complement sum of all the 16-bit words in the packet, excepting the authentication field. If the packet's length is not an integral number of 16-bit words, the packet is padded with a byte of zero before checksumming. The checksum is considered to be part of the packet authentication procedure; for some authentication types the checksum calculation is omitted." */
  uint16_t chksum;

  /* Not implemented. */
  uint16_t autype;

  /* Not implemented. */
  uint64_t authen;
};

/* http://tools.ietf.org/html/rfc2328#appendix-A.3.2 */
struct OSPFhello
{
  /* Self-explanatory */
  uint32_t netmask;

  /* Seconds between Hello packets */
  uint16_t hellointerval;
  
  /* http://tools.ietf.org/html/rfc2328#appendix-A.2 */
  uint8_t options;

  /* Router priority for DR/BDR election.
     http://packetlife.net/blog/2011/jun/2/ospf-designated-router-election/
     The highest DR priority wins. The highest Router ID breaks a tie. */
  uint8_t priority;

  /* Seconds until silent neighbor gets dropped. */
  uint32_t deadint;

  /* Router ID of the DR. */
  uint32_t dr;

  /* Router ID of the BDR. */
  uint32_t bdr;
};

struct lls_data_block
{
  uint16_t chksum;

  uint16_t length;
};

struct tlv
{
  uint16_t type;

  uint16_t length;
};

#endif /* _OSPF_H */

