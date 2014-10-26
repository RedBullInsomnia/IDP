#include "Arduino.h"
#include "Dispositif.h"

Dispositif::Dispositif()
{
  pin1 = 5;
  pin2 = 6;
  pin3 = 9;
  pinMode(9, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);

  ledOn = false;
  orderZone = true;
  zoneMatch = false;
}

void Dispositif::Action(String action)
{
  if("jour" == action)             // Turn the light on
  {
    digitalWrite(pin1, HIGH);
    ledOn = true;
  }
  else if("nuit" == action)        // Turn the light off
  {
    digitalWrite(pin1, LOW);
    ledOn = false;
  }
  else if("harlem" == action)      // Harlem Shake
  {
    int i = 3;
    int j = 10;
    while(i > 0)
    {
        while(j > 0)
        {
            digitalWrite(pin1,HIGH);
            delay(j*10);
            digitalWrite(pin1,LOW);
            delay(j*10);
            j -= 1;
        }
        while(j < 10)
        {
            digitalWrite(pin1,HIGH);
            delay(j*10);
            digitalWrite(pin1,LOW);
            delay(j*10);
            j += 1;
        }

        i -= 1;
    }
  }
  else if(action == "blink")	// LED blinks at the speed of page refresh
  {
     if(ledOn)
       digitalWrite(pin1, LOW);
     else
       digitalWrite(pin1, HIGH);
	   
	 ledOn = !ledOn;
  }
}

void Dispositif::setBrightness(int number, int percent)
{
    // Convert percentage into [0 255] range
    float temp = (float)percent / 100 * 255;
    
    int pin = 5;
    if (1 == number)
      pin = 6;
    else if (2 == number)
      pin = 9;
    
    analogWrite(pin, (int)temp);
}

void Dispositif::parseMessage(String code, int zone)
{
  if(orderZone)
  {
    //Serial.println(code);
    int temp = code.toInt();
    // we check we are in the good zone, if it's true, the next part can be read
    if(temp == zone)
    {
      zoneMatch = true;
    }
    else
    {
      zoneMatch = false;
    }
    orderZone = false; 
  }
  else
  { 
    orderZone = true;
    // if we are in the right zone, we can update the brightness
    if(zoneMatch)
    {
       int p = code.toInt();  // Code is a string
       setBrightness(0, p);
    }
  }
}
