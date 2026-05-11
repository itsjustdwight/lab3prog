/* Server side - UDP Code */
/* By Hugh Smith	4/1/2017	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "gethostbyname.h"
#include "networks.h"
#include "safeUtil.h"
#include "createPrintPDU.h"
#include "cpe464.h"


void processClient(int socketNum);
int checkArgs(int argc, char *argv[], float *errorRate);

int main ( int argc, char *argv[]  )
{ 
    int socketNum = 0;				
    int portNumber = 0;
    float errorRate = 0;

    portNumber = checkArgs(argc, argv, &errorRate);
		
    socketNum = udpServerSetup(portNumber);

    processClient(socketNum);

    sendErr_init(errorRate, DROP_ON, FLIP_ON, DEBUG_ON, RSEED_ON);

    close(socketNum);
	
    return 0;
}

void processClient(int socketNum)
{
    int dataLen = 0; // length of received PDU
    uint8_t pduBuffer[MAX_PDU_LEN]; // buffer changed to be what's received and echoed back out	 
    struct sockaddr_in6 client;
    int clientAddrLen = sizeof(client);	
	
    while (1)
    {
	dataLen = safeRecvfrom(socketNum, pduBuffer, MAX_PDU_LEN, 0, (struct sockaddr *) &client, &clientAddrLen);
	
	printPDU(pduBuffer, dataLen);
	
	safeSendto(socketNum, pduBuffer, dataLen, 0, (struct sockaddr *) & client, clientAddrLen);

    }
}

int checkArgs(int argc, char *argv[], float *errorRate)
{
    // Checks args and returns port number
    int portNumber = 0;
	
    if (argc < 2 || argc > 3)
    {
	fprintf(stderr, "Usage %s error-rate [optional-port]\n", argv[0]);
	exit(-1);
    }

    *errorRate = atof(argv[1]);
	
    if (argc == 3)
    {
	portNumber = atoi(argv[2]);
    }
	
    return portNumber;
}


