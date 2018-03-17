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
// Compiled using Arduino 1.6.9, Written for Arduino Due
// Used to test APDS9960 interface via a parallel control lines. 
// Note : Reset control is not included.
//


#define APDS9960_GVAL_NONE    ((byte)0x00)
#define APDS9960_GVAL_ERROR   ((byte)0x0f)
#define APDS9960_GVAL_UP      ((byte)0x01)
#define APDS9960_GVAL_DOWN    ((byte)0x02)
#define APDS9960_GVAL_LEFT    ((byte)0x03)
#define APDS9960_GVAL_RIGHT   ((byte)0x04)
#define APDS9960_GVAL_NEAR    ((byte)0x05)
#define APDS9960_GVAL_FAR     ((byte)0x06)

#define DEBUG_GENERAL            // Undefine this for general debug information via the serial port. 
#define GESTURE_CLEAR      2     // Digital pin 2
#define GESTURE_AVAILABLE  7     // Digital pin 7
#define GESTURE_D0         8     // Digital Pin 8
#define GESTURE_D1         9     // Digital Pin 9
#define GESTURE_D2         10    // Digital Pin 10
#define GESTURE_D3         11    // Digital Pin 11

byte bDataNew;
byte bDataOld;




void setup (void)
{
  #ifdef DEBUG_GENERAL
  Serial.begin(115200);           // start serial for output
  Serial.println("Serial port Active");
  #endif  


  pinMode(GESTURE_CLEAR,     OUTPUT);
  pinMode(GESTURE_AVAILABLE, INPUT);
  pinMode(GESTURE_D0,        INPUT);
  pinMode(GESTURE_D1,        INPUT);
  pinMode(GESTURE_D2,        INPUT);
  pinMode(GESTURE_D3,        INPUT);

  if (!digitalRead(GESTURE_AVAILABLE)) {
      digitalWrite(GESTURE_CLEAR, LOW);
      delay(1);
      digitalWrite(GESTURE_CLEAR, HIGH);
  }  

  bDataNew = 0;
  bDataOld = 0;
  delay(1000);
}






void loop (void)
{
  char cArray[30];

  if (!digitalRead(GESTURE_AVAILABLE)) {
    bDataNew = 0;
    if (digitalRead(GESTURE_D0))
      bDataNew |= (1 << 0);
    if (digitalRead(GESTURE_D1))
      bDataNew |= (1 << 1);
    if (digitalRead(GESTURE_D2))
      bDataNew |= (1 << 2);
    if (digitalRead(GESTURE_D3))
      bDataNew |= (1 << 3);

    digitalWrite(GESTURE_CLEAR, LOW);
    delay(1);
    digitalWrite(GESTURE_CLEAR, HIGH);
  }
  
  
  if (bDataNew != bDataOld) {  
    switch(bDataNew) {
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
    bDataOld = bDataNew;
  }
}
