#include <ros/ros.h>
#include <std_msgs/Float64MultiArray.h>
#include <std_msgs/Float64.h>
#include <message_filters/subscriber.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <sensor_msgs/Imu.h>
#include <sensor_msgs/JointState.h>
#include <math.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <string>
#include <tf/transform_datatypes.h>

typedef message_filters::sync_policies::ApproximateTime<sensor_msgs::JointState, sensor_msgs::Imu, sensor_msgs::Imu, sensor_msgs::Imu> SyncArmFeedback;

class Defs
{
  ros::Publisher joint_output_command,pitch_debug;
  message_filters::Subscriber<sensor_msgs::JointState> joint_input_command;
  message_filters::Subscriber<sensor_msgs::Imu> shoulder_sub, elbow_sub, wrist_sub;
  boost::shared_ptr<message_filters::Synchronizer<SyncArmFeedback>> sync;
  ros::NodeHandle nh;

  std_msgs::Float64MultiArray Jn;
  std_msgs::Float64 end_effector;
  std::vector<double> IMU_t,IMU_c,theta;
  //theta is unnecessary in this case but is essential for a generic mathemetical formulation
  double base_orientation, push_over;

  public:
    bool flag;
    Defs(ros::NodeHandle n_, double bo):nh(n_),base_orientation(bo)
    {
      push_over = 1*M_PI/180;
      flag = false;
      joint_output_command = nh.advertise<std_msgs::Float64MultiArray>("/armbot/arm_controller/command", 100);
      pitch_debug = nh.advertise<std_msgs::Float64>("/armbot/arm_controller/end_effector", 100);
      joint_input_command.subscribe(nh, "/armbot/arm_controller/target", 100);
      shoulder_sub.subscribe(nh, "/armbot/shoulder_imu", 100);
      elbow_sub.subscribe(nh, "/armbot/elbow_imu", 100);
      wrist_sub.subscribe(nh, "/armbot/wrist_imu", 100);
      sync.reset(new message_filters::Synchronizer<SyncArmFeedback>(SyncArmFeedback(100), joint_input_command, shoulder_sub, elbow_sub, wrist_sub));
      sync->registerCallback(&Defs::linkFeedback,this);
      Jn.data.push_back(0);Jn.data.push_back(-1);Jn.data.push_back(2);Jn.data.push_back(-1);Jn.data.push_back(0);
      IMU_t.push_back(base_orientation);IMU_t.push_back(0);IMU_t.push_back(0);IMU_t.push_back(0);
      IMU_c.push_back(base_orientation);IMU_c.push_back(0);IMU_c.push_back(0);IMU_c.push_back(0);
      theta.push_back(0);theta.push_back(M_PI/2);theta.push_back(M_PI);theta.push_back(M_PI);
    }
    void linkFeedback(const sensor_msgs::JointState::ConstPtr& js, const sensor_msgs::Imu::ConstPtr& shld, const sensor_msgs::Imu::ConstPtr& elbw, const sensor_msgs::Imu::ConstPtr& wrst);
    double setOrientation(sensor_msgs::Imu imu);
};

double Defs::setOrientation(sensor_msgs::Imu imu)
 {
    tf::Quaternion q(imu.orientation.x, imu.orientation.y, imu.orientation.z, imu.orientation.w);
    tf::Matrix3x3 m(q);
    double r,p,y;
    m.getRPY(r, p, y);
    if(imu.linear_acceleration.z<0)
    {
      if(p>0)
      {
        p = M_PI - p;
      }
      else
      {
        p = -M_PI - p;
      }
    }
    //p = p*180/M_PI;
    return p;
 }

void Defs::linkFeedback(const sensor_msgs::JointState::ConstPtr& js, const sensor_msgs::Imu::ConstPtr& shld, const sensor_msgs::Imu::ConstPtr& elbw, const sensor_msgs::Imu::ConstPtr& wrst)
{
  //IMU state solver
  for(int i=1;i<4;i++)
  {
  	double alpha = IMU_t[i-1] - theta[i] + js->position[i];
    if(flag==false)
    {
      alpha = -theta[i] + js->position[i];
    }
  	if(abs(alpha) < M_PI)
  	{
  		IMU_t[i] = alpha;
  	}
  	else if(alpha > M_PI)
  	{
  		IMU_t[i] = -2*M_PI + alpha;
  	}
  	else if(alpha < -M_PI)
  	{
  		IMU_t[i] = 2*M_PI + alpha;
  	}
  }
  std::cout<<"shoulder imu target-> "<<IMU_t[1]*(180/M_PI)<<std::endl;
  std::cout<<"elbow imu target-> "<<IMU_t[2]*(180/M_PI)<<std::endl;
  std::cout<<"wrist imu target-> "<<IMU_t[3]*(180/M_PI)<<std::endl;

  //Joint state solver
  IMU_c[1] = setOrientation(*shld);
  IMU_c[2] = setOrientation(*elbw);
  IMU_c[3] = setOrientation(*wrst);//IMU feedback
  std::cout<<"shoulder imu current-> "<<IMU_c[1]*(180/M_PI)<<std::endl;
  std::cout<<"elbow imu current-> "<<IMU_c[2]*(180/M_PI)<<std::endl;
  std::cout<<"wrist imu current-> "<<IMU_c[3]*(180/M_PI)<<std::endl;
  end_effector.data = IMU_c[3];
  Jn.data[0] = js->position[0]; //pan
  Jn.data[4] = js->position[4]; //gripper
  
  if(flag==true)
  {
    for(int i=1;i<4;i++)
    {
      double del_phi = IMU_t[i] - IMU_c[i];
    if(abs(del_phi) < push_over)
    {
      del_phi/=3;
    }
    else if(del_phi > push_over)
    {
        del_phi = push_over/3;
    }
    else if(del_phi < -push_over)
    {
        del_phi = -push_over/3;
    }
      Jn.data[i]+=del_phi; //shoulder, elbow, wrist
    }
    
  }
  else
  {
    for(int i=1;i<4;i++)
    {
      Jn.data[i]=IMU_t[i]; //shoulder, elbow, wrist
      IMU_t[i] = 0;
    }
  }
  joint_output_command.publish(Jn);
  pitch_debug.publish(end_effector);
  std::cout<<"shoulder joint -> "<<Jn.data[1]*(180/M_PI)<<std::endl;
  std::cout<<"elbow joint -> "<<Jn.data[2]*(180/M_PI)<<std::endl;
  std::cout<<"wrist joint -> "<<Jn.data[3]*(180/M_PI)<<std::endl;
  std::cout<<"----------------------------------------------\n";
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "dynamic_end_effector_stabilizer");
  ros::NodeHandle n;
  Defs obj(n,0);
  if(argv[1][0]=='s')
    obj.flag=true;

  ros::spin();
  // ros::Rate loop_rate(20);
  // while (ros::ok())
  // {
  //   ros::spinOnce();
  //   loop_rate.sleep();
  // }
  return 0;
}