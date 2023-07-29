#include "reply.h"

// Used to decode reply packets from the radio.
uint8_t decodeHeader(byte *header){
    Serial.print("Packet Type: ");
    // Check the packet's opcode.
    switch(header[3]){
        // 0x01: No-Op Ack
        case 1:
            Serial.print("No-Op ");
            ackHelper(header);
            return 0;
        // 0x02: Reset Ack
        case 2:
            Serial.println("Reset ");
            ackHelper(header);
            return 0;
        // 0x03: Transmit Ack
        case 3:
            Serial.println("Transmit ");
            ackHelper(header);
            return 0;
        // 0x04: Received Data
        case 4:
            Serial.println("Received Data");
            return (header[4] << 8) | header[5];
        // 0x05: Transceiver Configuration
        case 5:
            Serial.println("Transceiver Configuration");
            return (header[4] << 8) | header[5];
        // 0x06: Set Transceiver Configuration Ack
        case 6:
            Serial.println("Set Transceiver Configuration ");
            ackHelper(header);
            return 0;
        // 0x07: Telemetry
        case 7:
            Serial.println("Telemetry");
            return (header[4] << 8) | header[5];
        // 0x08: Write Flash Ack
        case 8:
            Serial.println("Write Flash ");
            ackHelper(header);
            return 0;
        // 0x09: RF Configure Ack
        case 9:
            Serial.println("RF Configure ");
            ackHelper(header);
            return 0;
        // 0x10: Beacon Data Ack
        case 16:
            Serial.println("Beacon Data ");
            ackHelper(header);
            return 0;
        // 0x11: Beacon Configure Ack
        case 17:
            Serial.println("Beacon Configure ");
            ackHelper(header);
            return 0;
        // 0x12: Firmware Rev
        case 18:
            Serial.println("Firmware Rev");
            return (header[4] << 8) | header[5];
        // 0x13: DIO Key Write Ack
        case 19:
            Serial.println("DIO Key Write ");
            ackHelper(header);
            return 0;
        // 0x14: Firmware Update Ack
        case 20:
            Serial.println("Firmware Update ");
            ackHelper(header);
            return 0;
        // 0x15: Firmware Packet Ack
        case 21:
            Serial.println("Firmware Packet ");
            ackHelper(header);
            return 0;
        // 0x20: Fast Set PA Ack
        case 32:
            Serial.println("Fast Set PA ");
            ackHelper(header);
            return 0;
        // Any other value
        default:
            Serial.println("Unknown");
            return 0;
    }
}

void decodePacket(byte *payload, byte opcode){
    // Check the packet's opcode to determine the payload structure.
    switch(opcode){
        // A received data packet.
        case 4:
            // No need to decode the data, it is printed raw in the hexdump.
            break;
        // A transceiver configuration packet.
        case 5: {
            Serial.print("interface_baud_rate: ");
            uint8_t interface_baud_rate = payload[0];
            switch (interface_baud_rate){
                case 0:
                    Serial.println("9600");
                    break;
                case 1:
                    Serial.println("19200");
                    break;
                case 2:
                    Serial.println("38400");
                    break;
                case 3:
                    Serial.println("57600");
                    break;
                case 4:
                    Serial.println("115200");
                    break;
                default:
                    Serial.println("Unknown");
                    break;
            }
            
            Serial.print("tx_power_amp_level: ");
            uint8_t tx_power_amp_level = payload[1];
            Serial.print(tx_power_amp_level);
            Serial.print(" (");
            float tx_power_amp_level_percent = (tx_power_amp_level / 255.0) * 100;
            Serial.print(tx_power_amp_level_percent);
            Serial.println("% of maximum value of 255)");

            Serial.print("rx_rf_baud_rate: ");
            uint8_t rx_rf_baud_rate = payload[2];
            switch (rx_rf_baud_rate){
                case 0:
                    Serial.println("1200");
                    break;
                case 1:
                    Serial.println("9600");
                    break;
                case 2:
                    Serial.println("19200");
                    break;
                case 3:
                    Serial.println("38400");
                    break;
                case 4:
                    Serial.println("57600");
                    break;
                case 5:
                    Serial.println("115200");
                    break;
                default:
                    Serial.println("Unknown");
                    break;
            }

            Serial.print("tx_rf_baud_rate: ");
            uint8_t tx_rf_baud_rate = payload[3];
            switch (tx_rf_baud_rate){
                case 0:
                    Serial.println("1200");
                    break;
                case 1:
                    Serial.println("9600");
                    break;
                case 2:
                    Serial.println("19200");
                    break;
                case 3:
                    Serial.println("38400");
                    break;
                case 4:
                    Serial.println("57600");
                    break;
                case 5:
                    Serial.println("115200");
                    break;
                default:
                    Serial.println("Unknown");
                    break;
            }

            Serial.print("rx_modulation: ");
            uint8_t rx_modulation = payload[4];
            switch (rx_modulation){
                case 0:
                    Serial.println("GFSK");
                    break;
                case 1:
                    Serial.println("AFSK");
                    break;
                case 2:
                    Serial.println("BPSK");
                    break;
                default:
                    Serial.println("Unknown");
                    break;
            }

            Serial.print("tx_modulation: ");
            uint8_t tx_modulation = payload[5];
            switch (tx_modulation){
                case 0:
                    Serial.println("GFSK");
                    break;
                case 1:
                    Serial.println("AFSK");
                    break;
                case 2:
                    Serial.println("BPSK");
                    break;
                default:
                    Serial.println("Unknown");
                    break;
            }

            Serial.print("rx_freq: ");
            uint32_t rx_freq = (payload[6] << 24) | (payload[7] << 16) | (payload[8] << 8) | payload[9];
            Serial.print(rx_freq);
            Serial.print("Hz (");
            float rx_freq_mhz = rx_freq / 1000000.0;
            Serial.print(rx_freq_mhz);
            Serial.println("MHz)");

            Serial.print("tx_freq: ");
            uint32_t tx_freq = (payload[10] << 24) | (payload[11] << 16) | (payload[12] << 8) | payload[13];
            Serial.print(tx_freq);
            Serial.print("Hz (");
            float tx_freq_mhz = tx_freq / 1000000.0;
            Serial.print(tx_freq_mhz);
            Serial.println("MHz)");

            Serial.print("source[6]: ");
            unsigned char source[6] = { payload[14], payload[15], payload[16], payload[17], payload[18], payload[19] };
            for (int i = 0; i < 6; i++) {
                Serial.print((char)source[i]);
            }
            Serial.println();

            Serial.print("destination[6]: ");
            unsigned char destination[6] = { payload[20], payload[21], payload[22], payload[23], payload[24], payload[25] };
            for (int i = 0; i < 6; i++) {
                Serial.print((char)destination[i]);
            }
            Serial.println();

            //TODO: continue from tx_preamble
            break;
        }
        // A telemetry reply packet.
        case 7: {
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
        // A firmware revision reply packet.
        case 18:
            Serial.print("firmware_rev: ");
            float firmware_rev = (payload[0] << 24) | (payload[1] << 16) | (payload[2] << 8) | payload[3];
            Serial.println(firmware_rev);
            break;
    }
}

// Used to determine if an ack-type header is an ack or not-ack.
void ackHelper(byte* header){
    // Check for ack/no-ack pattern in payload size bytes.
    if ((header[4] & 0x0F) == 0x0A && header[5] == 0x0A) {
        Serial.println("Ack");
    } else if ((header[4] & 0x0F) == 0x0F && header[5] == 0xFF) {
        Serial.println("Nack");
    }

    // The high 4 bits of the payload size are UART status flags.
    Serial.print("UART status bits: ");
    Serial.print(header[4] & 0x80);
    Serial.print(header[4] & 0x40);
    Serial.print(header[4] & 0x20);
    Serial.println(header[4] & 0x10);
}
