#include <can_interface/can_interface.h>

// might be useful: https://stackoverflow.com/questions/21135392/socketcan-continuous-reading-and-writing

 /**
  * TODO
  *  - Try to open socket once and close it with destructor. Will need to deal with potential exceptions ><
  **/

namespace can_interface
{
  CanInterface::CanInterface(){}

  CanInterface::~CanInterface(){}

  bool CanInterface::sendMessage(uint16_t id, uint8_t size, const std::vector<uint8_t> &data)
  {
    // setup socket
    if(!setupSocket(scktWrite_, ifrWrite_, addrWrite_))
      return false;

    // create can frame
    struct can_frame canFrame;
    canFrame.can_id = id;
    canFrame.can_dlc = size;
    uint8_t i = 0;
    for(auto &e : data)
      canFrame.data[i++] = e;
    
    // send can frame
    write(scktWrite_, &canFrame, sizeof(struct can_frame));
    close(scktWrite_);
    
    return true;
  }

  bool CanInterface::readMessage(uint16_t canId, std::vector<uint8_t> &msg)
  {
    uint16_t retries = 0;
    const uint8_t timeout = 20;
    struct can_frame canFrame;

    // setup socket
    if(!setupSocket(scktRead_, ifrRead_, addrRead_))
      return false;

    do
    {
      read(scktRead_, &canFrame, sizeof(struct can_frame));
    } while (canFrame.can_id  != canId && ++retries <= timeout);

    close(scktRead_);

    if(retries < timeout)
    {
      msg.insert( msg.begin(), std::begin(canFrame.data), std::end(canFrame.data) );
      return true;
    }
    else
    {
      return false;
    }
  }

  //source: https://forum.peak-system.com/viewtopic.php?t=5788
  bool CanInterface::setupSocket(int& sckt, ifreq& ifr, sockaddr_can& addr)
  {
    if((sckt = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
    {
      perror("Error while opening socket");
      return false;
    }

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 1000;
    setsockopt(sckt, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);


    strcpy(ifr.ifr_name, "can0");
    ioctl(sckt, SIOCGIFINDEX, &ifr);

    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if(bind(sckt, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
      perror("Error in socket bind");
      return false;
    }
  
    return true;
  }
}