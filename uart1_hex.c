#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdint.h>
int main() {
	int fd, len;
	char text[255];
	uint8_t hex_response[8] = {0};


	struct termios options; /* Serial ports setting */

	fd = open("/dev/ttyUSB1", O_RDWR | O_NDELAY | O_NOCTTY); //serial0
	if (fd < 0) {
		perror("Error opening serial port");
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
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &options);

	/* Write to serial port 
	strcpy(text, "Hello from my RPi\n\r");
	len = strlen(text);
	len = write(fd, text, len);
	printf("Wrote %d bytes over UART\n", len);

	printf("You have 5s to send me some input data...\n");
	*/

	sleep(8);

	/* Read from serial port */
	memset(hex_response, 0, 255);
	len = read(fd, hex_response, 255);
	printf("Received %d bytes\n", len);
	printf(" %x ", *(hex_response + 0));
	printf(" %x ", *(hex_response + 1));
	printf(" %x ", *(hex_response + 2));
	printf(" %x ", *(hex_response + 3));
	printf(" %x ", *(hex_response + 4));
	printf(" %x ", *(hex_response + 5));
	printf(" %x ", *(hex_response + 6));
	printf(" %x ", *(hex_response + 7));









	close(fd);
	return 0;
}
