#include <Arduino.h>

// Used to decode reply packets from the radio.
uint8_t decodeHeader(byte *header);
void decodePacket(byte *payload, byte opcode);
void ackHelper(byte* header);
