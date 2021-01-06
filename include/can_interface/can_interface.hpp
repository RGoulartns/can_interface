#ifndef CAN_INTERFACE__CAN_INTERFACE_HPP_
#define CAN_INTERFACE__CAN_INTERFACE_HPP_

#include <string>
#include <unistd.h>
#include <string.h>

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>


namespace can_interface
{

   /**
   * @class CanInterface
   * @brief Class provides methods for an application to interface with the devices via CAN bus.
   */
  class CanInterface
  {
    public:
      /**
       * @brief  Constructor - No params needed
       */
      CanInterface();

      /**
       * @brief  Destructor - Cleans up
       */
      virtual ~CanInterface();

      /**
       * @brief Send a can command based on a pre-defined list of options
       * @param command Available options: 
       *            0 - Switch interlock to KSI
       *            1 - Set PDO timeout to 200ms
       *            2 - Switch to operational mode
       * @return True if the command is sent successfully, false otherwise
       */
      bool sendCommands(double command);

      /**
       * @brief Send a can command based on a pre-defined list of options
       * @param command array used for cycling PDO at 100ms for VCL_Throttle input
       *            command[0] must be '3'
       *            command[1] left throttle command (0 ~ 65536)
       *            command[2] right throttle command (0 ~ 65536)
       * @param lrpm rpm of the left wheel reported by the motor controller
       * @param rrpm rpm of the right wheel reported by the motor controller
       * @return True if the command is sent successfully, false otherwise
       */      
      bool sendCommands(double commandArgs[], int& lrpm, int&rrpm);


    private:

      /**
       * @brief Setup the variables used for the connection
       * @return True if the socket is created successfully, false otherwise
       */ 
      bool setupSocket();

      /**
       * @brief Creates the can_frames arrays (left and right motors)
       * @param args array with the arguments required
       *            0 - command option
       *            1 - left throttle
       *            2 - right throttle
       */ 
      void createCanFrame(double args[]);

      /**
       * @brief Send a can command based on a pre-defined list of options
       * @param id ID of the device to extract the RPM from. The RPM is located in the data[5] and data[4]
       * @return The RPM value for the chosen motor
       */ 
      int extractRPM(unsigned short int id);

      // convert throttle to a range used by CAN bus (0~65535)
      int throttleToRange(double throttle);

      //sockets variables
      int sckt_;
      struct sockaddr_can addr_;
      struct ifreq ifr_;

      //frames to store can bus messages
      struct can_frame leftCommand_, rightCommand_;
  };
  
}  // namespace can_interface

#endif  // CAN_INTERFACE__CAN_INTERFACE_HPP_
