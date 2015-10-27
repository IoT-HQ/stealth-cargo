/*  
 *  Gateway Path Command to Data Originator Version 3
 *  =================================================
 *  Description: Send GET PACKET PATH command to Serial data sender UID
 *  Author: Faizal Mansor
 *  Date: 2015-10-21
 *  
 *  Edited:  Dan Kimball
 *  Date: 2015-10-26
 *  Comment: Added support for arbritration of the serial bus when connected as a gateway.
 *  the Cloud supplies an ACK of 06 when packets are successfully passed. this ACK is on the same
 *  RXD pin that the gateway's arduino needs to use to write commands.
 *  
 *  This code was changed to monitor the Serial port and to wait for the 06 ACK to occure before
 *  injecting any command to the gateway.  not waiting for an ACK results in the arduino sometimes 
 *  overwriting the ACK. preventing the cloud connection from initializing properly and perhaps other consequences
 *  such as loosing packets from reaching the cloud.
 *  
 *  Hardware Configuration:
 *  -----------------------
 *  TC-Backend USB to Arduino Due Serial0 (TX0 & RX0)
 *  RFMod TXD to Arduino Due RX1
 *  RFMod RXD to Arduino Due TX1
 *  
 *  +-------+-------------------------+------+------------------------------------------------------------------+
 *  | Byte# | Field name              | Size | Description                                                      |
 *  +-------+-------------------------+------+------------------------------------------------------------------+
 *  |   1   | Start Character         |   1  | Total number of bytes in the message, including start character  |
 *  +-------+-------------------------+------+------------------------------------------------------------------+
 *  |   2   | System ID               |   4  | System wide ID, must be identical for all devices in a system    |
 *  +-------+-------------------------+------+------------------------------------------------------------------+
 *  |   6   | Origin ID               |   4  | Total number of bytes in the message, including start character  |
 *  +-------+-------------------------+------+------------------------------------------------------------------+
 *  |  10   | Origin RSSI             |   1  | RSSI from first receiver to originating node                     |
 *  +-------+-------------------------+------+------------------------------------------------------------------+
 *  |  11   | Origin Network Level    |   1  | 'Hop' level, number of vertical hops to reach Gateway            |
 *  +-------+-------------------------+------+------------------------------------------------------------------+
 *  |  12   | Hop Counter             |   1  | Number of actual hops from Router to Gateway                     |
 *  +-------+-------------------------+------+------------------------------------------------------------------+
 *  |  13   | Message Counter         |   2  | Unique number maintained by originating node                     |
 *  +-------+-------------------------+------+------------------------------------------------------------------+
 *  |  15   | Latency Counter         |   2  | Time in 10 ms resolution from message creation to delivery       |
 *  +-------+-------------------------+------+------------------------------------------------------------------+
 *  |  17   | Packet Type             |   1  | Event 2 (0x02) or Serial data in 16 (0x10)                       |
 *  +-------+-------------------------+------+------------------------------------------------------------------+
 *   
 *  Byte index # 5,6,7,8 are Originator UID (because loop counter, i, byte index starts at 0)
 *  
 */

byte received_bytes_from_rfmod;   // use to buffer bytes from the RF Module on Serial
byte received_bytes_from_cloud;   // use to buffer bytes from the tiny connect code (coming from the cloud) on Serial1

void setup() {
  pinMode(13, OUTPUT);  // LED 13 for program running indicator
  pinMode(12, OUTPUT);  // LED 12 for ACK indicator
  Serial.begin(19200);  // Open the serial port for communication with the Tiny Connect Software
  // Open the serial port number 2 (Pin19) for reading the acknowledge that comes back from the cloud (tiny connect)
  // we should use this serial port also for passing data to the cloud when we move the tinyconnect code into our arduino
  // this will allow our arduino that is connected to the gateway to send the data going to the cloud and acknowledge the cloud ACK.
  // we should be passing all the cloud communication through this second serial port so that the gateway node is arbitrated to the cloud
  // through our arduino.  we will need to pass on any data we recieve to the other serial port.   At the moment however we are
  // still connecting to the RF module through the RadioCraft Dev Board's USB connection  (later well connect through the Arduino)
  Serial1.begin(19200); // Open the serial port for communication with the RF module
}

int i = 0;   // Used to count the bytes as they are read from the serial port
int packetsize=0;    // used to hold the size of the packet
byte addr[4];      // 4 bytes to hold the unique ID
byte ack=6;         // a constant for comparing the bytes we read from the cloud to find the ACK ( 06)
boolean getpathdata=false;    // this is set when we need to send a get path data command... (ie after the cloud gets sent a data packet)
boolean gotNewRFbyte=false;   // This is set when we get a new byte from RF Module
boolean gotNewCloudByte=false;  // This is set when we get a new byte from Cloud

void loop() {
  if (Serial.available()>0){
    received_bytes_from_cloud = Serial.read();   // Get the byte from the cloud  (the get nid command and acks)from tinyconnect Output
    gotNewCloudByte=true;
    Serial1.write(received_bytes_from_cloud);    // Send the byte to the RF Module (the RXD of the RF module recieves the NID and ACK)
  }
  if (Serial1.available()>0){
    received_bytes_from_rfmod = Serial1.read();   // Read the byte from the RF Module (data bytes being sent to the cloud from the RFMod TXD)
    gotNewRFbyte=true;
    Serial.write(received_bytes_from_rfmod);      // Send the byte to the Cloud (data bytes being sent to tinyconnect Input)
  }
  
  // If there is commands or ACK from the cloud on its way to the RFMod
  if (received_bytes_from_cloud == ack){    // If we found and ACK and its time to send getpath command
    digitalWrite(12,HIGH);                  // Set THE LED on 12 on TO SHOW that we got an ACK & are ready to send command
    if (getpathdata){
      // Send the GET PACKET PATH command [10 0 0 0 10 1 3 22 0 0] to the RFMod connected through Serial1
      Serial1.write((byte)0x0A);            // is the command packet length in bytes
      // Originator UID
      Serial1.write((byte)addr[0]);         // is where we store the 4 byte UID
      Serial1.write((byte)addr[1]);
      Serial1.write((byte)addr[2]);
      Serial1.write((byte)addr[3]);
      //  
      Serial1.write((byte)0x01);            // arbitrary
      Serial1.write((byte)0x03);            // this is to indicate a command
      Serial1.write((byte)0x16);            // the get packet path command
      Serial1.write((byte)0x00);            // filler
      Serial1.write((byte)0x00);            // filler
          
      getpathdata = false;                  // signal that we sent get path command.
    }
  }
  digitalWrite(12,LOW);
  gotNewCloudByte = false;
  
  // Then go handle at the bytes coming from the RFMod on their way to the cloud
  if (gotNewRFbyte){
    if (i==0){                                    // If the count is at 0 then were getting a new packet
        packetsize = received_bytes_from_rfmod;   // so set the packet size var with the first byte
        digitalWrite(13, HIGH);                   // and Turn THE LED on TO SHOW the outside world that were getting a new packet
    }
    if (i==5){                                    // If the count is at 5 then were getting the UID for the next 4 bytes
      addr[0] = received_bytes_from_rfmod;
    }
    if (i==6){     
      addr[1] = received_bytes_from_rfmod;
    }
    if (i==7){
      addr[2] = received_bytes_from_rfmod;
    }
    if (i==8){
      addr[3] = received_bytes_from_rfmod;
    }
    if (i==16){                                   // If the count is at 16 then were getting the packet type
      if (received_bytes_from_rfmod==16){         // If the value is 16 then its a serial packet
        getpathdata = true;                       //  so set up to send the get path command. 
        // We're taking a new action on the 16th byte because that's the packet type byte
        // and if the type value is Serial Data =(16) we want to get the path the data took by setting up to send a getpathcommand 
        // but only after we get an ACK so we don't overwrite the ACK handshaking.
      }
    }    
    i++;                                          // Count another byte as we read them
    if (i>0){
      if (i==packetsize){                         // Reset the byte counter when we reach the end of the packet
        i=0;
        digitalWrite(13, LOW);                    // Turn the LED off when were done handling the packet
      }
    }
    gotNewRFbyte = false;                         // Set new RFMod byte flag to false after finished using it
  }
} 

