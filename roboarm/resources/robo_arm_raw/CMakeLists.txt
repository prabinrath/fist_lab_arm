cmake_minimum_required(VERSION 2.8)
add_definitions(-std=c++11)
project( Robo_Arm )

include_directories(include)

add_executable(writearm src/write_data.cpp include/SerialLib.h src/SerialLib.cpp)
add_executable(readarm src/read_data.cpp include/SerialLib.h src/SerialLib.cpp)
add_executable(relative_arm_feedback src/relative_arm_feedback.cpp include/SerialLib.h src/SerialLib.cpp)
target_link_libraries(relative_arm_feedback pthread)
add_executable(absolute_arm_feedback src/absolute_arm_feedback.cpp include/SerialLib.h src/SerialLib.cpp)
target_link_libraries(absolute_arm_feedback pthread)
