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
            Serial.print("Reset ");
            ackHelper(header);
            return 0;
        // 0x03: Transmit Ack
        case 3:
            Serial.print("Transmit ");
            ackHelper(header);
            return 0;
        // 0x04: Received Data
        case 4:
            Serial.print("Received Data");
            return (header[4] << 8) | header[5];
        // 0x05: Transceiver Configuration
        case 5:
            Serial.print("Transceiver Configuration");
            return (header[4] << 8) | header[5];
        // 0x06: Set Transceiver Configuration Ack
        case 6:
            Serial.print("Set Transceiver Configuration ");
            ackHelper(header);
            return 0;
        // 0x07: Telemetry
        case 7:
            Serial.print("Telemetry");
            return (header[4] << 8) | header[5];
        // 0x08: Write Flash Ack
        case 8:
            Serial.print("Write Flash ");
            ackHelper(header);
            return 0;
        // 0x09: RF Configure Ack
        case 9:
            Serial.print("RF Configure ");
            ackHelper(header);
            return 0;
        // 0x10: Beacon Data Ack
        case 16:
            Serial.print("Beacon Data ");
            ackHelper(header);
            return 0;
        // 0x11: Beacon Configure Ack
        case 17:
            Serial.print("Beacon Configure ");
            ackHelper(header);
            return 0;
        // 0x12: Firmware Rev
        case 18:
            Serial.print("Firmware Rev");
            return (header[4] << 8) | header[5];
        // 0x13: DIO Key Write Ack
        case 19:
            Serial.print("DIO Key Write ");
            ackHelper(header);
            return 0;
        // 0x14: Firmware Update Ack
        case 20:
            Serial.print("Firmware Update ");
            ackHelper(header);
            return 0;
        // 0x15: Firmware Packet Ack
        case 21:
            Serial.print("Firmware Packet ");
            ackHelper(header);
            return 0;
        // 0x20: Fast Set PA Ack
        case 32:
            Serial.print("Fast Set PA ");
            ackHelper(header);
            return 0;
        // Any other value
        default:
            Serial.print("Unknown");
            return 0;
    }
}

// Used to decode a replied packet from the radio.
void decodePacket(byte *payload, byte opcode){
    // Check the packet's opcode to determine the payload structure.
    switch(opcode){
        // A received data packet.
        case 4:
            // No need to decode the data, it is printed raw in the hexdump.
            break;
        // A transceiver configuration packet.
        // The configuration of the radio is returned as a 34-byte payload.
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
            uint32_t rx_freq = (payload[9] << 24) | (payload[8] << 16) | (payload[7] << 8) | payload[6];
            Serial.print(rx_freq);
            Serial.print("Hz (");
            float rx_freq_mhz = rx_freq / 1000.0;
            Serial.print(rx_freq_mhz);
            Serial.println("MHz)");

            Serial.print("tx_freq: ");
            uint32_t tx_freq = (payload[13] << 24) | (payload[12] << 16) | (payload[11] << 8) | payload[10];
            Serial.print(tx_freq);
            Serial.print("Hz (");
            float tx_freq_mhz = tx_freq / 1000.0;
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

            Serial.print("tx_preamble: ");
            uint16_t tx_preamble = (payload[26] << 8) | payload[27];
            switch (tx_preamble){
                case 0:
                    Serial.println("20 flags");
                    break;
                default:
                    Serial.println("Unknown");
                    break;
            }

            Serial.print("tx_postamble: ");
            uint16_t tx_postamble = (payload[28] << 8) | payload[29];
            switch (tx_postamble){
                case 0:
                    Serial.println("20 flags");
                    break;
                default:
                    Serial.println("Unknown");
                    break;
            }

            Serial.println("function_config: ");
            uint16_t function_config = (payload[30] << 8) | payload[31];

            Serial.print("  GPIO_B, Pin #10: ");
            uint16_t gpiob_p10 = function_config & 0b11;
            switch (gpiob_p10)
            {
                case 0b00:
                    Serial.println("Off Logic Low");
                    break;
                case 0b01:
                    Serial.println("2.0 second Toggle");
                    break;
                case 0b10:
                    Serial.println("TX Packet Toggle");
                    break;
                case 0b11:
                    Serial.println("Rx Packet Toggle");
                    break;
                default:
                    Serial.println("Unknown");
                    break;
            }

            Serial.print("  Config Pin 1, Pin #14: ");
            uint16_t config1_p14 = (function_config >> 2) & 0b11;
            switch (config1_p14)
            {
                case 0b00:
                    Serial.println("Off Logic Low");
                    break;
                case 0b01:
                    Serial.println("Tx/Rx Switch");
                    break;
                case 0b10:
                    Serial.println("2.0 hz WDT");
                    break;
                case 0b11:
                    Serial.println("Rx Packet Toggle");
                    break;
                default:
                    Serial.println("Unknown");
                    break;
            }

            Serial.print("  External Event, Pin #12: ");
            uint16_t ee_p12_p = (function_config >> 5) & 0b1;
            switch (ee_p12_p)
            {
                case 0b0:
                    Serial.print("Pattern A, ");
                    break;
                case 0b1:
                    Serial.print("Pattern B, ");
                    break;
                default:
                    Serial.print("Unknown");
                    break;
            }
            uint16_t ee_p12 = (function_config >> 4) & 0b11;
            switch (ee_p12)
            {
                case 0b00:
                    Serial.println("Off Logic Low");
                    break;
                case 0b01:
                    Serial.println("Enable");
                    break;
                default:
                    Serial.print("Unknown");
                    break;
            }

            Serial.println("  CRC Functions:");

            Serial.print("    RX CRC ");
            uint16_t rx_crc = (function_config >> 6) & 0b1;
            switch (rx_crc)
            {
                case 0b0:
                    Serial.println("Disabled");
                    break;
                case 0b1:
                    Serial.println("Enabled");
                    break;
                default:
                    Serial.println("Unknown");
                    break;
            }

            Serial.print("    TX CRC (TBD) ");
            uint16_t tx_crc = (function_config >> 7) & 0b1;
            switch (tx_crc)
            {
                case 0b0:
                    Serial.println("Disabled");
                    break;
                case 0b1:
                    Serial.println("Enabled");
                    break;
                default:
                    Serial.println("Unknown");
                    break;
            }

            Serial.println("  Telemetry Functions:");

            Serial.print("    Telemetry Packet Logging ");
            uint16_t tel_pack_log = (function_config >> 8) & 0b1;
            switch (tel_pack_log)
            {
                case 0b0:
                    Serial.println("Disabled");
                    break;
                case 0b1:
                    Serial.println("Enabled");
                    break;
                default:
                    Serial.println("Unknown");
                    break;
            }

            Serial.print("    Logging Rate: ");
            uint16_t logging_rate = (function_config >> 9) & 0b11;
            switch (logging_rate)
            {
                case 0b00:
                    Serial.println("1/10 Hz");
                    break;
                case 0b01:
                    Serial.println("1 Hz");
                    break;
                case 0b10:
                    Serial.println("2 Hz");
                    break;
                case 0b11:
                    Serial.println("4 Hz");
                    break;
                default:
                    Serial.println("Unknown");
                    break;
            }

            Serial.print("    Telemetry Dump ");
            uint16_t tel_dump = (function_config >> 11) & 0b1;
            switch (tel_dump)
            {
                case 0b0:
                    Serial.println("Disabled");
                    break;
                case 0b1:
                    Serial.println("Enabled");
                    break;
                default:
                    Serial.println("Unknown");
                    break;
            }

            Serial.println("  Beacon Functions:");

            Serial.print("    Ping Return ");
            uint16_t ping_rtn = (function_config >> 12) & 0b1;
            switch (ping_rtn)
            {
                case 0b0:
                    Serial.println("Disabled");
                    break;
                case 0b1:
                    Serial.println("Enabled");
                    break;
                default:
                    Serial.println("Unknown");
                    break;
            }

            Serial.print("    Code Upload ");
            uint16_t code_upld = (function_config >> 13) & 0b1;
            switch (code_upld)
            {
                case 0b0:
                    Serial.println("Disabled");
                    break;
                case 0b1:
                    Serial.println("Enabled");
                    break;
                default:
                    Serial.println("Unknown");
                    break;
            }

            Serial.print("    System Reset ");
            uint16_t sys_rst = (function_config >> 14) & 0b1;
            switch (sys_rst)
            {
                case 0b0:
                    Serial.println("Disabled");
                    break;
                case 0b1:
                    Serial.println("Enabled");
                    break;
                default:
                    Serial.println("Unknown");
                    break;
            }

            Serial.println("  Factory Default Restored:");

            Serial.print("    Factory settings restore complete flag: ");
            uint16_t fact_rst = (function_config >> 15) & 0b1;
            switch (fact_rst)
            {
                case 0b0:
                    Serial.println("False");
                    break;
                case 0b1:
                    Serial.println("True");
                    break;
                default:
                    Serial.println("Unknown");
                    break;
            }

            //TODO: complete function_config2
            Serial.println("function_config2: ");
            uint16_t function_config2 = (payload[32] << 8) | payload[33];

            Serial.print("  AFC ");
            uint16_t afc = function_config2 & 0b1;
            switch (afc)
            {
                case 0b0:
                    Serial.println("Disabled");
                    break;
                case 0b1:
                    Serial.println("Enabled");
                    break;
                default:
                    Serial.println("Unknown");
                    break;
            }

            Serial.print("  RX CW: ");
            uint16_t rx_cw = (function_config2 >> 1) & 0b1;
            switch (rx_cw)
            {
                case 0b0:
                    Serial.println("False");
                    break;
                case 0b1:
                    Serial.println("True");
                    break;
                default:
                    Serial.println("Unknown");
                    break;
            }

            Serial.print("  TX CW: ");
            uint16_t tx_cw = (function_config2 >> 2) & 0b1;
            switch (tx_cw)
            {
                case 0b0:
                    Serial.println("False");
                    break;
                case 0b1:
                    Serial.println("True");
                    break;
                default:
                    Serial.println("Unknown");
                    break;
            }

            Serial.print("  Radio Alarm Transmit ");
            uint16_t radio_alarm_xmit = (function_config2 >> 3) & 0b1;
            switch (radio_alarm_xmit)
            {
                case 0b0:
                    Serial.println("Disabled");
                    break;
                case 0b1:
                    Serial.println("Enabled");
                    break;
                default:
                    Serial.println("Unknown");
                    break;
            }

            Serial.print("  Encryption A ");
            uint16_t enc_a = (function_config2 >> 4) & 0b1;
            switch (enc_a)
            {
                case 0b0:
                    Serial.println("Disabled");
                    break;
                case 0b1:
                    Serial.println("Enabled");
                    break;
                default:
                    Serial.println("Unknown");
                    break;
            }

            Serial.print("  Encryption B ");
            uint16_t enc_b = (function_config2 >> 5) & 0b1;
            switch (enc_b)
            {
                case 0b0:
                    Serial.println("Disabled");
                    break;
                case 0b1:
                    Serial.println("Enabled");
                    break;
                default:
                    Serial.println("Unknown");
                    break;
            }

            Serial.print("  Radio Configration Duration ");
            uint16_t radio_config_dur = (function_config2 >> 6) & 0b1;
            switch (radio_config_dur)
            {
                case 0b0:
                    Serial.println("Disabled");
                    break;
                case 0b1:
                    Serial.println("Enabled");
                    break;
                default:
                    Serial.println("Unknown");
                    break;
            }

            Serial.print("  No AX25 Header but has 16 bit CRC: ");
            uint16_t no_ax25_header = (function_config2 >> 7) & 0b1;
            switch (no_ax25_header)
            {
                case 0b0:
                    Serial.println("False");
                    break;
                case 0b1:
                    Serial.println("True");
                    break;
                default:
                    Serial.println("Unknown");
                    break;
            }

            // Remaining bits in function_config2 are BSL TBD

            break;
        }
        // A telemetry reply packet.
        case 7: {
            Serial.print("op_counter: ");
            uint16_t op_counter = (payload[1] << 8) | payload[0];
            Serial.println(op_counter);

            Serial.print("msp430_temp: ");
            int16_t msp430_temp = (payload[3] << 8) | payload[2];
            Serial.println(msp430_temp);

            Serial.print("time_count[3]: ");
            uint32_t time_count = 0x00 | (payload[6] << 16) | (payload[5] << 8) | payload[4];
            Serial.println(time_count);
            Serial.print("corresponding to ");
            Serial.print(time_count * 2.5);
            Serial.println(" seconds");

            Serial.print("rssi: ");
            uint8_t rssi = payload[7];
            Serial.println(rssi);

            Serial.print("bytes_received: ");
            uint32_t bytes_received = (payload[11] << 24) | (payload[10] << 16) | (payload[9] << 8) | payload[8];
            Serial.println(bytes_received);

            Serial.print("bytes_transmitted: ");
            uint32_t bytes_transmitted = (payload[15] << 24) | (payload[14] << 16) | (payload[13] << 8) | payload[12];
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
            float firmware_rev = (payload[3] << 24) | (payload[2] << 16) | (payload[1] << 8) | payload[0];
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
        Serial.print("BSL");
    }

    // The high 4 bits of the payload size are UART status flags.
    Serial.print("Transmit Buffer Full Flag: ");
    Serial.println(header[4] & 0x10);
    Serial.print("GPIO A State: ");
    Serial.println(header[4] & 0x20);
    Serial.print("GPIO B State: ");
    Serial.println(header[4] & 0x40);
    Serial.print("External Event State: ");
    Serial.println(header[4] & 0x80);
}
