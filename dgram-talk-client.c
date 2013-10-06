/* This code is an updated version of the sample code from "Computer Networks: A Systems
 * Approach," 5th Edition by Larry L. Peterson and Bruce S. Davis modified to use a DGRAM
 * socket instead of a STREAM socket. Some code comes from man pages, mostly getaddrinfo(3).
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "packetErrorSendTo.h"


#define SERVER_PORT "5432"
#define MAX_SIZE     1300
#define ACK	    '0'
#define FILE_NAME   '1'
#define BODY	    '2'
#define PACKET_TYPE buf[0]
#define PACKET_NUM  buf[1]
#define PAYLOAD	    (char *) &buf + 2

int
main(int argc, char *argv[])
{
	struct addrinfo 	hints;
	struct addrinfo 	*rp, *result;
	struct sockaddr_in 	server;
	struct timeval		tv;
	struct sockaddr 	*src_addr;
	socklen_t 		src_len;
	char   *host, *file_name;
	char	buf[MAX_SIZE];
	char	ack_buffer[2];
	char	last_packet_num;
	int  	s;
	int	file_name_recv	= 0;
	int	quit		= 0;
	char	curr_packet_num;
	fd_set 	rfds;
	int	file_desc;
	int	len;
	int	retval;
	int	file_name_ack;

	if (argc==3)
	{
		host      = argv[1];
		file_name = argv[2];
	}
	else
	{
		fprintf(stderr, "usage: %s host\n", argv[0]);
		exit(1);
	}

	/* Translate host name into peer's IP address */
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;
	if ((s = getaddrinfo(host, SERVER_PORT, &hints, &result)) != 0 )
	{
		fprintf(stderr, "%s: getaddrinfo: %s\n", argv[0], gai_strerror(s));
		exit(1);
	}

	/* Iterate through the address list and create socket */
	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		if ((s = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1 )
		{
			continue;
		}
		else{
			memcpy(&server, rp->ai_addr, sizeof(server));
			src_addr = (struct sockaddr *) rp->ai_addr;
			src_len  = (socklen_t) rp->ai_addrlen;
			
			break;
		}
		close(s);
	}
	if (rp == NULL)
	{
		perror("dgram-talk-client: connect");
		exit(1);
	}
	freeaddrinfo(result);

	FD_ZERO(&rfds);
	FD_SET (s, &rfds);
	strcpy(PAYLOAD, file_name);
	
	while(!file_name_ack)
	{
<<<<<<< HEAD
		/* Set select timeout settings */
		tv.tv_sec	= 2;
		tv.tv_usec 	= 300000;

		/* Set Header Values */
=======
		tv.tv_sec	= 2;
		tv.tv_usec 	= 300000;
>>>>>>> a3c2d23f5b0ea643c6f170a1a8bae02ef56cef3c
		PACKET_TYPE 	= FILE_NAME;
		PACKET_NUM 	= 'O';
		
		printf("BUFFER:%s\n", &buf);
<<<<<<< HEAD
		printf("FILENAME:%s\n", file_name);
		packetErrorSendTo(s, &buf, strlen(file_name)+2, 0, (struct sockaddr *)src_addr, src_len);

		retval = select(s+1, &rfds, NULL,NULL, &tv);

		if (retval == -1)		// Error State
=======
		packetErrorSendTo(s, &buf, strlen(file_name)+2, 0, (struct sockaddr *)src_addr, src_len);

		retval 		= select(s+1, &rfds, NULL,NULL, &tv);

		if (retval == -1)
>>>>>>> a3c2d23f5b0ea643c6f170a1a8bae02ef56cef3c
		{
		    	perror("CRASH...\n");
		    	close(s);
		    	exit(1);
		    
		}
<<<<<<< HEAD
		else if(retval == 0)		// Timeout State
		{
			printf("RETVAL = 0\n");
		}
		else				// Success State
=======
		else if(retval == 0)
		{
			printf("RETVAL = 0\n");
		}
		else
>>>>>>> a3c2d23f5b0ea643c6f170a1a8bae02ef56cef3c
		{
			if( recvfrom(s, &buf, sizeof(buf), 0, src_addr, src_len) == -1 )
			{
				perror("dgram-talk-server: recvfrom");
				close(s);
				exit(1);
			}
			else if ( PACKET_TYPE == ACK )
			{
				printf("ACK recieved neggaa!\n");
			}	
			printf("BUFF RECV: %s\n", &buf);
		}
	}
	
	if ((file_desc = open((char *) file_name, O_CREAT|O_TRUNC, S_IRWXU)) == -1)
	{
		printf("File open fail\n");
		exit(1);
	}

	FD_ZERO(&rfds);
	FD_SET (s, &rfds);
	
	/* Main loop: get and send lines of text */
/*	while (!quit)
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
		else if(retval == 0)
		{
		    	file_name_recv = 1;
		}
		else
		{
			ack_buffer[0] = ACK; // An ACK packet
			ack_buffer[1] = PACKET_NUM;

			packetErrorSendTo(s, &ack_buffer, sizeof(ack_buffer), 0, (struct sockaddr *)src_addr, src_len);
	
			/* If new packet and is of file_name type *//*
			if( PACKET_NUM != last_packet_number && PACKET_TYPE == FILE_NAME ) 
			{
				file_name = (char *) buf;
				file_name = file_name + 2;
				last_packet_number = PACKET_NUM;
				
				//*******************************************************************
				//
				// This shit needs some fixin' yo!
				// I hate programming. 
				//
				//*******************************************************************
				write(file_desc, AND SHIT)
				
				file_name_recv = 1;
			}	
		}
	}*/

	close(s);

	return 0;
}
