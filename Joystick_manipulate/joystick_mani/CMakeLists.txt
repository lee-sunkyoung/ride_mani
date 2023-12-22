cmake_minimum_required(VERSION 3.0.2)
project(joystick_mani)

find_package(catkin REQUIRED COMPONENTS
  message_generation
  geometry_msgs
  roscpp
  sensor_msgs
  std_msgs
  tutorial_msgs
)



generate_messages(DEPENDENCIES std_msgs)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES joystick_mani
  CATKIN_DEPENDS geometry_msgs roscpp sensor_msgs std_msgs message_generation
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

add_executable(joystick_mani src/joy_cmd_vel_mani.cpp)
target_link_libraries(joystick_mani ${catkin_LIBRARIES})
add_dependencies(joystick_mani ${${joystick_mani}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})