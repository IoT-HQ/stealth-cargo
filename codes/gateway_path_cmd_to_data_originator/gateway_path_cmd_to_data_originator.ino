/*  
 *  Gateway Path Command to Data Originator
 *  =======================================
 *  Description: Send GET PACKET PATH command to Serial data sender UID
 *  Author: Faizal Mansor
 *  Date: 2015-10-21
 * 
 *  Hardware Configuration:
 *  -----------------------
 *  Module TXD to Arduino Due RX0
 *  Module RXD to Arduino Due TX0
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

byte received_bytes;

void setup() {
  pinMode(13, OUTPUT);  // LED 13 for program running indicator
  // Open the serial port for communication with the Gateway module
  Serial.begin(19200);  
}

int i = 0;
int bytesize=0;
byte addr[4];

void loop() {
  
  if (Serial.available() > 0){
    
    received_bytes = Serial.read();

      if (i==0){
        bytesize = received_bytes;
//        Serial.println(bytesize);
//        Serial.println("------------------------");
      }
      if (i==5){
        addr[0] = received_bytes;
//        Serial.println(addr[0]);
      }
      if (i==6){     
        addr[1] = received_bytes;
//        Serial.println(addr[1]);
      }
      if (i==7){
        addr[2] = received_bytes;
//        Serial.println(addr[2]);
      }
      if (i==8){
        addr[3] = received_bytes;
//        Serial.println(addr[3]);
//        Serial.println("------------------------");
      }
      if (i==16){
        //Serial.println(received_bytes);
        //Serial.println("========================");
        // Check for Serial data (16 or 0x10)
        if (received_bytes==16){
          
          // Debug - START ###
//          Serial.println(String(addr[0]) + " : " + String(addr[1]) + " : " + String(addr[2]) + " : " + String(addr[3]));
//          Serial.println("Sending cmd...");
//          Serial.println("========================");
          // Debug - END ###
          
          // Send the GET PACKET PATH command [10 0 0 0 10 1 3 22 0 0]
          Serial.write((byte)0x0A);
          // Originator UID
          Serial.write((byte)addr[0]);
          Serial.write((byte)addr[1]);
          Serial.write((byte)addr[2]);
          Serial.write((byte)addr[3]);
          //
          Serial.write((byte)0x01);
          Serial.write((byte)0x03);
          Serial.write((byte)0x16);
          Serial.write((byte)0x00);
          Serial.write((byte)0x00);
        }
      }    
    i++;
    if (i>0){
      if (i==bytesize)
        i=0;
    }
  }
  
  //delay(1000);
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  delay(500);
}
