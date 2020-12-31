#include <SerialLib.h>

int main(int argc,char* argv[])
{
	SerialLib ser(argv[1]);
	sleep(5);
	
	while(1)
	{	
		string imu = ser.readData();
		cout<<imu<<endl;
	}
}
