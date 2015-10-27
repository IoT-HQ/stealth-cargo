// Serial Command to TinyMesh Module (AUTO)
// Description: Enter and send command during CONFIG mode AUTOMATICALLY using an output pin
// Author: Faizal Mansor
// Date: 2015-10-22

// ToDO: Add OUTPUT pin and connect to CONFIG header (P8 male #3???)


// Board connections:
// ------------------
// Module TX = Arduino Due RX1
// Module RXD (with pull up resistor) = Arduino Due TX1
//
// TEST PROCEDURE 
// ==============
// 1. Open serial monitor & press the CONFIG button on connected module board
// 2. You would receive the prompt (i.e. 62 which is equivalent to '>')
// 3. Send any command to the board (i.e. char 'S' for get RF signal strength) through the serial monitor input box
// 4. You would receive a byte with RF signal strength value (i.e. 200) & the prompt char
// 5. To exit type in serial monitor input box the exit char 'X'
// 6. You will receive the hex dump of the board (i.e. start byte with decimal 35)

#define mySerial Serial1  // defined for exclusive use of TM board's TXD & RXD

void setup() {
  Serial.begin(19200);    // Start serial monitor
  mySerial.begin(19200);  // Remember to start the TM serial connection!!!
  Serial.println("Start");
  delay(5000);
}

char rx_byte, rx1_byte;

void loop() {
  if (Serial.available() > 0) {    // is a character available?
    rx_byte = Serial.read();       // get the character
    Serial.println(rx_byte);
    mySerial.write((byte)rx_byte);  // write to module serial RXD
    
  }
  if (mySerial.available() > 0){  // is a character received from module TXD
    rx1_byte = mySerial.read();
    Serial.println((byte)rx1_byte); // print to serial monitor
  }
}
