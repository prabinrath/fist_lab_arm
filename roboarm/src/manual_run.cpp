#include <cmath>
#include <cstdlib>
#include <ctime>
#include <geometry_msgs/Twist.h>
#include <tf/transform_datatypes.h>
#include <iostream>
//#include <sensor_msgs/JointState.h>
//#include <sensor_msgs/Imu.h>
#include <nav_msgs/Odometry.h>
//#include <std_msgs/Empty.h>
//#include <kobuki_msgs/MotorPower.h>
#include <kobuki_msgs/BumperEvent.h>
#include <kobuki_msgs/CliffEvent.h>
#include <ros/ros.h>

/* Information from the gazebo simulation
/joint_states
/mobile_base/commands/motor_power
/mobile_base/commands/reset_odometry
/mobile_base/commands/velocity
/mobile_base/events/bumper
/mobile_base/events/cliff
/mobile_base/sensors/imu_data
/odom
*/

namespace roboarm
{
  class KobukiController
  {
    ros::NodeHandle nh_;
    ros::Subscriber bumper_event, cliff_event, odometry_feedback;
    ros::Publisher cmd_vel;
    nav_msgs::Odometry latest;

    bool bumper_left,bumper_center,bumper_right,cliff_left,cliff_center,cliff_right;

    void bumperEvent(const kobuki_msgs::BumperEventConstPtr msg);
    void cliffEvent(const kobuki_msgs::CliffEventConstPtr msg);
    void odometryFeedback(const nav_msgs::OdometryConstPtr msg);
    public:
      KobukiController(ros::NodeHandle& nh);
      void actuate(double surge, double yaw, float kp);
  };

  KobukiController::KobukiController(ros::NodeHandle& nh):nh_(nh)
  {
    bumper_event = nh_.subscribe("/mobile_base/events/bumper", 10, &KobukiController::bumperEvent, this);
    cliff_event = nh_.subscribe("/mobile_base/events/cliff", 10, &KobukiController::cliffEvent, this);
    odometry_feedback = nh_.subscribe("/odom", 10, &KobukiController::odometryFeedback, this);
    cmd_vel = nh_.advertise<geometry_msgs::Twist>("/mobile_base/commands/velocity", 10);
  }

  void KobukiController::bumperEvent(const kobuki_msgs::BumperEventConstPtr msg)
  {
    if (msg->state == kobuki_msgs::BumperEvent::PRESSED)
    {
      switch (msg->bumper)
      {
        case kobuki_msgs::BumperEvent::LEFT:
          bumper_left = true;
          break;
        case kobuki_msgs::BumperEvent::CENTER:
          bumper_center = true;
          break;
        case kobuki_msgs::BumperEvent::RIGHT:
          bumper_right = true;
          break;
      }
    }
    else
    {
      switch (msg->bumper)
      {
        case kobuki_msgs::BumperEvent::LEFT:    bumper_left  = false; break;
        case kobuki_msgs::BumperEvent::CENTER:  bumper_center = false; break;
        case kobuki_msgs::BumperEvent::RIGHT:   bumper_right  = false; break;
      }
    }
  }

  void KobukiController::cliffEvent(const kobuki_msgs::CliffEventConstPtr msg)
  {
    if (msg->state == kobuki_msgs::CliffEvent::CLIFF)
    {
      switch (msg->sensor)
      {
        case kobuki_msgs::CliffEvent::LEFT:
          cliff_left = true;
          break;
        case kobuki_msgs::CliffEvent::CENTER:
          cliff_left = true;
          break;
        case kobuki_msgs::CliffEvent::RIGHT:
          cliff_left = true;
          break;
      }
    }
    else
    {
      switch (msg->sensor)
      {
        case kobuki_msgs::CliffEvent::LEFT:    cliff_left  = false; break;
        case kobuki_msgs::CliffEvent::CENTER:  cliff_center = false; break;
        case kobuki_msgs::CliffEvent::RIGHT:   cliff_right = false; break;
      }
    }
  }

  void KobukiController::odometryFeedback(const nav_msgs::OdometryConstPtr msg)
  {
    latest = *msg;
  }

  void KobukiController::actuate(double surge, double target_yaw, float kp)
  {
    tf::Quaternion q(
    latest.pose.pose.orientation.x,
    latest.pose.pose.orientation.y,
    latest.pose.pose.orientation.z,
    latest.pose.pose.orientation.w);
    tf::Matrix3x3 m(q);
    double roll, pitch, yaw;
    m.getRPY(roll, pitch, yaw);
    double error = target_yaw - yaw;
    geometry_msgs::Twist target;
    target.angular.z = kp*error;
    target.linear.x = surge;
    cmd_vel.publish(target);
  }
}

int main(int argc, char** argv)
{
  ros::init (argc, argv, "run_kobuki_robot");
  ros::NodeHandle nh;

  roboarm::KobukiController obj(nh);
  ros::Rate loop_rate(20);
  double yaw = 0;
  while(true)
  {
    obj.actuate(0.8,yaw*(M_PI/180),4);
    ros::spinOnce();
    loop_rate.sleep();
  }
}