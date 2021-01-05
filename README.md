# fist_lab_arm
Robotic arm experiments for dynamic end effector stabilization, RL based motion planning and grasping. The project is under Computer Science and Engineering Department, NIT Rourkela.

## Dependencies
Install all the dependencies of the project using the command `rosdep install -r --from-path src --ignore-src` from workspace folder. Apart from that make sure to have python catkin tools installed (refer https://catkin-tools.readthedocs.io/en/latest/installing.html).

## Build
```
cd catkin_ws/
catkin build
```

## Run
To run the pick and place robot environment based on UR5 arm and Robotiq gripper:
`roslaunch icl_ur5_setup_gazebo icl_ur5_gripper.launch`

To run the dynamic end effector stabilization environment based on UR arm mounted on Husky robot
`roslaunch husky_gazebo huskyarm.launch`

## Acknowledgements
The codes in this repository have been accumulated from a number of open source repositories. Below, a few most signifiact of them are noted for reference. Many modifications were done inorder to customize the simulations according to the project requirements.
1. https://github.com/husky/
2. https://github.com/intuitivecomputing/ur5_with_robotiq_gripper
3. https://github.com/ros-industrial/universal_robot
4. https://github.com/IntelRealSense/realsense-ros
5. https://github.com/roboticsgroup/roboticsgroup_gazebo_plugins
