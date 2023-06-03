# astrodev-serial-test
This is an Arduino sketch used to test serial communication to the Astrodev (Li-3) radio. 

## Setup
There are two ways to use this program: back-to-back testing between Teensys, and direct testing with an Astrodev radio.
### Back-to-Back Testing
To test the program and verify the data you are sending, connect two Teensys as follows:

- Connect the ground pins of both Teensys.
- Connect the Serial1 TX pin of Teensy 1 (Pin 1) to the Serial1 RX pin of Teensy 2 (Pin 0).
- Connect the Serial1 RX pin of Teensy 1 (Pin 0) to the Serial1 TX pin of Teensy 2 (Pin 1).

Upload the code to **both** Teensys, and open a serial monitor for each board. Commanding one board to send a packet will result in that packet appearing on the other Teensy's Serial Monitor.

<img src="documentation\back-to-back.png" alt="Packet data being transmitted from one Teensy to another Teensy.">

NOTE: Arduino IDE 2 does not allow opening multiple windows and Serial Monitors in parallel. It is still possible to use the program:

- With both Teensys connected to the same computer, you can send a command to one Teensy, seeing the transmitted packet on the Serial Monitor. You can then switch the active board to the other Teensy and see what the other Teensy has received. The Serial Monitor is **not** cleared after switching the active board.
- You can also use two computers, each connected to a Teensy running Arduino IDE 2.

Use of [Arduino IDE 1.8.19](https://www.arduino.cc/en/software) is highly recommended for back-to-back testing.

### Direct Astrodev Testing
When ready to test the Astrodev radio, connect it to the Teensy as follows:

- Connect the ground pin of the Teensy to the ground pin of the Astrodev radio.
- Connect the Serial1 TX pin of the Teensy (Pin 1) to the RX pin of the Astrodev radio's carrier board.
- Connect the Serial1 RX pin of the Teensy (Pin 0) to the TX pin of the Astrodev radio's carrier board.
- Ensure 7VDC power is connected to the radio.
- **IMPORTANT: Ensure the radio has an antenna or dummy load connected if planning to perform RF transmit/receive tests.**

An image of the Li-3 connection pins labelled is included below. The carrier board in the lab has pre-labelled Dupont wires attached already.

<img src="documentation\astrodev-pins.png" alt="The pins on the Astrodev carrier board.">

Once connected, you can send test packets to the radio and observe the replies.


## Commands
The program uses the Serial Monitor input to determine which packet to send to the Astrodev radio. This input is a single character, which corresponds to a command to be sent from the Teensy to the radio. Every packet going from the Teensy to the radio has an opcode starting with '10', while every packet going from the radio to the Teensy has an opcode starting with '20'. Look for this in the received data in the Serial Monitor. 

<img src="documentation\opcodes.png" alt="The location of the opcodes for transmitted and received packets.">


The list of commands from the Teensy, and their corresponding reply from the radio, are listed below. This program implements the commands defined in [the Astrodev documentation (PDF)](http://www.astrodevllc.com/public_html3/datasheet/Radio_Interface_Manual_11062021.pdf).


| Command | Opcode | Command Name | Arguments | Summary |
| ------- | ------ | ------------ | --------- | ------- |
| 'a'     | 0x1001 | No-Op        | -         | A "dummy" command. Advances the operations counter, but no other effects. |
|         | 0x2002 | No-Op Ack    | -         | Acknowledgement. |
| 'b'     | 0x1002 | Reset        | -         | Soft reset of radio. |
|         | 0x2002 | Reset Ack    | -         | Acknowledgement. |
| 'c'     | 0x1003 | Transmit     | N bytes   | Transmit the argument data over RF. |
|         | 0x2003 | Transmit Ack | -         | Acknowledgement. |
|         | 0x2004 | Receive      | N bytes   | Data received over RF. |
| 'd'     | 0x1005 | Get Transceiver Configuration | - | Get the radio's configuration data. |
|         | 0x2005 | Transceiver Configuration | N bytes | The transceiver's configuration data. |
| 'e'     | 0x1006 | Set Transceiver Configuration | N bytes | Set the radio's configuration data. |
|         | 0x2006 | Set Transceiver Configuration Ack    | - | Acknowledgement. |
| 'f'     | 0x1007 | Telemetry    | -         | Query telemetry data. |
|         | 0x2007 | Telemetry    | N bytes   | Telemetry data. |
| 'g'     | 0x1008 | Write Flash  | 16 Bytes  | Write flash with MD5 checksum. |
|         | 0x2008 | Write Flash Ack | -      | Acknowledgement. |
| 'h'     | 0x1009 | RF Configure | N Bytes   | Low Level RF configuration data. |
|         | 0x2009 | RF Configure Ack | -     | Acknowledgement. |
| 'i'     | 0x1010 | Beacon Data  | N Bytes   | Beacon contents data. |
|         | 0x2010 | Beacon Data Ack | -      | Acknowledgement. |
| 'j'     | 0x1011 | Beacon Configure | N Bytes   | Set beacon configuration data. |
|         | 0x2011 | Beacon Configure Ack | -      | Acknowledgement. |
| 'k'     | 0x1012 | Get Firmware Revision | - | Get the radio's firmware revision number. |
|         | 0x2012 | Firmware Revision | 4 Bytes | The radio's firmware revision number. |
| 'l'     | 0x1013 | DI/O Key Write | 16 Bytes | The Digital I/O key. |
|         | 0x2013 | DI/O Key Write Ack | -   | Acknowledgement. |
| 'm'     | 0x1014 | Firmware Update | 16 Bytes | Firmware Update MD5 checksum. Unimplemented in the current Astrodev firmware. |
|         | 0x2014 | Firmware Update Ack | -   | Acknowledgement. |
| 'n'     | 0x1015 | Firmware Packet | ? Bytes | Firmware Update data. Unimplemented in the current Astrodev firmware. |
|         | 0x2015 | Firmware Packet Ack | -   | Acknowledgement. |
| 'o'     | 0x1020 | Fast Set PA | 1 Byte     | Set Power Amplifier level. |
|         | 0x2020 | Fast Set PA Ack | -   | Acknowledgement. |

## Packet data
Default data for packets to be sent to the radio is included at the top of the serial_test.ino file. This data may be altered as desired to modify the radio's behavior. The structures are stored as raw bytes in an array as described in  [the Astrodev documentation (PDF)](http://www.astrodevllc.com/public_html3/datasheet/Radio_Interface_Manual_11062021.pdf). The following are a list of the possible payloads:

- `xmitpayload[]`: The message to be transmitted by the radio.
- `setxcvrconfigpayload[]`: The transciever configuration structure. 
- `flashpayload[]`: The MD5 checksum for the radio flash.
- `rfconfigpayload[]`: The low-level radio RF configuration structure.
- `beacondatapayload[]`: The message to be periodically transmitted by the radio.
- `beaconconfigpayload[]`: The beacon configuration structure.
- `diopayload[]`: The Digital I/O key.
- `fastpapayload[]`: The power amplifier configuration structure.

Received data is printed to the Serial Monitor in a raw bytes format, with the header printed on its own line before the packet's payload. There is no processing of this data beyond printing the raw received bytes.