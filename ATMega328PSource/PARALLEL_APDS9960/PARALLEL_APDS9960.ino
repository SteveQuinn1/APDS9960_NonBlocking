// Steve Quinn 25/02/18
//
// Copyright 2018 Steve Quinn
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//
// Compiled using Arduino 1.6.9, Written for Arduino Uno (running on +3v3 supply rails).
//
// Interrupt driven Parallel interface and polled APDS9960 Interrupt line monitoring.
// See C:\Program Files (x86)\Arduino\hardware\tools\avr\avr\include\avr\iom328p.h
// for Assembler definitions for ATmega328P.
//

#include <Wire.h>
#include "SparkFun_APDS9960.h"


//#define DEBUG_GENERAL           // Undefine this for general debug information via the serial port. 
//#define DEBUG_APDS9960          // Undefine this for APDS9960 debug information via the serial port. Requires DEBUG_GENERAL.
//#define DEBUG_APDS9960_ADV      // Undefine this for APDS9960 debug information via the serial port. Requires DEBUG_GENERAL.
//#define DEBUG_GESTURE_CLEAR_INT // Undefine this for Interrupt 0 debug information via the serial port. Requires DEBUG_GENERAL.

#define GESTURE_CLEAR_INT0 2     // Digital pin 2,  Hardware IC pin 4,  The digital pin asciated with INT0
#define GESTURE_AVAILABLE  7     // Digital pin 7,  Hardware IC pin 13, PORTD Bit 7
#define APDS9960_INT       4     // Digital Pin 4,  Hardware IC Pin 6,  PORTD Bit 4
#define PORTBBit0          8     // Digital Pin 8,  Hardware IC Pin 14, PORTB Bit 0
#define PORTBBit1          9     // Digital Pin 9,  Hardware IC Pin 15, PORTB Bit 1
#define PORTBBit2          10    // Digital Pin 10, Hardware IC Pin 16, PORTB Bit 2
#define PORTBBit3          11    // Digital Pin 11, Hardware IC Pin 17, PORTB Bit 3

#ifdef DEBUG_GENERAL
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#endif

#define GVAL_NONE    ((byte)0x00)
#define GVAL_ERROR   ((byte)0x0f)
#define GVAL_UP      ((byte)0x01)
#define GVAL_DOWN    ((byte)0x02)
#define GVAL_LEFT    ((byte)0x03)
#define GVAL_RIGHT   ((byte)0x04)
#define GVAL_NEAR    ((byte)0x05)
#define GVAL_FAR     ((byte)0x06)

volatile byte cGestureValue = GVAL_NONE;
volatile bool bGestureAvailable = false;

// Global Variables
SparkFun_APDS9960 apds = SparkFun_APDS9960();



//##############################################
//###                                        ###
//###          Function Declarations         ###
//###                                        ###
//##############################################
void GESTURE_CLEAR(void);
void handleGesture(void);
#ifdef DEBUG_APDS9960_ADV  
void dumpAllGestureRegisters(void);
#endif


void setup() {
  #ifdef DEBUG_GENERAL
  Serial.begin(115200);           // start serial for output
  Serial.println(__FILENAME__);
  Serial.println("Serial port Active");
  #endif
  Wire.begin();   

  cGestureValue = GVAL_NONE;
  pinMode(APDS9960_INT,       INPUT);
  pinMode(GESTURE_CLEAR_INT0, INPUT_PULLUP);
  pinMode(GESTURE_AVAILABLE,  OUTPUT);
  pinMode(PORTBBit0,          OUTPUT);
  pinMode(PORTBBit1,          OUTPUT);
  pinMode(PORTBBit2,          OUTPUT);
  pinMode(PORTBBit3,          OUTPUT);

  bGestureAvailable = false;
  //digitalWrite(GESTURE_AVAILABLE,HIGH);
  // Set Gesture Available output high
  PORTD |= _BV(7);
  //digitalWrite(PORTBBit0,LOW);
  //digitalWrite(PORTBBit1,LOW);
  //digitalWrite(PORTBBit2,LOW);
  //digitalWrite(PORTBBit3,LOW);
  // clear port B bits 0...3
  PORTB &= 0xF0;
  
  // Initialize APDS-9960 (configure I2C and initial values)
  if ( apds.init() ) {
    #ifdef DEBUG_APDS9960
    Serial.println(F("APDS-9960 initialization complete"));
    #endif
    ;
    // Start running the APDS-9960 gesture sensor engine and enable the interrupt line
    if ( apds.enableGestureSensor(true) ) {
      #ifdef DEBUG_APDS9960
      Serial.println(F("Gesture sensor is now running"));
      #endif
      ;
    } else {
      #ifdef DEBUG_APDS9960
      Serial.println(F("Something went wrong during gesture sensor init!"));
      #endif
      ;
    }  
  } else {
    #ifdef DEBUG_APDS9960
    Serial.println(F("Something went wrong during APDS-9960 init!"));
    #endif
  }

  noInterrupts();  // disable all interrupts
  // Set Up External Interrupts
  pinMode(GESTURE_CLEAR_INT0, INPUT_PULLUP);
  attachInterrupt (digitalPinToInterrupt(GESTURE_CLEAR_INT0), GESTURE_CLEAR, FALLING);
  interrupts();    // enable all interrupts  
}



void loop() {
  if (!digitalRead(APDS9960_INT))
    handleGesture();

  #ifdef DEBUG_APDS9960_ADV  
    dumpAllGestureRegisters();
  #endif
}





void GESTURE_CLEAR(void)        // Triggered by falling edge on Int0, Physical Pin 4, Arduino Digital Pin 2 interrupt service routine
{
  // clear port B bits 0...3
  PORTB &= 0xF0;
  // Set Gesture Available output high
  PORTD |= _BV(7);
  bGestureAvailable = false;
  
  #ifdef DEBUG_GESTURE_CLEAR_INT
    Serial.println(F("."));
  #endif  
}





void handleGesture(void) {
  if ( apds.isGestureAvailable() ) {
    switch ( apds.readGesture() ) {
      case DIR_UP:
        cGestureValue = GVAL_UP;
        #ifdef DEBUG_APDS9960
        Serial.println(F("UP"));
        #endif
        break;
      case DIR_DOWN:
        cGestureValue = GVAL_DOWN;
        #ifdef DEBUG_APDS9960
        Serial.println(F("DOWN"));
        #endif
        break;
      case DIR_LEFT:
        cGestureValue = GVAL_LEFT;
        #ifdef DEBUG_APDS9960
        Serial.println(F("LEFT"));
        #endif
        break;
      case DIR_RIGHT:
        cGestureValue = GVAL_RIGHT;
        #ifdef DEBUG_APDS9960
        Serial.println(F("RIGHT"));
        #endif
        break;
      case DIR_NEAR:
        cGestureValue = GVAL_NEAR;
        #ifdef DEBUG_APDS9960
        Serial.println(F("NEAR"));
        #endif
        break;
      case DIR_FAR:
        cGestureValue = GVAL_FAR;
        #ifdef DEBUG_APDS9960
        Serial.println(F("FAR"));
        #endif
        break;
      default:
        cGestureValue = GVAL_NONE;
        return;
        #ifdef DEBUG_APDS9960
        Serial.println(F("NONE"));
        #endif
    }
    if (!bGestureAvailable) {
      // write port bits
      PORTB = ((PORTB & 0xf0) | (cGestureValue & 0x0f));
      bGestureAvailable = true;
      // Set Gesture Available output low
      PORTD &= ~_BV(7);
      #ifdef DEBUG_APDS9960
      char y[1000];
      sprintf(y,"PORTB 0x%02X, PORTD 0x%02X, bGestureAvailable %d, cGestureValue %d",PORTB, PORTD, bGestureAvailable, cGestureValue);
      Serial.println(y);
      #endif
    }

  }
}





#ifdef DEBUG_APDS9960_ADV  
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(b)  \
  (b & 0x80 ? '1' : '0'), \
  (b & 0x40 ? '1' : '0'), \
  (b & 0x20 ? '1' : '0'), \
  (b & 0x10 ? '1' : '0'), \
  (b & 0x08 ? '1' : '0'), \
  (b & 0x04 ? '1' : '0'), \
  (b & 0x02 ? '1' : '0'), \
  (b & 0x01 ? '1' : '0') 

void dumpAllGestureRegisters(void)
{
  uint8_t byteVal;
  char tmpStr[100];
//sprintf(tmpStr,"Leading text "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(byte));
//printf("m: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(m>>8), BYTE_TO_BINARY(m));

  if (apds.wireReadDataByte(APDS9960_ENABLE, byteVal)) {
    sprintf(tmpStr,"ENABLE <6 GEN> <0 PON> %c%c%c%c%c%c%c%c", BYTE_TO_BINARY(byteVal));
    Serial.println(tmpStr);
  } else {
    Serial.println(F("Couldn't read APDS9960_ENABLE"));
  }

  if (apds.wireReadDataByte(APDS9960_GPENTH,byteVal)) {
    sprintf(tmpStr,"GEXTH 0x%02X", byteVal);
    Serial.println(tmpStr);
  } else {
    Serial.println(F("Couldn't read APDS9960_GPENTH"));
  }

  if (apds.wireReadDataByte(APDS9960_GEXTH,byteVal)) {
    sprintf(tmpStr,"GPENTH 0x%02X", byteVal);
    Serial.println(tmpStr);
  } else {
    Serial.println(F("Couldn't read APDS9960_GEXTH"));
  }

  if (apds.wireReadDataByte(APDS9960_GCONF1,byteVal)) {
    sprintf(tmpStr,"GCONFIG1 <7,6 GFIFOTH> <5,2 GEXMSK> <1,0 GEXPERS> %c%c%c%c%c%c%c%c", BYTE_TO_BINARY(byteVal));
    Serial.println(tmpStr);
  } else {
    Serial.println(F("Couldn't read APDS9960_GCONFIG1"));
  }

  if (apds.wireReadDataByte(APDS9960_GCONF2,byteVal)) {
    sprintf(tmpStr,"GCONFIG2 <6,5 GGAIN> <4,3 GLDRIVE> <2,0 GWTIME> %c%c%c%c%c%c%c%c", BYTE_TO_BINARY(byteVal));
    Serial.println(tmpStr);
  } else {
    Serial.println(F("Couldn't read APDS9960_GCONFIG2"));
  }

  if (apds.wireReadDataByte(APDS9960_GOFFSET_U,byteVal)) {
    sprintf(tmpStr,"GOFFSET_U 0x%02X", byteVal);
    Serial.println(tmpStr);
  } else {
    Serial.println(F("Couldn't read APDS9960_GOFFSET_U"));
  }

  if (apds.wireReadDataByte(APDS9960_GOFFSET_D,byteVal)) {
    sprintf(tmpStr,"GOFFSET_D 0x%02X", byteVal);
    Serial.println(tmpStr);
  } else {
    Serial.println(F("Couldn't read APDS9960_GOFFSET_D"));
  }

  if (apds.wireReadDataByte(APDS9960_GOFFSET_L,byteVal)) {
    sprintf(tmpStr,"GOFFSET_L 0x%02X", byteVal);
    Serial.println(tmpStr);
  } else {
    Serial.println(F("Couldn't read APDS9960_GOFFSET_L"));
  }

  if (apds.wireReadDataByte(APDS9960_GOFFSET_R,byteVal)) {
    sprintf(tmpStr,"GOFFSET_R 0x%02X", byteVal);
    Serial.println(tmpStr);
  } else {
    Serial.println(F("Couldn't read APDS9960_GOFFSET_R"));
  }

  if (apds.wireReadDataByte(APDS9960_GPULSE,byteVal)) {
    sprintf(tmpStr,"GPULSE <7,6 GPLEN> <5,0 GPULSE> %c%c%c%c%c%c%c%c", BYTE_TO_BINARY(byteVal));
    Serial.println(tmpStr);
  } else {
    Serial.println(F("Couldn't read APDS9960_GPULSE"));
  }
  
  if (apds.wireReadDataByte(APDS9960_GCONF3,byteVal)) {
    sprintf(tmpStr,"GCONFIG3 <1,0 GDIMS> %c%c%c%c%c%c%c%c", BYTE_TO_BINARY(byteVal));
    Serial.println(tmpStr);
  } else {
    Serial.println(F("Couldn't read APDS9960_GCONFIG3"));
  }
  
  if (apds.wireReadDataByte(APDS9960_GCONF4,byteVal)) {
    sprintf(tmpStr,"GCONFIG4 <2 GFIFO_CLR> <1 GIEN> <0 GMODE> %c%c%c%c%c%c%c%c", BYTE_TO_BINARY(byteVal));
    Serial.println(tmpStr);
  } else {
    Serial.println(F("Couldn't read APDS9960_GCONFIG4"));
  }

  if (apds.wireReadDataByte(APDS9960_GFLVL,byteVal)) {
    sprintf(tmpStr,"GFLVL 0x%02X", byteVal);
    Serial.println(tmpStr);
  } else {
    Serial.println(F("Couldn't read APDS9960_GFLVL"));
  }

  if (apds.wireReadDataByte(APDS9960_GSTATUS,byteVal)) {
    sprintf(tmpStr,"GSTATUS <1 GFOV> <0 GVALID> %c%c%c%c%c%c%c%c", BYTE_TO_BINARY(byteVal));
    Serial.println(tmpStr);
  } else {
    Serial.println(F("Couldn't read APDS9960_GSTATUS"));
  }

  if (apds.wireReadDataByte(APDS9960_GFIFO_U,byteVal)) {
    sprintf(tmpStr,"GFIFO_U 0x%02X", byteVal);
    Serial.println(tmpStr);
  } else {
    Serial.println(F("Couldn't read APDS9960_GFIFO_U"));
  }
  
  if (apds.wireReadDataByte(APDS9960_GFIFO_D,byteVal)) {
    sprintf(tmpStr,"GFIFO_D 0x%02X", byteVal);
    Serial.println(tmpStr);
  } else {
    Serial.println(F("Couldn't read APDS9960_GFIFO_D"));
  }
  
  if (apds.wireReadDataByte(APDS9960_GFIFO_L,byteVal)) {
    sprintf(tmpStr,"GFIFO_L 0x%02X", byteVal);
    Serial.println(tmpStr);
  } else {
    Serial.println(F("Couldn't read APDS9960_GFIFO_L"));
  }
  
  if (apds.wireReadDataByte(APDS9960_GFIFO_R,byteVal)) {
    sprintf(tmpStr,"GFIFO_R 0x%02X", byteVal);
    Serial.println(tmpStr);
  } else {
    Serial.println(F("Couldn't read APDS9960_GFIFO_R"));
  }

  if (apds.wireReadDataByte(APDS9960_CONFIG1,byteVal)) {
    sprintf(tmpStr,"CONFIG1 <LOWPOW> 0x%02X", byteVal);
    Serial.println(tmpStr);
  } else {
    Serial.println(F("Couldn't read APDS9960_CONFIG1"));
  }


/*  
0x80 ENABLE <6 GEN> <0 PON>
0xA0 GPENTH
0xA1 GEXTH
0xA2 GCONFIG1 <7,6 GFIFOTH> <5,2 GEXMSK> <1,0 GEXPERS>
0xA3 GCONFIG2 <6,5 GGAIN> <4,3 GLDRIVE> <2,0 GWTIME>
0x93 STATUS <6 PGSAT>
0x90 CONFIG2 <5,4 LEDBOOST>
0xA4 GOFFSET_U
0xA5 GOFFSET_D
0xA7 GOFFSET_L
0xA9 GOFFSET_R
0xA6 GPULSE <7,6 GPLEN> <5,0 GPULSE>
0xAA GCONFIG3 <1,0 GDIMS>
0xAB GCONFIG4 <2 GFIFO_CLR> <1 GIEN> <0 GMODE>
0xAE GFLVL
0xAF GSTATUS <1 GFOV> <0 GVALID>
0xFC GFIFO_U
0xFD GFIFO_D
0xFE GFIFO_L
0xFF GFIFO_R
0x8D CONFIG1 <LOWPOW>
*/
} 
#endif




