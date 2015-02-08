#include "msgtype.h"

const char * getOSPFMessageType (unsigned short msgtype)
{
  const char * r;
  const char * messageTypes [] = {
    "Hello Packet",
    "Database Description",
    "Link State Request",
    "Link State Update",
    "Link State Acknowledgement"
  };
  const char * invalid = "Invalid";
  
  if (msgtype > 5 || msgtype == 0)
    r = invalid;
  else
    r = messageTypes[msgtype - 1];
  return r;
}
