// Module TX = Serial1 RX
// Module RXD = Serial1 TX
#define mySerial Serial1

int pinConf = 7;
int pinInput = 8;
byte* inbuf;

void setup() {
  // put your setup code here, to run once:
  pinMode(pinConf, OUTPUT); // Connected to P8 conn - no.3 on the TM demo board
  
  pinMode(pinInput, INPUT);
  digitalWrite(pinConf, HIGH);
  
  Serial.begin(19200);
  Serial.println("Hello World!");
  delay(10000);

  mySerial.begin(19200);
  
  // Start test
  mySerial.write((byte)0x58); // exit command 'X'
  digitalWrite(pinConf, LOW); // set module config pin to LOW to enter config mode 
  // Enter config mode
  Serial.println(mySerial.read());
  
  delay(5000);

//  mySerial.write((byte)0x53); // 'S' command for reading RSSI
//  mySerial.readBytes(inbuf, sizeof(inbuf));
//  for (int i = 0; i < sizeof(inbuf); i++) { 
//    Serial.println(inbuf[i]);
//  }
//  delay(5000);
  
  // Exit config mode
  digitalWrite(pinConf, HIGH);
  mySerial.write((byte)0x58); // exit command 'X'
  
//    if (char(inbuf[0]) == '>'){
//      Serial.write((byte)0x53); // 'S' command for reading RSSI
//      Serial.readBytes(inbuf, 1);
//      if (sizeof(inbuf)==1){
//        digitalWrite(pinConf, HIGH);
//        Serial.write((byte)0x58); // exit command 'X'
//        Serial.println(inbuf[0]);
//      }
//    }

}

void loop() {
  // put your main code here, to run repeatedly:
//  if (digitalRead(pinInput)==HIGH)
//    Serial.println("HIGH");
//  else
//    Serial.println("LOW");
//  
//  delay(3000);
}
