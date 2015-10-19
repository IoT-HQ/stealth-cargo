// Cargo Data Simulator
// Description: Send GPS position data and other relevant sensor ("dummy") data to simulate real cargo data in motion
// Author: Faizal Mansor
// Date: 2015-10-07

#include <Adafruit_GPS.h>

// You should make the following connections with the Due and GPS module:
// GPS power pin to Arduino Due 3.3V output.
// GPS ground pin to Arduino Due ground.
// For hardware serial 1 (recommended):
//   GPS TX to Arduino Due Serial1 RX pin 19
//   GPS RX to Arduino Due Serial1 TX pin 18
#define mySerial Serial1

Adafruit_GPS GPS(&mySerial);


// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences. 
#define GPSECHO  true

// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy

//***********************************************
// convert NMEA GPS Format to Std decimal Format
//***********************************************
int degLatInt, degLongInt;
float minLatFloat, minLongFloat, decLat, decLong; //googleMapsLat, googleMapsLong;
char* lat_str = new char[10];
char* long_str = new char[10];
char degLatString[5], degLongString[5];

char *dtostrf (double val, signed char width, unsigned char prec, char *sout) {
  char fmt[20];
  sprintf(fmt, "%%%d.%df", width, prec);
  sprintf(sout, fmt, val);
  return sout;
}

void setup() {
  pinMode(13, OUTPUT);
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(19200);

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  mySerial.begin(9600);
  
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  // the nice thing about this code is you can have a timer0 interrupt go off
  // every 1 millisecond, and read data from the GPS for you. that makes the
  // loop code a heck of a lot easier!

#ifdef __arm__
  usingInterrupt = false;  //NOTE - we don't want to use interrupts on the Due
#else
  useInterrupt(true);
#endif

  delay(1000);
  // Ask for firmware version
  // mySerial.println(PMTK_Q_RELEASE);
}

#ifdef __AVR__
// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) {
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
#ifdef UDR0
  if (GPSECHO)
    if (c) UDR0 = c;  
    // writing direct to UDR0 is much much faster than Serial.print 
    // but only one character can be written at a time. 
#endif

}
void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}
#endif //#ifdef__AVR__

uint32_t timer = millis();
const String TRANSPORTER_ID = "R0001";
const String TRIP_ID = "P0003";

void loop() {
  // in case you are not using the interrupt above, you'll
  // need to 'hand query' the GPS, not suggested :(
  if (! usingInterrupt) {
    // read data from the GPS in the 'main loop'
    char c = GPS.read();
    // if you want to debug, this is a good time to do it!
    //if (GPSECHO)
      //if (c) Serial.print(c);
  }
  
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences! 
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
  
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }

  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 4000) { 
    timer = millis(); // reset the timer

    // Data to be formatted and send on the serial bus
    //
    // uid = 0.0.0.10
    // record_type = 2 (cargo)
    // GPS.year + GPS.month + GPS.day + GPS.hour + GPS.minute + GPS.seconds (timestamp)
    // GPS.latitude + GPS.lat 
    // GPS.longitude + GPS.lon
    // GPS.angle
    // GPS.speed
    // GPS.altitude
    // transporter_id
    // trip_id
    // weight
    // temperature
    // pressure
    // moisture

    // Convert NMEA Latitude to String
    dtostrf(GPS.latitude, 7, 4, lat_str); //convert GPS.latitude from float to String. 7 - means 8 characters long ; 4 - means 4 decimal point 

    // Get Degree substring from latitude String
    strncpy(degLatString, lat_str, 1); // Get Degree from Full Latitude String. Example 311.7799 - Get 3

    // Get Minutes substring from latitude String
    char* minLatString = lat_str + 1; // Move pointer up 1 position. Example 311.7799 will move to 11.7799

    // Convert Deg String to int
    degLatInt = atoi(degLatString);

    // Convert Min String to float
    minLatFloat = atof(minLatString);

    //Convert NMEA latitude to Google Maps latitude
    decLat = degLatInt + (minLatFloat / 60); 
    // Serial.print("Google Map Latitude: ");
    // Serial.println(googleMapsLat, 4);

    // Convert NMEA Longitude to String
    dtostrf(GPS.longitude, 8, 4, long_str); //convert GPS.latitude from float to String. 8 - means 9 characters long ; 4 - means 4 decimal point 

    // Get Degree substring from latitude String
    strncpy(degLongString, long_str, 3); // Get Degree from Full Latitude String. Example 10141.91699 - Get 141

    // Get Minutes substring from latitude String
    char* minLongString = long_str + 3; // Move pointer up 3 position. Example 10141.9169 will move to 41.7799

    // Convert Deg String to int
    degLongInt = atoi(degLongString);

    // COnvert Min String to float
    minLongFloat = atof(minLongString);

    //Convert NMEA latitude to Google Maps latitude
    decLong = degLongInt + (minLongFloat / 60); 
    // Serial.print("Google Map Longitude: ");
    // Serial.println(googleMapsLong, 4);


    
    String uid = "0.0.0.13";
    String rty = ";2";
    String tsp = ';' + String(GPS.year) + '-' + String(GPS.month) + '-' + GPS.day + 'T' + GPS.hour + ':' + GPS.minute + ':' + GPS.seconds;
    String lat = ';' + String(decLat, 4) + String(GPS.lat);
    String lng = ';' + String(decLong, 4) + String(GPS.lon);
    String crs = ';' + String(GPS.angle);
    String spd = ';' + String(GPS.speed);
    String alt = ';' + String(GPS.altitude);
    
    // write the payload data to the serial line for transmission
    Serial.println(uid + rty + tsp + lat + lng + crs + spd + alt + TRANSPORTER_ID + TRIP_ID);
    
    digitalWrite(13, HIGH);
    delay(500);
    digitalWrite(13, LOW);
    delay(500);
  }

}
