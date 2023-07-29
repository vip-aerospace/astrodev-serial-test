#include "reply.h"

// Used to decode reply packets from the radio.
void decodePacket(byte *payload, byte opcode){
    Serial.print("Packet Type: ");
    // Check the packet's opcode.
    switch(opcode){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        // A telemetry reply packet.
        case 7:
            Serial.println("Telemetry Reply Packet");

            Serial.print("op_counter: ");
            uint16_t op_counter = (payload[0] << 8) | payload[1];
            Serial.println(op_counter);

            Serial.print("msp430_temp: ");
            int16_t msp430_temp = (payload[2] << 8) | payload[3];
            Serial.println(msp430_temp);

            Serial.print("time_count[3]: ");
            uint32_t time_count = 0x00 | (payload[4] << 16) | (payload[5] << 8) | payload[6];
            Serial.println(time_count);
            Serial.print("corresponding to ");
            Serial.print(time_count * 2.5);
            Serial.println(" seconds");

            Serial.print("rssi: ");
            uint8_t rssi = payload[7];
            Serial.println(rssi);

            Serial.print("bytes_received: ");
            uint32_t bytes_received = (payload[8] << 24) | (payload[9] << 16) | (payload[10] << 8) | payload[11];
            Serial.println(bytes_received);

            Serial.print("bytes_transmitted: ");
            uint32_t bytes_transmitted = (payload[12] << 24) | (payload[13] << 16) | (payload[14] << 8) | payload[15];
            Serial.println(bytes_transmitted);

            Serial.print("rssi_lastpacket: ");
            uint8_t rssi_lastpacket = payload[16];
            Serial.println(rssi_lastpacket);

            Serial.print("rtc_alarm_flag: ");
            uint8_t rtc_alarm_flag = payload[17];
            Serial.println(rtc_alarm_flag);

            break;
    }
}
