# astrodev-serial-test
This is an Arduino sketch used to test serial communication to the Astrodev (Li-3) radio. 

## Setup
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

## Replies from the radio
Received data is decoded for the four types of radio reply packets that contain a payload. These are:
- Received Data (opcode `0x2004`): The raw data packet received by the radio's RF connection.
- Transceiver Configuration (opcode `0x2005`): The radio configuration struct indicating the current status of the radio's settings.
- Telemetry (opcode `0x2007`): The radio telemetry struct indicating the current usage and status statistics of the radio.
- Firmware Revision (opcode `0x2012`): A four-byte float representing the current firmware revision flashed to the radio.

<img src="documentation\decoding.png" alt="The Transceiver Configuration struct decoded and printed to the Serial Monitor.">

For header-only reply packets without a payload, the header data is read to determine the type of reply and whether it is an ack or not-ack. The UART status bits are also decoded.

<img src="documentation\decoding-header.png" alt="The no-op ack header and UART status bits being decoded and printed to the Serial Monitor.">

For all replies, the raw packet in its entirety is printed.


