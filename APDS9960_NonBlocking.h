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
//
//

#ifndef APDS9960_NonBlocking_h
#define APDS9960_NonBlocking_h

//#include <inttypes.h>



const uint8_t APDS9960_dataBit0Pin        = 0;  // Reads Data Bit 0 input
const uint8_t APDS9960_dataBit1Pin        = 1;  // Reads Data Bit 1 input
const uint8_t APDS9960_dataBit2Pin        = 2;  // Reads Data Bit 2 input
const uint8_t APDS9960_dataBit3Pin        = 3;  // Reads Data Bit 3 input
//const uint8_t APDS9960_dataBit4Pin        = 4;  // Reads Data Bit 4 input. Not used
const uint8_t APDS9960_gestureAvilablePin = 5;  // Read Gesture Available Output from ADPS9960
const uint8_t APDS9960_gestureClearPin    = 6;  // Writes to Gesture Clear output
const uint8_t APDS9960_resetADPS9960Pin   = 7;  // Writes to Reset Output

#define APDS9960_GVAL_MASK 	   0b00001111

#define APDS9960_GVAL_NONE    ((uint8_t)0x00)
#define APDS9960_GVAL_ERROR   ((uint8_t)0x0f)
#define APDS9960_GVAL_UP      ((uint8_t)0x01)
#define APDS9960_GVAL_DOWN    ((uint8_t)0x02)
#define APDS9960_GVAL_LEFT    ((uint8_t)0x03)
#define APDS9960_GVAL_RIGHT   ((uint8_t)0x04)
#define APDS9960_GVAL_NEAR    ((uint8_t)0x05)
#define APDS9960_GVAL_FAR     ((uint8_t)0x06)



class APDS9960_NonBlocking
{
 public:
    // Create an instance of the APDS9960_NonBlocking library and pass the I2C address of the NXP PCF8574P connected to the ATMega328P
    APDS9960_NonBlocking(uint8_t address);

    // Initialise this instance, returns false if unable to establish connection with NXP PCF8574P
	bool init(void);

	// Reads the output from the NXP PCF8574P and masks off upper nibble leaving only D0 ... D3
	uint8_t read(void);

	// Returns true if ~Gesture Available input is active low. ie. There has been a successful gesture read at the APDS9960
    bool gestureAvailable(void);

	// Call this to clear the last recorded gesture on D0 ... D3
    void gestureClear(void);

	// Call this to reset the ATMega328P. User must select High then Low.
	void reset(boolean resetState);

 protected:
    uint8_t _address;
    uint8_t _state;
    uint8_t _bytes;
    uint8_t _APDS9960_GVAL;
};


#endif
