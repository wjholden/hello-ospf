#pragma once

#ifndef _OSPF_LISTEN_H
#define _OSPF_LISTEN_H

#include <net/if.h>

int receive (int sd, long int timeout_sec, long int timeout_usec);

#endif /* _OSPF_LISTEN_H */
