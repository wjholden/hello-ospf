#include "authtype.h"

const char * authTypes [] = {
  "Null",
  "Simple password",
  "Cryptographic authentication"
};
const char * invalid = "Reserved (probably invalid)";

const char * getOSPFAuthType (unsigned short autype)
{
  const char * r;

  if (autype > 2)
    r = invalid;
  else
    r = authTypes[autype];
  return r;
}
