# CAN bus Interface

can_interface is a generic package that utilizes sockets to communication with devices via CAN bus.
Main branch tested on Ubuntu20 + ROS2 Foxy.
Tested using the following canbus driver: [pcan](https://www.peak-system.com/fileadmin/media/linux/index.htm)

## CAN bus setup
```sh
sudo ip link set can0 type can bitrate 125000
sudo ip link set up can0
```

## Usage Example
```c++
auto canInterface_ = std::make_unique<can_interface::CanInterface>();
canInterface_->sendCommands(command, lRPM, rRPM);
```
