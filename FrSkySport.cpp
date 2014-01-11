#include "Arduino.h"
#include "Serial.h"
#include "config.h"
#include "def.h"
#include "types.h"
#include "MultiWii.h"
#include "FrSkySport.h"
#include "Sensors.h"

/*
 FrSky Smart Port (SPort) support for MultiWii.

 For this to work you'll need to add the following to config.h:

    #define FRSKY_SPORT
    #define FRSKY_SPORT_SERIAL 3 // Use the correct serial port

 Also set your serial speed of the chosen serial port to 57600:

    #define SERIAL3_COM_SPEED 57600 // FrSky Smart Port (SPORT) speed

 Next you'll need to add this at the top of MultiWii.cpp:
 
    #include "FrSkySport.h"
    
 ...and just above the call to computeIMU() (line ~1130) add this line:
 
    FrSkySport_busCheck();
  
 It should look like this:
 
    FrSkySport_busCheck();
    computeIMU();

*/

//#if defined(FRSKY_SPORT)

short FrSkySport_crc;


// The main function that checks the Smart Port bus and responds at appropriate times
bool FrSkySport_busCheck() {
  static uint8_t lastRx = 0;
  uint8_t c = SerialAvailable(FRSKY_SPORT_SERIAL);
  while (c--) {
    int rx = SerialRead(FRSKY_SPORT_SERIAL);
    if (lastRx == 0x7e) {
      // NOTE:  Device IDs are entirely arbitrary for the most part.
      #ifdef VBAT
      if (rx == FRSKY_SPORT_DEVICE_4) { // A2 Voltage/VBAT
        lastRx = 0;
        FrSkySport_sendA2voltage();
      }
      #endif
      if (rx == FRSKY_SPORT_DEVICE_6) { // Barometer/Altitude
        lastRx = 0;
        FrSkySport_sendAltitude();
      }
      if (rx == FRSKY_SPORT_DEVICE_7) { // T1 (Temperature)
        lastRx = 0;
        uint16_t adjustedTemp = (baroTemperature - 273.15)/100; // No idea if this is correct but it works on my AIOP to get degrees C
        FrSkySport_sendTemperature(1, adjustedTemp);
      }
      #ifdef VARIOMETER
      if (rx == FRSKY_SPORT_DEVICE_12) { // Vario Speed (Vspd in OpenTx)
        lastRx = 0;
        FrSkySport_sendVarioSpeed();
      }
      #endif
      #ifdef GPS
      if (rx == FRSKY_SPORT_DEVICE_14) { // GPS speed
        lastRx = 0;
        FrSkySport_sendGPSSpeed();
      }
      if (rx == FRSKY_SPORT_DEVICE_15) { // GPS Altitude
        lastRx = 0;
        FrSkySport_sendGPSAltitude();
      }
      if (rx == FRSKY_SPORT_DEVICE_16) { // GPS Altitude
        lastRx = 0;
        FrSkySport_sendGPSLongLat();
      }
      #endif
      // MultiWii doesn't seem to have the ability to read RPMs but telemetry support is here:
//      if (rx == FRSKY_SPORT_DEVICE_17) { ]
//        lastRx = 0;
//        FrSkySport_sendRPM();
//      }
      // If MultiWii ever supports tracking of fuel just uncomment we're ready:
//      if (rx == FRSKY_SPORT_DEVICE_10) { // Fuel (why not?)
//        lastRx = 0;
//        FrSkySport_sendFuel();
//      }
    }
    lastRx = rx;
  }
  return false;
}

void FrSkySport_sendByte(uint8_t byte) {
  // CRC update
  FrSkySport_crc += byte; //0-1FF
  FrSkySport_crc += FrSkySport_crc >> 8; //0-100
  FrSkySport_crc &= 0x00ff;
  FrSkySport_crc += FrSkySport_crc >> 8; //0-0FF
  FrSkySport_crc &= 0x00ff;

  if ( (byte == 0x7E) || (byte == 0x7D) ) {
      SerialWrite(FRSKY_SPORT_SERIAL, 0x7D);
      byte &= ~0x20;
  }
  SerialWrite(FRSKY_SPORT_SERIAL, byte);
}

void FrSkySport_sendCrc() {
  FrSkySport_sendByte(0xFF - FrSkySport_crc);
}

void FrSkySport_sendValue(uint16_t id, uint32_t value) {
  FrSkySport_crc = 0; // Reset CRC
  FrSkySport_sendByte(0x10); // DATA_FRAME
  uint8_t *bytes = (uint8_t*)&id;
  FrSkySport_sendByte(bytes[0]);
  FrSkySport_sendByte(bytes[1]);
  bytes = (uint8_t*)&value;
  FrSkySport_sendByte(bytes[0]);
  FrSkySport_sendByte(bytes[1]);
  FrSkySport_sendByte(bytes[2]);
  FrSkySport_sendByte(bytes[3]);
  FrSkySport_sendCrc();
}

void FrSkySport_sendA2voltage() {
  // A2 voltage is represented by a value in the range 0-255.
  // So if you set the max for A2 in Companion9x to be 12.4 each step will increase the displayed voltage by ~0.0486
  // NOTE: VBAT gets stored as an integer.  16 would be 1.6V, 124 is 12.4V, etc etc
  uint32_t opentx_val = (255.0 * (float)(analog.vbat/(float)FRSKY_SPORT_A2_MAX));
  FrSkySport_sendValue(FRSKY_SPORT_ADC2_ID, (opentx_val));
}

void FrSkySport_sendAltitude() {
// Since OpenTx performs its own difference-based calculations for altitude this should be all that's necessary:
  FrSkySport_sendValue(FRSKY_SPORT_ALT_ID, ((int32_t)alt.EstAlt)); // TODO: Validate this
}

void FrSkySport_sendTemperature(uint8_t t_id, uint16_t temperature) { // id will be 1 or 2 for T1 and T2
  // TODO: Figure this out.  It is so wrong.
  int32_t convertedTemp = temperature - 31;
  debug[2] = convertedTemp;
  // NOTE: Values sent and what they show up as on the Taranis: -10 == 14, -20 == -4, -15 == 5, -16 == 3, -17 == 1, -18 == 0, 16 == 61, 8 = 46, 6 = 43, 4 == 39, 2 == 35, 1 == 34, 0 == 32, -1 == 30
  // I have no idea what kind of wacky curve that is!  If anyone can tell me how to translate the barometer temperature to Taranisese I'd appreciate it!
  if (t_id == 1) {
    FrSkySport_sendValue(FRSKY_SPORT_T1_ID, (convertedTemp));
  } else if (t_id == 2) {
    FrSkySport_sendValue(FRSKY_SPORT_T2_ID, (convertedTemp));
  }
}

void FrSkySport_sendFuel() {
  // No idea how I'd get this info...  Simulate it for now with the number 42
  FrSkySport_sendValue(FRSKY_SPORT_FUEL_ID, ((uint32_t)42)); // This value shows up on the Taranis exactly as-is.  Why can't the other values work like this?!?
}

void FrSkySport_sendGPSAltitude() {
  debug[0] = GPS_altitude;
  FrSkySport_sendValue(FRSKY_SPORT_GPS_ALT_ID, ((uint32_t)GPS_altitude)); // Shows up as "GAlt" in OpenTx
}

// I have no idea how to construct the combined longetude+latitude data (even after looking at the OpenTx code).  Help?
void FrSkySport_sendGPSLongLat() {
  debug[1] = GPS_coord[LAT];
  debug[2] = GPS_coord[LON];
  //FrSkySport_sendValue(FRSKY_SPORT_GPS_LONG_LATI_ID, ((uint32_t)GPS_altitude)); // Shows up as "GAlt" in OpenTx
}

void FrSkySport_sendGPSSpeed() {
  FrSkySport_sendValue(FRSKY_SPORT_GPS_SPEED_ID, ((uint32_t)GPS_speed)); // Shows up as "Spd" in OpenTx
}

void FrSkySport_sendVarioSpeed() {
  FrSkySport_sendValue(FRSKY_SPORT_VARIO_ID, ((uint32_t)alt.vario)); // NOTE: 4000 here means 40.00 in OpenTx
}

void FrSkySport_sendRPM() {
  // Value will be:  RPM == value * 0.25
  // Use simulated value for now since I don't know how to get that info from MultiWii
  FrSkySport_sendValue(FRSKY_SPORT_RPM_ID, ((int32_t)45000)); // 100 == 25 RPM; 200 == 50, 45000 == 11250
}


//#endif // defined(FRSKY_SPORT)

