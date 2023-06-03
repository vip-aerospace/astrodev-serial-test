#include <Arduino.h>

extern void initializeHeader(byte *header);
extern void setHeader(byte *header, byte command, size_t payloadsize);


void calcCS(uint8_t *data, uint16_t size, uint8_t *ckloc);
uint8_t reverseBits(uint8_t b);
void printHexDump(const uint8_t* data, size_t size);
