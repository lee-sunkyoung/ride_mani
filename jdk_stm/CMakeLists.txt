cmake_minimum_required(VERSION 2.8.3)
project(jdk_stm)

find_package(catkin REQUIRED COMPONENTS
  message_generation
  roscpp
  std_msgs
  serial
  geometry_msgs
  tutorial_msgs
 
)

#generate_messages(DEPENDENCIES std_msgs)

catkin_package(
   INCLUDE_DIRS inc
   LIBRARIES ${PROJECT_NAME}
   CATKIN_DEPENDS roscpp std_msgs geometry_msgs #joystick_mani
   
)


include_directories(inc ${catkin_INCLUDE_DIRS})

add_executable(${PROJECT_NAME}
  src/main.cpp
  src/node.cpp
  src/stm_read.cpp
  src/stm_write.cpp
  src/test.cpp)

target_link_libraries(jdk_stm ${catkin_LIBRARIES})





install(
    TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
    LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
    RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION}
)

install(
    DIRECTORY inc/${PROJECT_NAME}/
    DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
)
