
/*********************************
Author: Frank A.
Date created: 2/10/24
Date modified: 2/11/24
Description:


TODO:
2.  When a port is open create a thread to send data to aggregate stucture
3.  Send aggregate structure to TCP port [server] workflow
5.  Cleanup work with printing results ( make a for loop as opposed to brute printing)
Complete:

1. Poll fd1 and fd2 open
4. Created a single muxtex for critical section ( should try multithreaded approach)

*********************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdint.h>
#include <pthread.h>

typedef struct _data
{
	uint8_t device1_data[8];
	uint8_t device2_data[8];

	unsigned int writes;
}agg_data_t;

void cleanup_agg_data( agg_data_t * me)
{
	for( int  i = 0 ; i < 8; i++)
	{	
		me->device1_data[i] = 0;
		me->device2_data[i] = 0;  
	}
	me->writes = 0;
}

void cleanup_agg_data_results( agg_data_t *me)
{
	printf("\n Aggregate data cleanup ");

	printf("\n Device 1 data \n\r ");

	for( int  i = 0 ; i < 8 ; i++)
	{
		printf("%X", me->device1_data[i]);
	}

	printf("\n Device 2 data \n\r");


	for( int  i = 0 ; i < 8 ; i++)
	{
		printf("%X", me->device2_data[i]);
	}


	printf("\n  [writes]= %d \n\r", me->writes);
}


void peek_agg_data_results( agg_data_t *me)
{
	printf("\n Aggregate data [RESULTS] ");

	printf("\n Device 1 data \n\r ");

	for( int  i = 0 ; i < 8 ; i++)
	{
		printf("%X", me->device1_data[i]);
	}

	printf("\nDevice 2 data \n\r");


	for( int  i = 0 ; i < 8 ; i++)
	{
		printf("%X", me->device2_data[i]);
	}


	printf("\n  [writes]= %d \n\r", me->writes);
}


	pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;


int main() {
	int fd1,fd2, len;
	char text[255];
	uint8_t hex1_response[8] = {0};
	uint8_t hex2_response[8] = {0};

	agg_data_t *data_for_server = (agg_data_t *)malloc(sizeof(agg_data_t)); //aggregate data
	cleanup_agg_data(data_for_server);
	cleanup_agg_data_results(data_for_server);

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

	printf("\n Startresponse \n\r");
	
//	sleep(5);
	/* Read from serial port */
	fd1 = open("/dev/ttyUSB0", O_RDWR | O_NDELAY | O_NOCTTY); //serial0
	sleep(1);
	memset(hex1_response, 0, 255);
	len = read(fd1, hex1_response, 255);
	if( *(hex1_response + 0 ) >= 1)
	{
	pthread_mutex_lock(&mutex1);

	printf("Received %d bytes\n", len);
	printf(" %x ", *(hex1_response + 0));
	printf(" %x ", *(hex1_response + 1));
	printf(" %x ", *(hex1_response + 2));
	printf(" %x ", *(hex1_response + 3));
	printf(" %x ", *(hex1_response + 4));
	printf(" %x ", *(hex1_response + 5));
	printf(" %x ", *(hex1_response + 6));
	printf(" %x ", *(hex1_response + 7));

	//add data to server for device 1
	data_for_server->device1_data[0] = *(hex1_response + 0);
	data_for_server->device1_data[1] = *(hex1_response + 1);
	data_for_server->device1_data[2] = *(hex1_response + 2);
	data_for_server->device1_data[3] = *(hex1_response + 3);
	data_for_server->device1_data[4] = *(hex1_response + 4);
	data_for_server->device1_data[5] = *(hex1_response + 5);
	data_for_server->device1_data[6] = *(hex1_response + 6);
	data_for_server->device1_data[7] = *(hex1_response + 7);
	
	data_for_server->writes++;
	
	pthread_mutex_unlock(&mutex1);
	}
	printf("\n Now for the next response \n\r");
	
	//sleep(5);	

	fd2 = open("/dev/ttyUSB1", O_RDWR | O_NDELAY | O_NOCTTY); //serial0
	sleep(1);
	
	

	memset(hex2_response, 0, 255);
	len = read(fd2, hex2_response, 255);
	if( *(hex2_response + 0 ) >= 1)
	{
	pthread_mutex_lock(&mutex1);
	printf("Received %d bytes\n", len);
	printf(" %x ", *(hex2_response + 0));
	printf(" %x ", *(hex2_response + 1));
	printf(" %x ", *(hex2_response + 2));
	printf(" %x ", *(hex2_response + 3));
	printf(" %x ", *(hex2_response + 4));
	printf(" %x ", *(hex2_response + 5));
	printf(" %x ", *(hex2_response + 6));
	printf(" %x ", *(hex2_response + 7));
	
	
	data_for_server->device2_data[0] = *(hex2_response + 0);// add data to server to device 2
	data_for_server->device2_data[1] = *(hex2_response + 1);// add data to server to device 2
	data_for_server->device2_data[2] = *(hex2_response + 2);// add data to server to device 2
	data_for_server->device2_data[3] = *(hex2_response + 3);// add data to server to device 2
	data_for_server->device2_data[4] = *(hex2_response + 4);// add data to server to device 2
	data_for_server->device2_data[5] = *(hex2_response + 5);// add data to server to device 2
	data_for_server->device2_data[6] = *(hex2_response + 6);// add data to server to device 2
	data_for_server->device2_data[7] = *(hex2_response + 7);// add data to server to device 2
	
	data_for_server->writes++;
	pthread_mutex_unlock(&mutex1);
	}
	peek_agg_data_results(data_for_server);// print device data buffers results
	cleanup_agg_data(data_for_server); //clear device data buffer and reset count
	sleep(1);
	


	}

	close(fd1);
	close(fd2);
	return 0;
}
