/* createPrintPDU.h */


#ifndef CREATEPRINTPDU_H
#define CREATEPRINTPDU_H

/*-----------> Includes <-----------*/

#include <stdint.h>

/*-----------> Constants <-----------*/

#define MAX_PAYLOAD_LEN 1400 // max length for any payload
#define MAX_PDU_LEN     1407 // max length for entire PDU

/*-----------> Function Prototypes <-----------*/

int createPDU(uint8_t *pduBuffer, uint32_t sequenceNumber, uint8_t flag,
	      uint8_t *payload, int payloadLen);

void printPDU(uint8_t *aPDU, int pduLength);

#endif
