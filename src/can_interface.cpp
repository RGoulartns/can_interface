#include "can_interface/can_interface.hpp"

namespace can_interface
{

  CanInterface::CanInterface(){}

  CanInterface::~CanInterface(){}


  bool CanInterface::setupSocket()
  {
    if((sckt_ = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
    {
      perror("Error while opening socket");
      return false;
    }

    strcpy(ifr_.ifr_name, "can0");
    ioctl(sckt_, SIOCGIFINDEX, &ifr_);

    addr_.can_family  = AF_CAN;
    addr_.can_ifindex = ifr_.ifr_ifindex;

    if(bind(sckt_, (struct sockaddr *)&addr_, sizeof(addr_)) < 0)
    {
      perror("Error in socket bind");
      return false;
    }
    
    return true;
  }

  void CanInterface::createCanFrame(double args[])
  {
    switch((int)args[0])
    {
      case 0:
        // Switch interlock to KSI
        leftCommand_.can_id = 0x626;
        leftCommand_.can_dlc = 5;
        leftCommand_.data[0] = 0x22;
        leftCommand_.data[1] = 0x3e;
        leftCommand_.data[2] = 0x30;
        leftCommand_.data[3] = 0x00;
        leftCommand_.data[4] = 0x02;

        rightCommand_.can_id = 0x627;
        rightCommand_.can_dlc = 5;
        rightCommand_.data[0] = 0x22;
        rightCommand_.data[1] = 0x3e;
        rightCommand_.data[2] = 0x30;
        rightCommand_.data[3] = 0x00;
        rightCommand_.data[4] = 0x02;
        break;
      case 1:
        // Set PDO timeout to 200ms
        leftCommand_.can_id = 0x626;
        leftCommand_.can_dlc = 5;
        leftCommand_.data[0] = 0x22;
        leftCommand_.data[1] = 0x49;
        leftCommand_.data[2] = 0x31;
        leftCommand_.data[3] = 0x00;
        leftCommand_.data[4] = 0x32;

        rightCommand_.can_id = 0x627;
        rightCommand_.can_dlc = 5;
        rightCommand_.data[0] = 0x22;
        rightCommand_.data[1] = 0x49;
        rightCommand_.data[2] = 0x31;
        rightCommand_.data[3] = 0x00;
        rightCommand_.data[4] = 0x32;
        break;
      case 2:
        // Enter operational mode
        leftCommand_.can_id = 0x00;
        leftCommand_.can_dlc = 2;
        leftCommand_.data[0] = 0x01;
        leftCommand_.data[1] = 0x26;

        rightCommand_.can_id = 0x00;
        rightCommand_.can_dlc = 2;
        rightCommand_.data[0] = 0x01;
        rightCommand_.data[1] = 0x27;
        break;
      case 3:
        // Cycle PDO at 100ms for VCL_Throttle input (data[2]&[3])
        leftCommand_.can_id = 0x226;
        leftCommand_.can_dlc = 4;
        leftCommand_.data[0] = 0x00;
        leftCommand_.data[1] = 0x00;
        leftCommand_.data[2] = throttleToRange(args[1]) & 255;
        leftCommand_.data[3] = (throttleToRange(args[1]) >> 8) & 255;

        rightCommand_.can_id = 0x227;
        rightCommand_.can_dlc = 4;
        rightCommand_.data[0] = 0x00;
        rightCommand_.data[1] = 0x00;
        rightCommand_.data[2] = throttleToRange(args[2]) & 255;
        rightCommand_.data[3] = (throttleToRange(args[2]) >> 8) & 255;
        break;
      default:
        //error
        break;
    }
  }

  int CanInterface::extractRPM(unsigned short int id)
  {
    short int readTimeout = 0;
    int rpm = 0;
    struct can_frame canMessage;

    do
    {
      read(sckt_, &canMessage, sizeof(struct can_frame));
      readTimeout++;
    } while (canMessage.can_id  != id || readTimeout > 5000);

    rpm = (canMessage.data[5] << 8) + canMessage.data[4];
    if(rpm > 30000)
      rpm -= 65536;

    return rpm;
  }

  bool CanInterface::sendCommands(double command)
  {
    //prepare message
    double option[] = {command};
    createCanFrame(option);

    //setup socket
    if(!setupSocket())
      return 1;

    //send commands and read response if required
    write(sckt_, &leftCommand_, sizeof(struct can_frame));
    usleep(100000);
    write(sckt_, &rightCommand_, sizeof(struct can_frame));

    close(sckt_);
    return 0;
  }


  bool CanInterface::sendCommands(double commandArgs[], int& lrpm, int&rrpm)
  {
    //prepare message
    createCanFrame(commandArgs);

    //setup socket
    if(!setupSocket())
      return 1;

    //send commands and read response if required
    write(sckt_, &leftCommand_, sizeof(struct can_frame));
    lrpm = extractRPM(0x1A6);

  
    write(sckt_, &rightCommand_, sizeof(struct can_frame));
    rrpm = extractRPM(0x1A7);

    close(sckt_);
    return 0;
  }

  int CanInterface::throttleToRange(double throttle)
  {
    return 32767 * throttle + 65535*(throttle<0);
  }
  
}  // namespace can_interface

