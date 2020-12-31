cmake_minimum_required(VERSION 2.8.3)
project(roboarm)
add_definitions(-std=c++0x)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  sensor_msgs
  gazebo_msgs
  geometry_msgs
  # kobuki_msgs
  nav_msgs
  tf
)

catkin_package()

###########
## Build ##
###########

include_directories(include ${catkin_INCLUDE_DIRS})

# add_library(dst_lib src/test.cpp include/ROBOARM/include.h)
# target_link_libraries(robo_arm ${catkin_LIBRARIES})

# add_executable(run_kobuki src/manual_run.cpp)
# target_link_libraries(run_kobuki ${catkin_LIBRARIES})

add_executable(arm_control src/arm_motion.cpp)
target_link_libraries(arm_control ${catkin_LIBRARIES})

add_executable(defs src/defs.cpp)
target_link_libraries(defs ${catkin_LIBRARIES})
