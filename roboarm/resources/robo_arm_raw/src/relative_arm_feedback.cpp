#include <SerialLib.h>
#include <pthread.h>
#include <chrono>
#include <math.h>

typedef chrono::high_resolution_clock Clock;
typedef chrono::system_clock::time_point TimePoint;

int count=0;
bool read_flag=false;

double getTimeDifference(TimePoint end, TimePoint start)
{
    return ((double)chrono::duration_cast<chrono::milliseconds>(end-start).count());
}

//all will be modified for multipe imus

void *imuFeedback(void* port)
{
	SerialLib fed((char*)port);
	cout<<"Initializing IMU...\n";
	sleep(10);
	cout<<"Ready to take targets...\n";
	
	int feed=0,prev=0;
	string imu;
	imu = fed.readData();
	cout<<imu<<endl;
	if(imu=="")
	{
		cout<<"IMU not accessable!!!\n";
		exit(0);
	}
	feed=atoi(imu.c_str());
	prev=atoi(imu.c_str());
	TimePoint chkpt = Clock::now();
	while(true)
	{	
		imu = fed.readData();
		feed=atoi(imu.c_str());
		double diff = getTimeDifference(Clock::now(), chkpt);
		if(feed!=prev)
		{
			if(diff<50)
			{
				if(read_flag==true)
				{
					count++;
					cout<<"ang: "<<prev<<" | t: "<<diff<<" | count: "<<count<<"\n";
				}
				prev=feed;
			}
			chkpt=Clock::now();
		}
	}
}

int main(int argc,char* argv[])
{
	SerialLib ser(argv[1]);
	pthread_t fbk;
	int rc;
	rc=pthread_create(&fbk,NULL,imuFeedback,(void *)argv[2]);
	if(rc)
	{
		cout<<"Error: creating input thread\n";
		exit(-1);
	}
	
	unsigned char msg[]={0,0,0,85,0};
	int prev=0,curr=0;
	
	sleep(2);
	cout<<"ARMED!!!\n";

	while(curr!=-1)
	{
		cin>>curr;
		count=0;
		read_flag=true;
		while(count<abs(curr-prev))
		{
			if(curr>prev)
			{
				msg[2]++;
				if(msg[2]>170)
					msg[2]=170;
			}
			else if(curr<prev)
			{
				msg[2]--;
				if(msg[2]<0)
					msg[2]=0;
			}
			ser.writeData(msg,5);
			for(int i=0;i<5;i++)
			{
				cout<<(int)msg[i]<<",";
			}
			cout<<endl;
			usleep(30000);
		}
		prev=curr;
		read_flag=false;
		cout<<"Reached targets...\n";
    }
}
