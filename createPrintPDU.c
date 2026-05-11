/* createPrintPDU.c */


/*-----------> Includes <-----------*/

#include <stdint.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>

#include "checksum.h"
#include "createPrintPDU.h"

/*-----------> createPDU Function <-----------*/
int createPDU(uint8_t *pduBuffer, uint32_t sequenceNumber, uint8_t flag,
	      uint8_t *payload, int payloadLen) {
    if (pduBuffer == NULL) { // checking buffer validity
	return -1; // invalid buffer given (no buffer)
    }
    if (payload == NULL) { // checking data validity
	return -1; // invalid data given (no data)
    }
    if (payloadLen > MAX_PAYLOAD_LEN) {
	return -1; // payload more than 1400 bytes
    }

    memset(pduBuffer, 0, MAX_PDU_LEN); // zeroing out full PDU to ensure odd byte is zeroed out for in_cksum()

    unsigned short checksumValue = 0; // checksum to be calculated
    uint32_t pduLength = sizeof(sequenceNumber) + sizeof(checksumValue) + sizeof(flag) + payloadLen; // 1407 bytes max
    uint32_t sequenceNumberNet = htonl(sequenceNumber); // converting to network order

    uint32_t offset = 0; // offset for copying data into right spots of PDU

    memcpy(pduBuffer, &sequenceNumberNet, sizeof(sequenceNumberNet)); // copying sequence # into PDU at pduBuffer[0]
    offset += sizeof(sequenceNumberNet); // incrementing offset to index farther in PDU
    memcpy(pduBuffer + offset, &checksumValue, sizeof(checksumValue)); // initializing checksum to 0
    offset += sizeof(checksumValue); // incrementing offset
    memcpy(pduBuffer + offset, &flag, sizeof(flag)); // copying flag into PDU at pduBuffer[6]
    offset += sizeof(flag); // incrementing offset
    memcpy(pduBuffer + offset, payload, payloadLen); // copying full payload into PDU at pduBuffer[7]
    offset += payloadLen; // incrementing offset

    checksumValue = in_cksum((unsigned short *)pduBuffer, pduLength); // calculating the checksum for entire PDU
    memcpy(pduBuffer + 4, &checksumValue, sizeof(checksumValue)); // copying checksum into PDU at pduBuffer[4]

    return pduLength; // returning length of PDU
}

/*-----------> printPDU Function <-----------*/
void printPDU(uint8_t *aPDU, int pduLength) {
    uint16_t checksumValue; // initializing checksum to be extracted and put into aPDU
    uint32_t sequenceNumberNet; // intializing sequence # (network order) to be extracted and put into aPDU
    uint8_t flag; // initializing flag to be extracted and put into aPDU

    checksumValue = in_cksum((unsigned short *)aPDU, pduLength); // calculating checksum for error checking

    if (checksumValue != 0) {
        printf("Error: checksum failed, data corrupted\n"); // happens when checksum fails (!= 0)
	return;
    }

    memcpy(&sequenceNumberNet, aPDU, 4); // copy sequence # into PDU to be printed
    memcpy(&flag, aPDU + 6, 1); // copy flag into PDU to be printed

    uint8_t *payload = aPDU + 7; // initializing payload to start at 7th byte of PDU
    int payloadLen = pduLength - 7; // initializing payload length
    uint32_t sequenceNumber = ntohl(sequenceNumberNet); // converting sequence # to host order for pritnting

    printf("Sequence #: %d\n",sequenceNumber); // because checksum = 0, print sequence #
    printf("Flag: %d\n", flag); // because checksum = 0, print flag
    printf("Payload: %.*s\n", payloadLen, payload); // because checksum = 0, print full payload
    printf("Payload Length: %d\n", payloadLen); // because checksum = 0, print payload length
}
