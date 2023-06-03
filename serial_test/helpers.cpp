#include "helpers.h"

// Some fields in the header are the same for all packets.
// This function fills in those fields with values that will not change over the program's execution.
void initializeHeader(byte *header){
  // The first sync byte.
  header[0] = (byte)'H';
  // The second sync byte.
  header[1] = (byte)'e';
  // The packet's direction of travel.
  // 0x10 = Packet from Teensy to radio.
  header[2] = 0x10;
}

// This fills in the rest of the fields of the header with the appropriate data.
void setHeader(byte *header, byte command, size_t payloadsize){
  // The packet command.
  header[3] = command;
  // The high byte of the packet size.
  header[4] = 0x00;
  // The low byte of the packet size.
  header[5] = reverseBits(payloadsize);
  // Calculate the two-byte checksum.
  calcCS(&header[2], 4, &header[6]);
}

// Calculate a two-byte checksum and write it to the designated location.
void calcCS(uint8_t *data, uint16_t size, uint8_t *ckloc) {
  /**
   * The checksum consists of two bytes, \c ck_a and \c 
   * ck_b. Combined, they form the two-byte checksum \c cs.
   */
  uint8_t ck_a=0, ck_b=0;
  
  /**
   * The checksum algorithm is relatively simple. For
   * each byte in the data, it is added to \c ck_a. \c
   * ck_a is then added to \c ck_b. This repeats for
   * every byte in the data to be summed over. 
   * The resulting two-byte checksum consists of \c 
   * ck_a in the low byte position and \c ck_b in the
   * high byte position.
   */
  for (uint16_t i=0; i < size; ++i)
  {
      ck_a += data[i];
      ck_b += ck_a;
  }

  // Store the high byte.
  ckloc[0] = ck_a;
  // Store the low byte.
  ckloc[1] = ck_b;
}

// Function to reverse the bit order of a byte.
uint8_t reverseBits(uint8_t b) {
  uint8_t result = 0;
  for (int i = 0; i < 8; i++) {
    result <<= 1;
    result |= (b & 1);
    b >>= 1;
  }
  return result;
}

// Prints a hex dump of the designated memory location.
void printHexDump(const uint8_t* data, size_t size) {
  // Number of bytes to display per line
  const int bytesPerLine = 16; 
  // Buffer to hold each byte representation
  char buffer[4]; 

  for (size_t i = 0; i < size; i += bytesPerLine) {
    // Print the address of the current line
    Serial.print("0x");
    // Pad the first line with an extra 0
    if (i < 0x10) {
      Serial.print('0');
    }
    Serial.print(i, HEX);
    Serial.print(": ");

    // Print the hex values of each byte
    for (size_t j = 0; j < bytesPerLine; j++) {
      if (i + j < size) {
        sprintf(buffer, "%02X ", data[i + j]);
        Serial.print(buffer);
      } else {
        // Print empty spaces for padding
        Serial.print("   "); 
      }
    }

    // Print the ASCII representation of the bytes
    Serial.print("  ");
    for (size_t j = 0; j < bytesPerLine; j++) {
      if (i + j < size) {
        char c = (data[i + j] >= 32 && data[i + j] <= 126) ? (char)data[i + j] : '.';
        Serial.print(c);
      } else {
        Serial.print(' ');
      }
    }

    Serial.println();
  }
}
