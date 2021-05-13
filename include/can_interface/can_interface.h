#ifndef CAN_INTERFACE__CAN_INTERFACE_H
#define CAN_INTERFACE__CAN_INTERFACE_H

#include <unistd.h>
#include <cstdint>
#include <cstdio>
#include <string.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <vector>

namespace can_interface
{
  class CanInterface
  {
    public:
      CanInterface();
      virtual ~CanInterface();

      bool sendMessage(uint16_t id, uint8_t size, const std::vector<uint8_t> &data);
      bool readMessage(uint16_t canId, std::vector<uint8_t> &msg);
      bool setupSocket(int& sckt, ifreq& ifr, sockaddr_can& addr);

    private:
      int scktRead_, scktWrite_;
      struct ifreq ifrRead_, ifrWrite_;
      struct sockaddr_can addrRead_, addrWrite_;
      
    
  };
}

#endif // CAN_INTERFACE__CAN_INTERFACE_H