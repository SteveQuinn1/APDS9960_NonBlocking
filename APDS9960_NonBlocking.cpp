// Steve Quinn 04/03/18
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
//  To make this work you must make the following calls as ordered.
//
//
//  #include <Wire.h>
//  #include <APDS9960_NonBlocking.h>
//  #define YourI2CAddress 0xXX
//  APDS9960_NonBlocking gestureSensor(YourI2CAddress);
//
//  void setup(void) {
//    Wire.begin();
//    if (!gestureSensor.init())
//      Serial.println("APDS9960 not responding");
//  }
//
//  void loop(void) {
//    if (gestureSensor.gestureAvailable()) {
//		uint8_t x = gestureSensor.read();
//	    gestureSensor.gestureClear();
//      DoSomething with x
//	  }
//  }
//

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "APDS9960_NonBlocking.h"
#include <Wire.h>



#define B_SET_BIT(p,whichBit)    ((p) |=  (1    << (whichBit)))
#define B_CLEAR_BIT(p,whichBit)  ((p) &= ~((1)  << (whichBit)))
#if defined(ARDUINO_SAM_DUE)
#define _BV(whichBit)            (1             << (whichBit))
#endif

APDS9960_NonBlocking::APDS9960_NonBlocking(uint8_t address)
	: _state(0)
	, _bytes(0)
	, _APDS9960_GVAL(APDS9960_GVAL_NONE)
{
	_address = address;
}


bool APDS9960_NonBlocking::init(void)
{
	_bytes = Wire.requestFrom((uint8_t)_address, (uint8_t)1);
	_state = (uint8_t) Wire.read();
	Wire.beginTransmission((uint8_t)_address);
	B_CLEAR_BIT(_state,APDS9960_gestureClearPin);
	Wire.write(_state);
	//delayMicroseconds(500);
	B_SET_BIT(_state,APDS9960_gestureClearPin);
	Wire.write(_state);
	Wire.endTransmission();
	_APDS9960_GVAL = _state & APDS9960_GVAL_MASK;
	return (_bytes == 1);
}



uint8_t APDS9960_NonBlocking::read(void)
{
	Wire.requestFrom((uint8_t)_address, (uint8_t)1);
	_state = (uint8_t) Wire.read();
	_APDS9960_GVAL = _state & APDS9960_GVAL_MASK;
}


bool APDS9960_NonBlocking::gestureAvailable(void)
{
	Wire.requestFrom((uint8_t)_address, (uint8_t)1);
	_state = (uint8_t) Wire.read();
	return (bool)((_state & (1 << APDS9960_gestureAvilablePin)) == 0);
}


void APDS9960_NonBlocking::reset(boolean resetState)
{
	Wire.requestFrom((uint8_t)_address, (uint8_t)1);
	_state = (uint8_t) Wire.read();
	Wire.beginTransmission((uint8_t)_address);
	if (resetState == HIGH)
		B_SET_BIT(_state,APDS9960_resetADPS9960Pin);
	else
		B_CLEAR_BIT(_state,APDS9960_resetADPS9960Pin);
	Wire.write(_state);
	Wire.endTransmission();
	_APDS9960_GVAL = APDS9960_GVAL_NONE;
}


void APDS9960_NonBlocking::gestureClear(void)
{
	Wire.requestFrom((uint8_t)_address, (uint8_t)1);
	_state = (uint8_t) Wire.read();
	Wire.beginTransmission((uint8_t)_address);
	B_CLEAR_BIT(_state,APDS9960_gestureClearPin);
	Wire.write(_state);
	Wire.endTransmission();
	//delayMicroseconds(500);
	Wire.beginTransmission((uint8_t)_address);
	B_SET_BIT(_state,APDS9960_gestureClearPin);
	Wire.write(_state);
	Wire.endTransmission();
	_APDS9960_GVAL = APDS9960_GVAL_NONE;
}



