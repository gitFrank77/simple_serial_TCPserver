
/*********************************
Author: Frank A.
Date created: 2/10/24
Date modified: 2/11/24
Description:


TODO:
6.  hex#_response data being set as cryptic to server. Sending strings works so try sending hex and dev this with server5.c, implement findings here
2.  When a port is open create a thread to send data to aggregate stucture
3.  Send aggregate structure to TCP port [server] workflow
5.  Cleanup work with printing results ( make a for loop as opposed to brute printing)
Complete:

1. Poll fd1 and fd2 open
4. Created a single muxtex for critical section ( should try multithreaded approach)

Note:
-> Careful not to fork in a loop , can lead to overflow and unwanted multitasking
*********************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdint.h>
#include <pthread.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "1502"  // the port user will be connection to 
#define BACKLOG 10	// how many pending connections queue will hold




//---[start of server functions]

unsigned long count = 0;

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}





//---[end of server functions]






//----[start serial data functions]

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
//----[end of serial functions]

//---[Start of global variables]

	uint8_t hex1_response[8] = {0};
	 uint8_t hex2_response[8] = {0};

	char device1_string[255];
//---[End of global variables]


int main() {
	int fd1,fd2, len;
	//char text[255];

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

//	sleep(8);
//--[start of Server setup]
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

         char *mystring = {"<A1,TC,31,TCPIP>\r\n"};
	 char *ptr;
	 char *mystring2 = {"<A2,TC,98,TCPIP>\r\n"};
	
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

//*********8

        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
         //   continue;
        }

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\r\n", s);



//---[End of server setup]



	for(;;)
	{

	//---[Start of server processing]


//        if (!fork()) { // this is the child process
  //          close(sockfd); // child doesn't need the listener
	

		 if (send(new_fd,ptr, 13, 0) == -1) // "1\r\n"
               	perror("");
		sleep(1);
		
		if( count == 1)
		{
		//	ptr = mystring;
	//		ptr = (char *)hex1_response;		
			ptr = device1_string;
			count = 0;
		sleep(1);

		}	
   		 else if(count ==0)
		 {
		//	ptr = mystring2;
			ptr = (char *)hex2_response ;		
				count =1;
				sleep(1);
		 }
//	}

	//---[End of Server processing]




	//---[Start of serial processing]
	printf("\n Startresponse \n\r");
//
	sleep(1);	
//	sleep(5);
	/* Read from serial port */
	
	fd1 = open("/dev/ttyUSB0", O_RDWR | O_NDELAY | O_NOCTTY); //serial0
	sleep(1);
//dd	memset(hex1_response, 0, 64);
	memset(device1_string,0,255);
//	len = read(fd1, hex1_response, 64);
	len = read(fd1, device1_string, 255);
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
	sleep(5);
	}
	
	printf("\n Now for the next response \n\r");
	
	//sleep(5);	

	fd2 = open("/dev/ttyUSB1", O_RDWR | O_NDELAY | O_NOCTTY); //serial0
	sleep(1);
	
	

	memset(hex2_response, 0, 64);
	len = read(fd2, hex2_response, 64);
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
	sleep(5);
	}
	peek_agg_data_results(data_for_server);// print device data buffers results
//	cleanup_agg_data(data_for_server); //clear device data buffer and reset count

	data_for_server = (agg_data_t *)calloc(1, sizeof(agg_data_t));
	//memset(hex1_response,0,8*sizeof(uint8_t));
	//memset(hex2_response,0,8*sizeof(uint8_t));
	sleep(1);
	//---[End of serial processing]
	
	fflush(stdout);
	}
	close(fd1);
	close(new_fd);
	close(fd2);
	return 0;
}
