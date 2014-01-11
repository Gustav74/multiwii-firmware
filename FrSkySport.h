/*
 * FrSkySPort.h
 *
 *  Created on: 02 Jan 2014
 *      Author: riskable
 */

#ifndef FRSKY_SPORT_H_
#define FRSKY_SPORT_H_

#define FrSkyRxPin = 9
#define FrSkyTxPin = 10


void frsky_sport();
bool FrSkySport_busCheck();
void FrSkySport_sendByte(uint8_t byte);
void FrSkySport_sendCrc();
void FrSkySport_sendValue(uint16_t id, uint32_t value);
void FrSkySport_sendA2voltage();
void FrSkySport_sendAltitude();
void FrSkySport_sendTemperature(uint8_t t_id, uint16_t temperature);
void FrSkySport_sendFuel();
void FrSkySport_sendGPSLongLat();
void FrSkySport_sendGPSAltitude();
void FrSkySport_sendGPSSpeed();
void FrSkySport_sendVarioSpeed();
void FrSkySport_sendRPM();


// FrSky data IDs (2 bytes)
#define FRSKY_SPORT_RSSI_ID           0xf101
#define FRSKY_SPORT_ADC1_ID           0xf102 // A1
#define FRSKY_SPORT_ADC2_ID           0xf103 // A2
#define FRSKY_SPORT_BATT_ID           0xf104
#define FRSKY_SPORT_SWR_ID            0xf105
#define FRSKY_SPORT_T1_ID             0x0400
#define FRSKY_SPORT_T2_ID             0x0410
#define FRSKY_SPORT_RPM_ID            0x0500
#define FRSKY_SPORT_FUEL_ID           0x0600
#define FRSKY_SPORT_ALT_ID            0x0100
#define FRSKY_SPORT_VARIO_ID          0x0110
#define FRSKY_SPORT_ACCX_ID           0x0700
#define FRSKY_SPORT_ACCY_ID           0x0710
#define FRSKY_SPORT_ACCZ_ID           0x0720
#define FRSKY_SPORT_CURR_ID           0x0200
#define FRSKY_SPORT_VFAS_ID           0x0210
#define FRSKY_SPORT_CELLS_ID          0x0300
#define FRSKY_SPORT_GPS_LONG_LATI_ID  0x0800
#define FRSKY_SPORT_GPS_ALT_ID        0x0820
#define FRSKY_SPORT_GPS_SPEED_ID      0x0830
#define FRSKY_SPORT_GPS_COURS_ID      0x0840
#define FRSKY_SPORT_GPS_TIME_DATE_ID  0x0850

// FrSky sensor IDs (this also happens to be the order in which they're broadcast from an X8R)
// NOTE: As FrSky puts out more sensors let's try to add comments here indicating which is which
#define FRSKY_SPORT_DEVICE_1    0xa1
#define FRSKY_SPORT_DEVICE_2    0x22
#define FRSKY_SPORT_DEVICE_3    0x83
#define FRSKY_SPORT_DEVICE_4    0xe4
#define FRSKY_SPORT_DEVICE_5    0x45
#define FRSKY_SPORT_DEVICE_6    0xc6
#define FRSKY_SPORT_DEVICE_7    0x67
#define FRSKY_SPORT_DEVICE_8    0x48
#define FRSKY_SPORT_DEVICE_9    0xe9
#define FRSKY_SPORT_DEVICE_10   0x6a // TEMPORARY: Used for GPS info
#define FRSKY_SPORT_DEVICE_11   0xcb
#define FRSKY_SPORT_DEVICE_12   0xac
#define FRSKY_SPORT_DEVICE_13   0xd
#define FRSKY_SPORT_DEVICE_14   0x8e
#define FRSKY_SPORT_DEVICE_15   0x2f
#define FRSKY_SPORT_DEVICE_16   0xd0
#define FRSKY_SPORT_DEVICE_17   0x71 // TEMPORARY: Used for Barometer/Altitude info
#define FRSKY_SPORT_DEVICE_18   0xf2
#define FRSKY_SPORT_DEVICE_19   0x53
#define FRSKY_SPORT_DEVICE_20   0x34
#define FRSKY_SPORT_DEVICE_21   0x95
#define FRSKY_SPORT_DEVICE_22   0x16
#define FRSKY_SPORT_DEVICE_23   0xb7
#define FRSKY_SPORT_DEVICE_24   0x98 // Voltage sensor/VBAT
#define FRSKY_SPORT_DEVICE_25   0x39
#define FRSKY_SPORT_DEVICE_26   0xba
#define FRSKY_SPORT_DEVICE_27   0x1b


#endif /* FRSKY_SPORT_H_ */




