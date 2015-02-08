#include <stdio.h>
#include <stdlib.h>
#include <netinet/ip.h>     /* struct iphdr */
#include <unistd.h>         /* read */
#include <sys/socket.h>
#include <string.h>         /* strerror */
#include <argp.h>
#include <stdbool.h>
#include <net/if.h>         /* struct ifreq */
#include <sys/ioctl.h>      /* SIOCGIFINDEX */
#include <bits/ioctls.h>

#include "ospf.h"
#include "chksum.h"
#include "msgtype.h"
#include "authtype.h"
#include "print.h"
#include "init.h"
#include "getiface.h"
#include "verbose.h"
#include "options.h"
#include "receive.h"

const char * argp_program_bug_address = "wjholden@gmail.com";
const char * argp_program_version = "0.9";

#define PCKT_LEN 8192

bool verbose = false;
uint16_t helloInterval = 10;
uint32_t deadInterval = 40;
char * iface = NULL;
uint8_t priority = 0;
uint8_t options = 0;
uint8_t ttl = 1;
uint32_t area = 0;

enum speed { SLOW, NORMAL, FAST };
enum speed speed = NORMAL;

struct argp_option argument_options[] =
  {
    { 0, 0, 0, 0, "Program Options:", 5 },
    { "verbose", 'v', 0, 0, "explain what is being done", 5 },
    { "FAST", 'f', 0, 0, "stop listening for hellos after just 250ms", 5 },
    { "SLOW", 's', 0, 0, "wait full dead timer for responses", 5 },

    { 0, 0, 0, 0, "Network Options:", 10 },
    { "interval", 'i', "seconds", 0, "hello interval (default: 10)", 10},
    { "dead", 'd', "seconds", 0, "router dead interval (default: 40)", 10},
    { "priority", 'p', "[0-255]", 0, "priority (default: 0)", 10},   
    { "interface", 'I', "interface", 0, "network interface", 10 },
    { "ttl", 't', "[0-255]", 0, "time to live (default: 1)", 10},

    { 0, 0, 0, 0, "OSPF Option Flags:", -10},
    { "dn", 'D', 0, 0, "set down bit for RFC 2547 BGP/MPLS VPNs", -10 },
    { "opaque", 'O', 0, 0, "set the O bit to support Opaque LSAs", -10 },
    { "dc", 'C', 0, 0, "set the DC bit to support Demand Circuits", -10 },
    { "lls", 'L', 0, 0, "set L bit for RFC 4813 Link-Local Signaling", -10 },
    { "nssa", 'N', 0, 0, "set N bit for Type-7 NSSA-External-LSAs", -10 },
    { "propogate", 'P', 0, 0, "set P bit to translate Type-7 LSA to Type-5", -10 },
    { "mospf", 'M', 0, 0, "set MC bit for Multicast extensions to OSPF", -10 },
    { "external", 'E', 0, 0, "clear E bit for external routing (set by default)", -10 },
    { "multitopology", 'T', 0, 0, "set MT bit for MT-OSPF", -10 },
    
    { 0 }
  };

static int parse_opt (int key, char * arg, struct argp_state * state);

int main (int argc, char ** argv)
{
  struct iphdr * ip;
  struct OSPFheader * header;
  struct OSPFhello * hello;
  
  uint8_t buffer[PCKT_LEN];
  int sd = 0;
  struct ifreq ifr;
  const int on = 1;
  long int timeout_sec, timeout_usec;

  options = OSPF_OPTION_E;

  uint32_t address, netmask;

  struct argp argp = { argument_options, parse_opt, "",
		       "Send and receive and OSPF Hello Packet.", 0, 0, 0 };
  argp_parse (&argp, argc, argv, 0, 0, 0);
  
  ip = (struct iphdr *) buffer;
  header = (struct OSPFheader *) (buffer + sizeof(struct iphdr));
  hello = (struct OSPFhello *) (buffer + sizeof(struct iphdr) +
		 	        sizeof(struct OSPFheader));
  
  if (iface == NULL)
    iface = getDefaultInterface();

  // open the socket
  if ((sd = socket (AF_INET, SOCK_RAW, IPPROTO_OSPF)) < 0)
    {
      perror("socket() failed to get socket descriptor for using ioctl()");
      exit(EXIT_FAILURE);
    }

  if (strlen(iface) < sizeof(ifr.ifr_name))
    {
      strcpy(ifr.ifr_name, iface);
    }
  else
    {
      fprintf(stderr, "%s is too long, may not be a valid interface name.\n", iface);
      exit(EXIT_FAILURE);
    }

  if (ioctl (sd, SIOCGIFINDEX, &ifr) < 0)
    {
      perror("ioctl() failed to find interface");
      exit(EXIT_FAILURE);
    }

  if (ioctl (sd, SIOCGIFADDR, &ifr) < 0)
    {
      perror("Cannot find interface address.");
      exit(EXIT_FAILURE);
    }
  address = (uint32_t) ((struct sockaddr_in *) &ifr.ifr_addr)->sin_addr.s_addr;

  if (ioctl (sd, SIOCGIFNETMASK, &ifr) < 0)
    {
      perror("Cannot find interface netmask.");
      exit(EXIT_FAILURE);
    }
  netmask = (uint32_t) ((struct sockaddr_in *) &ifr.ifr_addr)->sin_addr.s_addr;

  initIP(ip, address, ttl);
  initOSPFHeader(header, address, area);
  initOSPFHello(hello, address, netmask, helloInterval, deadInterval, priority, options);
  
  ip->tot_len = htobe16(sizeof(struct iphdr) + sizeof(struct OSPFheader) +
		      sizeof(struct OSPFhello));

  header->length = htobe16(sizeof(struct OSPFheader) + sizeof(struct OSPFhello));

  ip->check = 0;
  ip->check = chksum(ip, ip->ihl * 4); // interesting fact: do not change byte ordering

  header->chksum = 0;
  header->chksum = chksum(header, be16toh(header->length));
  
  if (setsockopt(sd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0)
    {
      perror("setsockopt() failed to set IP_HDRINCL");
      exit(EXIT_FAILURE);
    }

  if (setsockopt (sd, SOL_SOCKET, SO_BINDTODEVICE, &ifr, sizeof(ifr)) < 0)
    {
      perror("setsockopt() failed to bind to interface");
      exit(EXIT_FAILURE);
    }

  if (sendto(sd, buffer, be16toh(ip->tot_len), 0, &(ifr.ifr_addr),
	     sizeof(struct sockaddr)) < 0)
    {
      perror("sendto() failed");
      exit(EXIT_FAILURE);
    }
  switch (speed)
    {
    case SLOW: timeout_sec = (long int) deadInterval; timeout_usec = 0; break;
    case NORMAL: timeout_sec = 1; timeout_usec = 0; break;
    case FAST: timeout_sec = 0; timeout_usec = 250000; break;
    }
  receive(sd, timeout_sec, timeout_usec);
  close(sd);

  return(EXIT_SUCCESS);
}

static int parse_opt (int key, char * arg, struct argp_state * state)
{
  char * err;
  
  switch (key)
    {
    case 'v': verbose = true; break;
    case 'i':
      {
	helloInterval = strtoul(arg, &err, 0);
	if (* err != '\0')
	  argp_failure(state, 1, 0, "invalid hello interval");
	break;
      }
    case 'd':
      {
	deadInterval = strtoul(arg, &err, 0);
	if (* err != '\0')
	  argp_failure(state, 1, 0, "invalid dead interval");
	break;
      }
    case 'I': iface = arg; break;
    case 'p':
      {
	unsigned long int pri = strtoul(arg, &err, 0);
	if (pri > 255 || (* err) != '\0')
	  argp_failure(state, 1, 0, "invalid priority");
	priority = (uint8_t) pri;
	break;
      }
    case 't':
      {
	unsigned long int timetolive = strtoul(arg, &err, 0);
	if (timetolive > 255 || (* err) != '\0')
	  argp_failure(state, 1, 0, "invalid TTL value");
	ttl = (uint8_t) timetolive;
	break;
      }
    case 'D': options = options ^ OSPF_OPTION_DN; break;
    case 'O': options = options ^ OSPF_OPTION_O;  break;
    case 'C': options = options ^ OSPF_OPTION_DC; break;
    case 'L': options = options ^ OSPF_OPTION_L;  break;
    case 'P': options = options ^ OSPF_OPTION_NP; break;
    case 'M': options = options ^ OSPF_OPTION_MC; break;
    case 'E': options = options ^ OSPF_OPTION_E;  break;
    case 'T': options = options ^ OSPF_OPTION_MT; break;
    case 'f': speed = FAST; break;
    case 's': speed = SLOW; break;
    }
  return 0;
}
