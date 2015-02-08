#include <stdio.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <unistd.h>       /* close */
#include <sys/time.h>     /* struct timeval */
#include <endian.h>

#include "receive.h"
#include "ospf.h"
#include "options.h"
#include "print.h"
#include "verbose.h"

int packets = 0;

int receive (int sd, long int timeout_sec, long int timeout_usec)
{
  uint8_t buffer[65536];
  unsigned int saddr_size;
  int data_size;
  struct sockaddr saddr;
  struct timeval timeout;

  timeout.tv_sec = timeout_sec;
  timeout.tv_usec = timeout_usec;

  //  sd = socket(AF_INET, SOCK_RAW, IPPROTO_OSPF);

  if (sd < 0)
    {
      perror("socket() failed");
      exit(EXIT_FAILURE);
    }

  if (setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout)) < 0)
    {
      perror("setsockopt() failed");
      exit(EXIT_FAILURE);
    }

  while (1)
    {
      saddr_size = sizeof(saddr);
      data_size = recvfrom(sd, buffer, 65536, 0, &saddr, &saddr_size);
      if (data_size < 0)
	{
	  break; // This might indicate an error, but it's more likely we timed out.
	}
      struct iphdr * ip;
      struct OSPFheader * header;
      struct OSPFhello * hello;
      uint32_t * neighbors;
      uint8_t neighborc = 0;
      bool lls_set;
      struct lls_data_block * lls;

      ip = (struct iphdr *) buffer;
      header = (struct OSPFheader *) ((uint8_t *) ip + (ip->ihl * 4));
      hello = (struct OSPFhello *) ((uint8_t *) header + sizeof(struct OSPFheader));
      neighborc = (be16toh(header->length) - sizeof(struct OSPFheader) - sizeof(struct OSPFhello)) / 4;

      if (verbose)
	printf("Looks like we've got %d neighbors.\n", neighborc);

      if (neighborc > 0)
	//	neighbors = (uint32_t *) (buffer + (ip->ihl * 4) + sizeof(struct OSPFheader) + sizeof(struct OSPFheader));
	neighbors = (uint32_t *) ((uint8_t *) hello + sizeof(struct OSPFhello));

      lls_set = ((hello->options & OSPF_OPTION_L) > 0);

      if (lls_set && (be16toh(ip->tot_len) > (ip->ihl * 4) + be16toh(header->length)))
	{
	  //lls = (struct lls_data_block *) (buffer + (ip->ihl * 4) + sizeof(struct OSPFheader) + sizeof(struct OSPFheader) + (neighborc * 4));
	  lls = (struct lls_data_block *) (&neighbors[neighborc]);
	}
      else
	{
	  fprintf(stderr, "LLS bit set, but packet length is just %d, which is less than %d + %d.\n", be16toh(ip->tot_len), ip->ihl * 4, header->length);
	}

      printIP(ip);
      printOSPFHeader(header);
      printOSPFHello(hello, neighbors, neighborc);

      if (lls)
	printLLSDataBlock(lls);

      packets++;
    }
  printf("%d OSPF hello %s received.\n", packets, (packets > 1) ? "packets" : "packet");
  close(sd);
  return (EXIT_SUCCESS);
}
