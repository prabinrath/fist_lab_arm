#include <SerialLib.h>
#include <chrono>
#include <math.h>

//all will be modified for multipe imus
int curr=0,feed;
bool master=true;
void *input(void* args)
{
	while(curr!=-1)
	{
		cin>>curr;
	}
	master = false;
}

int main(int argc,char* argv[])
{
	SerialLib ser(argv[1]);
	SerialLib fed(argv[2]);
	cout<<"Initializing IMU...\n";
	sleep(10);
	
	string imu;
	imu = fed.readData();
	cout<<imu<<endl;
	if(imu=="")
	{
		cout<<"IMU not accessable!!!\n";
		exit(0);
	}
	
	pthread_t inp;
	int rc;
	rc=pthread_create(&inp,NULL,input,(void *)NULL);
	if(rc)
	{
		cout<<"Error: creating input thread\n";
		exit(-1);
	}
	
	unsigned char msg[]={0,0,0,85,0};
	sleep(2);
	cout<<"Ready to take targets...\n";
	cout<<"ARMED!!!\n";

	while(master)
	{
		imu = fed.readData();
		feed = atoi(imu.c_str());
		if(curr>feed)
		{
			msg[2]++;
			if(msg[2]>170)
				msg[2]=170;
		}
		else if(curr<feed)
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
}
