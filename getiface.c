#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <sys/socket.h>

#include "getiface.h"
#include "verbose.h"

static char iface[16];

/* https://trac.transmissionbt.com/browser/trunk/third-party/libnatpmp/getgateway.c */
char * getDefaultInterface ()
{
  char line[1024], interface[16];
  uint64_t destination, gateway, mask, flags;
  int refcnt, use, metric;
  FILE * f;
  int lines = 0;

  if ((f = fopen("/proc/net/route", "r")) == NULL)
    {
      perror("fopen");
      exit(EXIT_FAILURE);
    }

  eprintf("%s\n", "[getaddr] Iface \tDestination\tGateway\t\tFlags\tRefCnt\tUse\tMetric\tMask");

  while (fgets(line, 1024, f))
    {
      if (lines > 0 && strlen(line) > 0)
	{
	  sscanf(line, "%s\t%lx%lx%lx%d%d%d%lx", interface, &destination, &gateway, &flags, &refcnt, &use, &metric, &mask);

	  eprintf("[getaddr] %-6s\t%08lX\t%08lX\t%04lX\t%d\t%d\t%d\t%08lX\n", interface, destination, gateway, flags, refcnt, use, metric, mask);

	  if (destination == 0 && mask == 0)
	    strncpy(iface, interface, 16);
	}
      lines++;
    }

  if (fclose(f) != 0)
    {
      perror("fclose");
      exit(EXIT_FAILURE);
    }

  if (strlen(iface) == 0)
    {
      fprintf(stderr, "No default interface found!\n");
      exit(EXIT_FAILURE);
    }

  eprintf("[getaddr] Found default interface: %s\n", iface);

  return iface;
}
