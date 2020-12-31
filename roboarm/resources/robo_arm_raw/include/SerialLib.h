// C library headers
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>
// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
#include <stdlib.h>

using namespace std;

class SerialLib
{
	private:
		int serial_port;
		struct termios tty;
	public:
		SerialLib(char* port);
		void writeData(unsigned char msg[],int size);
		void writeData(string msg);
		string readData();
		~SerialLib();
};
