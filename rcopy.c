// Client side - UDP Code				    
// By Hugh Smith	4/1/2017		

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "gethostbyname.h"
#include "networks.h"
#include "safeUtil.h"
#include "createPrintPDU.h"
#include "cpe464.h"

// #define MAXBUF 80

void talkToServer(int socketNum, struct sockaddr_in6 * server);
int readFromStdin(char * buffer);
int checkArgs(int argc, char * argv[], float *errorRate);

int main (int argc, char *argv[])
{
    int socketNum = 0;				
    struct sockaddr_in6 server;	// Supports 4 and 6 but requires IPv6 struct
    int portNumber = 0;
    float errorRate = 0;
	
    portNumber = checkArgs(argc, argv, &errorRate);
	
    socketNum = setupUdpClientToServer(&server, argv[2], portNumber);

    sendErr_init(errorRate, DROP_ON, FLIP_ON, DEBUG_ON, RSEED_OFF);
	
    talkToServer(socketNum, &server);
	
    close(socketNum);

    return 0;
}

void talkToServer(int socketNum, struct sockaddr_in6 * server)
{
    int serverAddrLen = sizeof(struct sockaddr_in6);
    // char * ipString = NULL;
    int dataLen = 0; 
    char buffer[MAX_PDU_LEN];

    uint8_t pduBuffer[MAX_PDU_LEN]; // initializing buffer to build PDUs
    int pduLen = 0; // length of built PDU
    uint8_t recvBuffer[MAX_PDU_LEN]; // initializing buffer used for incoming PDUs
    int recvLen = 0; // length of received PDU
    uint32_t sequenceNumCount = 0; // used for sequence number counting for PDUs
    uint8_t flag = 11; // random value from 1-255
	
    buffer[0] = '\0';
    while (buffer[0] != '.')
    {
	dataLen = readFromStdin(buffer);
	pduLen = createPDU(pduBuffer, sequenceNumCount++, flag, (uint8_t *)buffer, dataLen);  
 
	printPDU(pduBuffer, pduLen);
	
	safeSendto(socketNum, pduBuffer, pduLen, 0, (struct sockaddr *) server, serverAddrLen);
		
	recvLen = safeRecvfrom(socketNum, recvBuffer, MAX_PDU_LEN, 0, (struct sockaddr *) server, &serverAddrLen);
		
	// print out bytes received
	// ipString = ipAddressToString(server);
	printPDU(recvBuffer, recvLen);
    }
}

int readFromStdin(char * buffer)
{
    char aChar = 0;
    int inputLen = 0;        
	
    // Important you don't input more characters than you have space 
    buffer[0] = '\0';
    printf("Enter data: ");
    while (inputLen < (MAX_PDU_LEN - 1) && aChar != '\n')
    {
	aChar = getchar();
	if (aChar != '\n')
	{
	    buffer[inputLen] = aChar;
	    inputLen++;
	}
    }
	
    // Null terminate the string
    buffer[inputLen] = '\0';
    inputLen++;
	
    return inputLen;
}

int checkArgs(int argc, char * argv[], float *errorRate)
{

    int portNumber = 0;
	
    /* check command line arguments  */
	
    if (argc != 4)
    {
	printf("usage: %s error-rate server-host server-port \n", argv[0]);
	exit(1);
    }

    *errorRate = atof(argv[1]);	
    portNumber = atoi(argv[3]);
		
    return portNumber;
}
