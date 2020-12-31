#include <SerialLib.h>

int main(int argc,char* argv[])
{
	SerialLib ser(argv[1]);
	unsigned char msg[]={0,0,0,0,0};
	int prev[]={0,0,0,85,0};
	int curr[]={0,0,0,0,0};
	
	sleep(2);
	cout<<"Ready...\n";
	string str;
	while(str!="exit")
	{
		cin>>str;
		string temp="";
		int index =0;
		for(int i=0; i < str.length(); i++)
		{
		  if(str[i]==',')
		  {
		    curr[index]=atoi(temp.c_str());
		    index++;
		    temp="";        
		  }
		  else
		  {
		    temp+=str[i];
		  }
		}
		curr[index]=atoi(temp.c_str());
		if(index!=4)
		{
			cout<<"Invalid input\n";
			continue;
		}
		if(curr[index]>51)
			curr[index]=51;	//gripper angle threshold
		
		/*for(int i=0;i<5;i++)
			cout<<curr[i]<<" ";
		exit(0);*/
		
		bool flag[5]={1,1,1,1,1};
		while(flag[0] || flag[1] || flag[2] || flag[3] || flag[4])
		{
			for(int i=0; i<5; i++)
			{
				if(curr[i] == prev[i])
				{
				    msg[i]=curr[i];
				    flag[i]=false;
				}
				else if(curr[i]>prev[i])
				{
					prev[i]++;
				    msg[i]=prev[i];
				}
				else if(curr[i]<prev[i])
				{
					prev[i]--;
					msg[i]=prev[i];
				}
			}
			/*for(int i=0;i<5;i++)
				cout<<(int)msg[i]<<" ";
			cout<<endl;*/
			ser.writeData(msg,5);
			usleep(30000);
		}
		cout<<"Reached targets...\n";
    }
}
