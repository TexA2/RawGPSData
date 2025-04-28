cmake_minimum_required(VERSION 3.8)
project(ubox_gps)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic -lminmea)
endif()


find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()


find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)


include_directories(
  include
)

link_directories(/usr/local/lib)

add_executable(ublox_gps src/Ublox_node.cpp)

target_link_libraries(ublox_gps minmea)


ament_target_dependencies(ublox_gps
  rclcpp
  sensor_msgs
  std_msgs
)

install(
 TARGETS ublox_gps
 DESTINATION lib/${PROJECT_NAME}
)

ament_package()
