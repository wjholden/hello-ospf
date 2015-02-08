#pragma once

#ifndef _OSPF_VERBOSE_H
#define _OSPF_VERBOSE_H

#include <stdbool.h>

extern bool verbose;

/* http://stackoverflow.com/a/10017285 */
#define eprintf(format, ...) do {\
    if (verbose > 0)\
      fprintf(stderr, format, __VA_ARGS__);\
  } while (0)

#endif /* _OSPF_VERBOSE_H */
