#include "UboxGPS/NodeGPS.hpp"

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<NodeUbox>());
  rclcpp::shutdown();
  return 0;
}