/* This code is an updated version of the sample code from "Computer Networks: A Systems
 * Approach," 5th Edition by Larry L. Peterson and Bruce S. Davis modified to use a DGRAM
 * socket instead of a STREAM socket. Some code comes from man pages, mostly getaddrinfo(3). */
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define SERVER_PORT "5432"
#define MAX_SIZE    1300
#define MAX_PENDING 5
#define	ACK	    '0'
#define FILE_NAME   '1'
#define BODY	    '2'
#define PACKET_TYPE buf[0]
#define PACKET_NUM  buf[1]
int
main(int argc, char *argv[])
{
	struct addrinfo         hints;
        struct addrinfo         *rp, *result;
        struct sockaddr_in      server;
        struct timeval          tv;
        struct sockaddr		*src_addr;
        socklen_t               src_len;
        char   *host, *file_name;
        char    buf[MAX_SIZE];
        char    ack_buffer[2];
        int     s;
        fd_set  rfds;
        int     file_desc;
        int     len;
        int     retval;
        char 	last_packet_num = '0';
        int     file_name_ack	= 0;
	int     file_name_recv  = 0;
        int     quit            = 0;
	char    curr_packet_num = 0;

	/* Build address data structure */
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family 	= AF_INET;
	hints.ai_socktype 	= SOCK_DGRAM;
	hints.ai_flags 		= AI_PASSIVE;
	hints.ai_protocol 	= 0;
	hints.ai_canonname	= NULL;
	hints.ai_addr 		= NULL;
	hints.ai_next 		= NULL;
	
	//[1][P][T][E][X][T][.][T][X][T]
	//[0][#]
	/* Get local address info */
	if ((s = getaddrinfo(NULL, SERVER_PORT, &hints, &result)) != 0 )
	{
		fprintf(stderr, "%s: getaddrinfo: %s\n", argv[0], gai_strerror(s));
		exit(1);
	}

	/* Iterate through the address list and bind to an address */
	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		if ((s = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1 )
		{
			continue;
		}

		if (!bind(s, rp->ai_addr, rp->ai_addrlen))
		{
			
			break;
		}

		close(s);
	}
	if (rp == NULL)
	{
		perror("dgram-talk-server: bind");
		exit(1);
	}
	src_addr = (struct sockaddr*) rp->ai_addr;
	src_len  = sizeof src_addr;
	freeaddrinfo(result);
	
	FD_ZERO(&rfds);
	FD_SET (s, &rfds);
	
	while (!file_name_recv)
	{
		tv.tv_sec  = 5;
		tv.tv_usec = 0;

		retval = select(s+1, &rfds, NULL,NULL, &tv);

		if (retval == -1)
		{
		    	perror("CRASH...\n");
		    	close(s);
		    	exit(1);
		    
		}
		/* If select times out, quit */
		else if(retval == 0)
		{
		    	perror("dgram-talk-server: select timeout");
			close(s);
			exit(1);
			//file_name_recv = 1;
		}
		else
		{
			if( recvfrom(s, (void *)&buf, sizeof(buf), 0, (struct sockaddr *)src_addr, (socklen_t *) &src_len) == -1 )
			{
				perror("dgram-talk-server: recvfrom");
				printf("BUFFER: %s\n", &buf);
				close(s);
				exit(1);
			}
			
			ack_buffer[0] = ACK; // An ACK packet
			ack_buffer[1] = PACKET_NUM;

			int tmp = packetErrorSendTo(s, (void *) &ack_buffer, 2, 0, (struct sockaddr *) src_addr, (socklen_t *) &src_len);
			printf("Error Sendto Val: %d\n", tmp);
				
			/* If new packet and is of file_name type */
			if( PACKET_NUM != last_packet_num && PACKET_TYPE == FILE_NAME ) 
			{
				file_name = (char *) buf;
				file_name = file_name + 2;
				printf("Recieved Filename: %s\n", file_name);
				last_packet_num = PACKET_NUM;
				file_desc = open(file_name, O_RDONLY);
				file_name_recv = 1;
			}	
		}
	}	
				
	/* Receive and print text */
	while (!quit)
	{
		tv.tv_sec  = 0;
		tv.tv_usec = 300;

		retval = select(s+1, &rfds, NULL,NULL, &tv);

		if (retval == -1)
		{
		    	perror("CRASH...\n");
		    	close(s);
		    	exit(1);
		    
		}
		else if(retval == 0)
		{
		    	quit = 1;
		}
		else
		{
			if( recvfrom(s, &buf, sizeof(buf), 0, src_addr, src_len) == -1 ){
				perror("dgram-talk-server: recvfrom");
				close(s);
				exit(1);
			}
		
		}
	}

	close(s);

	return 0;
}
