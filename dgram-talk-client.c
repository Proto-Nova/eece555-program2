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
#include <sys/select.h>
#include <sys/time.h>
#include "packetErrorSendTo.h"

#define SERVER_PORT "5432"
#define MAX_SIZE     1300
#define ACK	    '0'
#define FILE_NAME   '1'
#define BODY	    '2'
#define HEADER_SIZE  2
#define PACKET_TYPE buf[0]
#define PACKET_NUM  buf[1]
#define PAYLOAD	    (char *) &buf + HEADER_SIZE

//void selectSetup(struct timeval *tv, int seconds, int u_seconds, fd_set *rfds);

int main(int argc, char *argv[])
{
	struct addrinfo 	hints;
	struct addrinfo 	*rp, *result;
	struct sockaddr_in 	server;
	struct timeval		tv;
	struct sockaddr 	*src_addr;
	socklen_t 		src_len;
	char   *host, *file_name;
	char	buf[MAX_SIZE];
	char	ack_buffer[HEADER_SIZE];
	char	last_packet_num;
	int  	s;
	int	file_name_recv	= 0;
	int	quit		= 0;
	char	curr_packet_num;
	fd_set 	rfds;
	int	file_desc;
	int	len;
	int	retval;
	int	file_name_ack	= 0;

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

	strcpy(PAYLOAD, file_name);
	
	while(!file_name_ack)
	{
		/* Set select timeout settings */
		//selectSetup(&tv, 3, 30000, &rfds);
		tv.tv_sec	= 3;
		tv.tv_usec 	= 300000;
		FD_ZERO(&rfds);
		FD_SET (s, &rfds);

		/* Set Header Values */
		PACKET_TYPE 	= FILE_NAME;
		PACKET_NUM 	= '0';
		
		printf("BUFFER:%s\n", &buf);
		printf("FILENAME:%s\n", file_name);

		packetErrorSendTo(s, &buf, strlen(file_name)+3, 0, (struct sockaddr *)src_addr, src_len);
		retval = select(s+1, &rfds, NULL,NULL, &tv);

		if (retval == -1)		// Error State
		{
		    	perror("CRASH...\n");
		    	close(s);
		    	exit(1);
		    
		}
		else if(retval == 0)		// Timeout State
		{
			printf("RETVAL = 0\n");
		}
		else
		{
			if( recvfrom(s, (void *) &buf, sizeof(buf), 0, src_addr, &src_len) == -1 )
			{
				perror("dgram-talk-server: recvfrom");
				close(s);
				exit(1);
			}
			else if ( PACKET_TYPE == ACK )
			{
				printf("ACK recieved neggaa!\n");
				file_name_ack = 1;
			}	
		}
	}
	
	if ((file_desc = open((char *) file_name, O_CREAT|O_TRUNC|O_WRONLY, S_IRWXU)) == -1)
	{
		printf("File open fail\n");
		exit(1);
	}

	last_packet_num = "NULL"; 	// Sets up variable to unreachable status before first use
	
	/* Main loop: get and send lines of text */
	while (!quit)
	{
		FD_ZERO(&rfds);
                FD_SET (s, &rfds);
		tv.tv_sec  = 3;
		tv.tv_usec = 0;

		retval = select(s+1, &rfds, NULL,NULL, &tv);

		printf("MAIN LOOP~\n");
		if (retval == -1)
		{
		    	perror("CRASH...\n");
		    	close(s);
		    	exit(1);
		    
		}
		else if(retval == 0)
		{
		    	perror("dgram-talk-client: timeout\n");
		    	close(s);
			close(file_desc);
			exit(1);
		}
		else
		{
			int bytes_recv = recvfrom(s, (void *)&buf, sizeof(buf), 0, (struct sockaddr *)src_addr, &src_len);
			if( bytes_recv == -1 )
                        {
                                perror("dgram-talk-server: recvfrom");
                                printf("BUFFER: %s\n", &buf);
                                close(s);
                                exit(1);
                        }
			printf("PACKET NUMBER: %c\n", PACKET_NUM);
			ack_buffer[0]	= ACK; // An ACK packet
			ack_buffer[1]	= PACKET_NUM;

			packetErrorSendTo(s, &ack_buffer, sizeof(ack_buffer), 0, (struct sockaddr *)src_addr, src_len);
	
			/* If new packet and is of body type */
			if( PACKET_NUM != last_packet_num && PACKET_TYPE == BODY) 
			{
				write(file_desc, PAYLOAD, bytes_recv - HEADER_SIZE);
				last_packet_num = PACKET_NUM;
			}	
		}
	}

	close(s);

	return 0;
}

/*void selectSetup(struct timeval *tv, int seconds, int u_seconds, fd_set *rdfs)

	struct timeval *tv;
	*tv.tv_sec       = seconds;
        *tv.tv_usec      = u_seconds; 
	FD_ZERO(rfds);
	FD_SET (s, rfds);
}*/
