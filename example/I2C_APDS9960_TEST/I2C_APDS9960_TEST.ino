// Steve Quinn 03/03/18
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
// Compiled using Arduino 1.6.9, Written Arduino Due
// Used to test APDS9960 interface over I2C and the APDS9960_NonBlocking Library.
// Note : Reset control is not included.
//


#include <Wire.h>
#include <APDS9960_NonBlocking.h>

#define DEBUG_GENERAL      // Undefine this for general debug information via the serial port. 
#define GESTURE_SENSOR_I2C_ADDRESS 0x3F

boolean bGestureAvailableFlag = false;
uint8_t uiGestureValue = APDS9960_GVAL_NONE;

APDS9960_NonBlocking gestureSensor(GESTURE_SENSOR_I2C_ADDRESS);

void setup (void)
{
  #ifdef DEBUG_GENERAL
  Serial.begin(115200);           // start serial for output
  Serial.println("Serial port Active");
  #endif  
  bGestureAvailableFlag = false;
  uiGestureValue = APDS9960_GVAL_NONE;
  
  Wire.begin();
  if (gestureSensor.init()) { ;
      #ifdef DEBUG_GENERAL
      Serial.println("APDS9960 initialised");
      #endif  
  } else { ;
      #ifdef DEBUG_GENERAL
      Serial.println("APDS9960 not responding");
      #endif  
  }
  delay(1000);
}

  

void readAPDS9960(boolean *bGestureAvailableFlag, uint8_t *uiGestureValue)
{
  *bGestureAvailableFlag = false;
  if (gestureSensor.gestureAvailable()) {
    *uiGestureValue = gestureSensor.read();
    gestureSensor.gestureClear();
    *bGestureAvailableFlag = true;
  }
  //delay(1000);
}


void loop (void)
{
  char cArray[30];

  readAPDS9960(&bGestureAvailableFlag, &uiGestureValue);
  if (bGestureAvailableFlag) {
    bGestureAvailableFlag = false;

    switch(uiGestureValue) {
      case APDS9960_GVAL_NONE :
        sprintf(cArray,"None");
        break;
      case APDS9960_GVAL_ERROR :
        sprintf(cArray,"Error");
        break;
      case APDS9960_GVAL_UP :
        sprintf(cArray,"Up");
        break;
      case APDS9960_GVAL_DOWN :
        sprintf(cArray,"Down");
        break;
      case APDS9960_GVAL_LEFT :
        sprintf(cArray,"Left");
        break;
      case APDS9960_GVAL_RIGHT :
        sprintf(cArray,"Right");
        break;
      case APDS9960_GVAL_NEAR :
        sprintf(cArray,"Near");
        break;
      case APDS9960_GVAL_FAR :
        sprintf(cArray,"Far");
        break;
    }
    #ifdef DEBUG_GENERAL
    Serial.println(cArray);
    #endif  
  }
}
