#include <ros/ros.h>
#include <sensor_msgs/JointState.h>
#include <unistd.h>
#include <math.h>
#include <iostream>

int main(int argc, char **argv)
{
  ros::init(argc, argv, "arm_control");
  ros::NodeHandle n;
  ros::Publisher pub = n.advertise<sensor_msgs::JointState>("/roboarm/arm_controller/target", 100);
  ros::Rate loop_rate(15);

  float pan=0,shld=-1,elbow=2,wrist=-1,grip=0;
  sensor_msgs::JointState msg;
  msg.position.push_back(0);
  msg.position.push_back(0);
  msg.position.push_back(0);
  msg.position.push_back(0);
  msg.position.push_back(0);

  for(int i=0;i<5;i++)
  {
    msg.header.stamp = ros::Time::now();
    msg.position[0] = pan;
    msg.position[1] = shld + M_PI/2;
    msg.position[2] = elbow + M_PI;
    msg.position[3] = wrist + M_PI;
    msg.position[4] = grip;
  	pub.publish(msg);
  	sleep(1);
  }

  bool flag = true;
  while (ros::ok())
  {

  	// if(flag)
  	// {
	  // 	shld+=0.005;
	  // 	elbow+=0.005;
	  // 	wrist+=0.005;
  	// }
  	// else
  	// {
  	// 	shld-=0.005;
	  // 	elbow-=0.005;
	  // 	wrist-=0.005;
  	// }

  	// if(shld>0.7)
  	// {
  	// 	flag = false;
  	// }
  	// if(shld<-0.7)
  	// {
  	// 	flag = true;
  	// }

    std::cout<<"shoulder imu target-> "<<shld*(180/M_PI)<<std::endl;
    std::cout<<"elbow imu target-> "<<elbow*(180/M_PI)<<std::endl;
    std::cout<<"wrist imu target-> "<<wrist*(180/M_PI)<<std::endl;
    msg.header.stamp = ros::Time::now();
    msg.position[0] = pan;
    msg.position[1] = shld + M_PI/2;
    msg.position[2] = elbow + M_PI;
    msg.position[3] = wrist + M_PI;
    msg.position[4] = grip;
    pub.publish(msg);
    
    ros::spinOnce();
    loop_rate.sleep();
  }
  return 0;
}