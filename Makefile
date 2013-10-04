# @author	Jon Hourany
# @date		9/18/2013
# @file		Makefile
# 
# @class	EECE 555
# @term		Fall 2013

all: snw-client snw-server 

snw-client: dgram-talk-client.c
	gcc -Wall -o snw-client dgram-talk-client.c -L./ -lpacketErrorSendTo

snw-server: dgram-talk-server.c
	gcc -Wall -o snw-server dgram-talk-server.c -L./ -lpacketErrorSendTo


