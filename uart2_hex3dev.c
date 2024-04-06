#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdint.h>
#include <pthread.h>

int main() {
	int fd1,fd2, len;
	char text[255];
	uint8_t hex1_response[8] = {0};
	uint8_t hex2_response[8] = {0};



	struct termios options; /* Serial ports setting */

	fd1 = open("/dev/ttyUSB0", O_RDWR | O_NDELAY | O_NOCTTY); //serial0
	if (fd1 < 0) {
		perror("Error opening serial port #0");
		return -1;
	}
	fd2 = open("/dev/ttyUSB1", O_RDWR | O_NDELAY | O_NOCTTY); //serial0
	if (fd2 < 0) {
		perror("Error opening serial port #1");
		return -1;
	}


	/* Read current serial port settings */
	// tcgetattr(fd, &options);
	
	/* Set up serial port */
	options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;

	/* Apply the settings */
	tcflush(fd1, TCIFLUSH);
	tcsetattr(fd1, TCSANOW, &options);
	/* Apply the settings */
	tcflush(fd2, TCIFLUSH);
	tcsetattr(fd2, TCSANOW, &options);


	/* Write to serial port 
	strcpy(text, "Hello from my RPi\n\r");
	len = strlen(text);
	len = write(fd, text, len);
	printf("Wrote %d bytes over UART\n", len);

	printf("You have 5s to send me some input data...\n");
	*/

	sleep(8);

	for(;;)
	{

	printf("\n Now for the next response \n\r");
	
	sleep(5);
	/* Read from serial port */
	fd1 = open("/dev/ttyUSB0", O_RDWR | O_NDELAY | O_NOCTTY); //serial0
	sleep(1);
	memset(hex1_response, 0, 255);
	len = read(fd1, hex1_response, 255);
	printf("Received %d bytes\n", len);
	printf(" %x ", *(hex1_response + 0));
	printf(" %x ", *(hex1_response + 1));
	printf(" %x ", *(hex1_response + 2));
	printf(" %x ", *(hex1_response + 3));
	printf(" %x ", *(hex1_response + 4));
	printf(" %x ", *(hex1_response + 5));
	printf(" %x ", *(hex1_response + 6));
	printf(" %x ", *(hex1_response + 7));

	printf("\n Now for the next response \n\r");
	
	sleep(5);	

	fd2 = open("/dev/ttyUSB1", O_RDWR | O_NDELAY | O_NOCTTY); //serial0
	sleep(1);
	
	memset(hex2_response, 0, 255);
	len = read(fd2, hex2_response, 255);
	printf("Received %d bytes\n", len);
	printf(" %x ", *(hex2_response + 0));
	printf(" %x ", *(hex2_response + 1));
	printf(" %x ", *(hex2_response + 2));
	printf(" %x ", *(hex2_response + 3));
	printf(" %x ", *(hex2_response + 4));
	printf(" %x ", *(hex2_response + 5));
	printf(" %x ", *(hex2_response + 6));
	printf(" %x ", *(hex2_response + 7));
	
	sleep(5);

	}

	close(fd1);
	close(fd2);
	return 0;
}
