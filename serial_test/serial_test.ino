#include <string.h>
#include "helpers.h"
#include "reply.h"

#define HEADERSIZE 8
#define CHECKSUMSIZE 2

// The transmitted packet data.
// The packet.
byte* packet;
// The size of the packet.
uint16_t packetSize;
// The header of the packet.
byte header[HEADERSIZE];

// The received packet data.
// The header portion of the packet.
byte receivedHeader[HEADERSIZE];
// The payload portion of the packet consists of a 255-byte payload and a two-byte checksum.
byte receivedPayload[255 + CHECKSUMSIZE];

// Used for both transmitted and received packet.
// The size of the payload.
uint8_t payloadsize;

// Payloads for transmitted packet.
// Message to be transmitted by the radio.
uint8_t xmitpayload[] = { "Hello World!" };
// Transceiver configuration data.
uint8_t setxcvrconfigpayload[] = {
  0x00,                                   // Radio Interface Baud Rate (9600=0x00)
  0x00,                                   // Tx Power Amp level (min = 0x00 max = 0xFF)
  0x00,                                   // Radio RX RF Baud Rate (9600=0x00)
  0x00,                                   // Radio TX RF Baud Rate (9600=0x00)
  0x00,                                   // (0x00 = GFSK)
  0x00,                                   // (0x00 = GFSK)
  0x02, 0xAE, 0xA5, 0x40,                 // Channel Rx Frequency (ex: 45000000)
  0x02, 0xAE, 0xA5, 0x40,                 // Channel Tx Frequency (ex: 45000000)
  0x4E, 0x4F, 0x43, 0x41, 0x4C, 0x4C,     // AX25 Mode Source Call Sign (default NOCALL)
  0x43, 0x51, 0x00, 0x00, 0x00, 0x00,     // AX25 Mode Destination Call Sign (default CQ)
  0x00, 0x00,                             // AX25 Mode Tx Preamble Byte Length (0x00 = 20 flags)
  0x00, 0x00,                             // AX25 Mode Tx Postamble Byte Length (0x00 = 20 flags)
  0x00, 0x00,                             // Radio Configuration Discrete Behaviors
  0x00, 0x00                              // Radio Configuration Discrete Behaviors 2
};
// MD5 checksum for radio flash.
uint8_t flashpayload[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
// Low level RF configuration data.
uint8_t rfconfigpayload[] = {
  0x00,                                   // Front end level (0-63)
  0x00,                                   // Tx power amp level (0-255, non-linear)
  0x00, 0x00, 0x00, 0x00,                 // Tx freq offset (up to 20 kHz)
  0x00, 0x00, 0x00, 0x00,                 // Rf freq offset (up to 20 kHz)
  0x00,                                   // Tx freq deviation (0 (2.7 kHz), 1 (5.4 kHz), 2 (10.8 kHz), 3 (21.6 kHz), 4 (43.2 kHz)) CAUTION
  0x00,                                   // Rx freq deviation (N/A for firmware release 3.10)
  0x00,                                   // Pre xmit delay (in tens of milliseconds. Default 1 second = 100)
  0x00                                    // Post xmit delay (Delay in tens of milliseconds. Default 0 (NOT IMPLEMENTED USE POSTAMBLE))
};
// Message to be periodically transmitted by the radio.
uint8_t beacondatapayload[] = { "Hello Beacon!" };
// Beacon configuration data.
uint8_t beaconconfigpayload[] = { 0x00 }; // How frequently to transmit the beacon (value of 0 is off, 2.5 sec delay per LSB)
// Digital I/O key payload.
uint8_t diopayload[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
// Fast power amplifier data payload.
uint8_t fastpapayload[] = { 0xFF };

void setup() {
  // Initialize the Serial Monitor
  Serial.begin(9600);
  // Initialize Serial connection to radio
  Serial1.begin(9600);     
  // Initialize the header with default values
  initializeHeader(header);
}

void loop() {
  // Check if data is available in Serial Monitor
  if (Serial.available()) {
    // Read the incoming character
    char data = Serial.read();
    switch(data) {
      // Send a NOOP packet. This "packet" has no payload and is just a header.
      case 'a':
        setHeader(header, 0x01, 0);
        Serial1.write(header, HEADERSIZE);
        Serial.println("Sent NOOP packet");
        printHexDump(header, HEADERSIZE);
        break;
      // Send a radio reset packet. This "packet" has no payload and is just a header.
      case 'b':
        setHeader(header, 0x02, 0);
        Serial1.write(header, HEADERSIZE);
        Serial.println("Sent reset packet");
        printHexDump(header, HEADERSIZE);
        break;
      // Send a transmit packet. This packet has an N-byte payload.
      case 'c':
        // The payload is the data to be transmitted to the ground station.
        payloadsize = sizeof(xmitpayload);
        
        // Fill in the header.
        setHeader(header, 0x03, payloadsize);
        
        // Create a packet to hold the header and payload.
        packetSize = HEADERSIZE + payloadsize + CHECKSUMSIZE;
        packet = (byte*)malloc(packetSize);
        
        // Copy the header to the packet.
        memcpy(packet, header, HEADERSIZE);
        // Copy the payload to the packet.
        memcpy(packet + HEADERSIZE, xmitpayload, payloadsize);

        // Calculate the checksum over the header and payload.
        calcCS(&packet[CHECKSUMSIZE], packetSize - 4, &packet[packetSize - CHECKSUMSIZE]);

        // Transmit the packet to the radio.
        Serial1.write(packet, packetSize);
        Serial.println("Sent transmit packet");
        printHexDump(packet, packetSize);
        
        // Free the packet's memory before moving on.
        free(packet);
        break;
      // Send a Get Transciever Configuration packet. This "packet" has no payload and is just a header.
      case 'd':
        setHeader(header, 0x05, 0);
        Serial1.write(header, HEADERSIZE);
        Serial.println("Sent get config packet");
        printHexDump(header, HEADERSIZE);
        break;
      // Send a transceiver configuration packet. This packet has an N-byte payload.
      case 'e':
        // The payload is the transceiver configuration data.
        payloadsize = sizeof(setxcvrconfigpayload);

        // Fill in the header.
        setHeader(header, 0x06, payloadsize);

        // Create a packet to hold the header and payload.
        packetSize = HEADERSIZE + payloadsize + CHECKSUMSIZE;
        packet = (byte*)malloc(packetSize);
        
        // Copy the header to the packet.
        memcpy(packet, header, HEADERSIZE);
        // Copy the payload to the packet.
        memcpy(packet + HEADERSIZE, setxcvrconfigpayload, payloadsize);

        // Calculate the checksum over the header and payload.
        calcCS(&packet[CHECKSUMSIZE], packetSize - 4, &packet[packetSize - CHECKSUMSIZE]);

        // Transmit the packet to the radio.
        Serial1.write(packet, packetSize);
        Serial.println("Sent transmit packet");
        printHexDump(packet, packetSize);
        
        // Free the packet's memory before moving on.
        free(packet);
        break;
      // Send a Get Telemetry packet. This "packet" has no payload and is just a header.
      case 'f':
        setHeader(header, 0x07, 0);
        Serial1.write(header, HEADERSIZE);
        Serial.println("Sent get telem packet");
        printHexDump(header, HEADERSIZE);
        break;
      // Send a write flash packet. This packet has a 16-byte payload.
      case 'g':
        // The payload is the 16-byte MD5 checksum of the flash.
        payloadsize = sizeof(flashpayload);

        // Fill in the header.
        setHeader(header, 0x08, payloadsize);

        // Create a packet to hold the header and payload.
        packetSize = HEADERSIZE + payloadsize + CHECKSUMSIZE;
        packet = (byte*)malloc(packetSize);
        
        // Copy the header to the packet.
        memcpy(packet, header, HEADERSIZE);
        // Copy the payload to the packet.
        memcpy(packet + HEADERSIZE, flashpayload, payloadsize);

        // Calculate the checksum over the header and payload.
        calcCS(&packet[CHECKSUMSIZE], packetSize - 4, &packet[packetSize - CHECKSUMSIZE]);

        // Transmit the packet to the radio.
        Serial1.write(packet, packetSize);
        Serial.println("Sent write flash packet");
        printHexDump(packet, packetSize);
        
        // Free the packet's memory before moving on.
        free(packet);
        break;
      // Send an RF configure packet. This packet has an N-byte payload.
      case 'h':
        // The payload is the RF configuration.
        payloadsize = sizeof(rfconfigpayload);

        // Fill in the header.
        setHeader(header, 0x09, payloadsize);

        // Create a packet to hold the header and payload.
        packetSize = HEADERSIZE + payloadsize + CHECKSUMSIZE;
        packet = (byte*)malloc(packetSize);
        
        // Copy the header to the packet.
        memcpy(packet, header, HEADERSIZE);
        // Copy the payload to the packet.
        memcpy(packet + HEADERSIZE, rfconfigpayload, payloadsize);

        // Calculate the checksum over the header and payload.
        calcCS(&packet[CHECKSUMSIZE], packetSize - 4, &packet[packetSize - CHECKSUMSIZE]);

        // Transmit the packet to the radio.
        Serial1.write(packet, packetSize);
        Serial.println("Sent RF config packet");
        printHexDump(packet, packetSize);
        
        // Free the packet's memory before moving on.
        free(packet);
        break;
      // Send a beacon data packet. This packet has an N-byte payload.
      case 'i':
        // The payload is the beacon message that the radio periodically transmits.
        payloadsize = sizeof(beacondatapayload);

        // Fill in the header.
        setHeader(header, 0x10, payloadsize);

        // Create a packet to hold the header and payload.
        packetSize = HEADERSIZE + payloadsize + CHECKSUMSIZE;
        packet = (byte*)malloc(packetSize);
        
        // Copy the header to the packet.
        memcpy(packet, header, HEADERSIZE);
        // Copy the payload to the packet.
        memcpy(packet + HEADERSIZE, beacondatapayload, payloadsize);

        // Calculate the checksum over the header and payload.
        calcCS(&packet[CHECKSUMSIZE], packetSize - 4, &packet[packetSize - CHECKSUMSIZE]);

        // Transmit the packet to the radio.
        Serial1.write(packet, packetSize);
        Serial.println("Sent beacon data packet");
        printHexDump(packet, packetSize);
        
        // Free the packet's memory before moving on.
        free(packet);
        break;
      // Send a beacon configuration packet. This packet has an N-byte payload.
      case 'j':
        // The payload is the beacon configuration struct, which has one 1-byte member. 
        // The member is the beacon_interval, which sets the interval between regular beacon transmission by the radio.
        // Each LSB adds 2.5 seconds to the delay. A value of 0x00 disables the beacon.
        payloadsize = sizeof(beaconconfigpayload);
        
        // Fill in the header.
        setHeader(header, 0x11, payloadsize);

        // Create a packet to hold the header and payload.
        packetSize = HEADERSIZE + payloadsize + CHECKSUMSIZE;
        packet = (byte*)malloc(packetSize);
        
        // Copy the header to the packet.
        memcpy(packet, header, HEADERSIZE);
        // Copy the payload to the packet.
        memcpy(packet + HEADERSIZE, beaconconfigpayload, payloadsize);

        // Calculate the checksum over the header and payload.
        calcCS(&packet[CHECKSUMSIZE], packetSize - 4, &packet[packetSize - CHECKSUMSIZE]);

        // Transmit the packet to the radio.
        Serial1.write(packet, packetSize);
        Serial.println("Sent beacon config packet");
        printHexDump(packet, packetSize);
        
        // Free the packet's memory before moving on.
        free(packet);
        break;
      // Send a Get Firmware packet. This "packet" has no payload and is just a header.
      case 'k':
        setHeader(header, 0x12, 0);
        Serial1.write(header, 8);
        Serial.println("Sent get firmware rev packet");
        printHexDump(header, 8);
        break;
      // Send a DIO key write packet. This packet has a 16-byte payload.
      case 'l':
        // The payload is a 16-byte key allowing control of the digital I/O pins from the ground.
        payloadsize = sizeof(diopayload);

        // Fill in the header.
        setHeader(header, 0x13, payloadsize);

        // Create a packet to hold the header and payload.
        packetSize = HEADERSIZE + payloadsize + CHECKSUMSIZE;
        packet = (byte*)malloc(packetSize);
        
        // Copy the header to the packet.
        memcpy(packet, header, HEADERSIZE);
        // Copy the payload to the packet.
        memcpy(packet + HEADERSIZE, diopayload, payloadsize);

        // Calculate the checksum over the header and payload.
        calcCS(&packet[CHECKSUMSIZE], packetSize - 4, &packet[packetSize - CHECKSUMSIZE]);

        // Transmit the packet to the radio.
        Serial1.write(packet, packetSize);
        Serial.println("Sent DIO key write packet");
        printHexDump(packet, packetSize);
        
        // Free the packet's memory before moving on.
        free(packet);
        break;
      // Send a Firmware update packet. This is not implemented in the radio firmware.
      case 'm':
        Serial.println("Not implemented in radio firmware.");
        break;
      // Send a Firmware write packet. This is not implemented in the radio firmware.
      case 'n':
        Serial.println("Not implemented in radio firmware.");
        break;
      // Send a fast set power amp packet. This packet has a one-byte payload.
      case 'o':
        // The payload is a single byte controlling the amplification of the transmit power amplifier.
        // 0x00 is minimum, 0xFF is maximum amplification.
        payloadsize = sizeof(fastpapayload);

        // Fill in the header.
        setHeader(header, 0x20, payloadsize);

        // Create a packet to hold the header and payload.
        packetSize = HEADERSIZE + payloadsize + CHECKSUMSIZE;
        packet = (byte*)malloc(packetSize);
        
        // Copy the header to the packet.
        memcpy(packet, header, sizeof(header));
        // Copy the payload to the packet.
        memcpy(packet + HEADERSIZE, fastpapayload, payloadsize);

        // Calculate the checksum over the header and payload.
        calcCS(&packet[CHECKSUMSIZE], packetSize - 4, &packet[packetSize - CHECKSUMSIZE]);

        // Transmit the packet to the radio.
        Serial1.write(packet, packetSize);
        Serial.println("Sent fast set power amp packet");
        printHexDump(packet, packetSize);
        
        // Free the packet's memory before moving on.
        free(packet);
        break;
      default:
        break;
    }
  }

  // Check if 8 bytes are available on Serial connection to radio.
  if (Serial1.available() >= 8) {    
    // Read 8 bytes into received header.
    Serial1.readBytes(receivedHeader, 8);
    // Determine the size of the payload as described in the received header.
    payloadsize = decodeHeader(receivedHeader);
    // If there are bytes in the packet's payload,
    if(payloadsize){
      // receive that payload, plus the two-byte checksum.
      Serial1.readBytes(receivedPayload, payloadsize + CHECKSUMSIZE);
      Serial.print("Received ");
      Serial.print(payloadsize);
      Serial.println("-byte payload.");
      // Interpet the received packet.
      decodePacket(receivedPayload, receivedHeader[3]);
    }
    // Print the raw packet for interpetation.
    Serial.println("Raw packet: ");
    printHexDump(receivedHeader, 8);
    if(payloadsize){
      printHexDump(receivedPayload, payloadsize + CHECKSUMSIZE);
    }
    Serial.println();
  }
}
