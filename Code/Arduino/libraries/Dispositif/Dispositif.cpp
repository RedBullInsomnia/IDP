/*
  Dispositif.cpp
  Library for the IDP project "Dispositif Lumineux"
  Created on 24/10/2014.

  *************************** Description **************************************
  This library contains several methods to simplify the main code of the Arduino
  Multiple zones can easily be controlled by the same Arduino because the zone
  of the Arduino is asked at every request that make the arduino completely
  compatible with other technology than Wi-Fi if we use the same information
  packaging.

  ***************************** Methods ****************************************
  Super user / Action:
        the information must be a special string from this list:
                -jour   : turn the light on.
                -nuit   : turn the light off.
                -harlem : turn the light on than off on a special frequency
                            which progressively gets lower.
                -blink  : turn the light on if it's off and off if it's on each
                            time the Arduino asks for information from the
                            server.
  analyse:
        this method will analyse the zone the two different zone and will turn on the ligh if the two zones match. the zone of the arduino must be an int but the zone of the order is a string (easyly to get it but the code is modulable). The order is a string wich contain an int between 0 and 100 (we can put higher but won't produce it).
*/

#include "Arduino.h"
#include "Dispositif.h"

Dispositif::Dispositif(int pin)
{
  pin = pin;
  Serial.println(pin);
  Serial.println(pin);
  pinMode(pin, OUTPUT);

  ledOn = false;
  orderZone = true;
  zoneMatch = false;
}

void Dispositif::Action(String action)
{
  if("jour" == action)             // Turn the light on
  {
    digitalWrite(pin, HIGH);
    ledOn = true;
  }
  else if("nuit" == action)        // Turn the light off
  {
    digitalWrite(pin, LOW);
    ledOn = false;
  }
  else if("harlem" == action)      // Harlem Shake
  {
    Serial.println("AND THAN WE DO THE HARLEM SHAKE !");
    int i = 3;
    int j = 10;
    while(i > 0)
    {
        while(j > 0)
        {
            digitalWrite(pin,HIGH);
            delay(j*10);
            digitalWrite(pin,LOW);
            delay(j*10);
            j -= 1;
        }
        while(j < 10)
        {
            digitalWrite(pin,HIGH);
            delay(j*10);
            digitalWrite(pin,LOW);
            delay(j*10);
            j += 1;
        }

        i -= 1;
    }
  }
  else if(action == "blink")       // Make the led blink at the speed of page refresh
  {
     if(ledOn)
     {
       digitalWrite(pin, LOW);
       ledOn = false;
     }
     else
     {
       digitalWrite(pin, HIGH);
       ledOn = true;
     }
  }
}

void Dispositif::setBrightness(int percent)
{
    //Serial.print("intensite à emmetre : ");
    //Serial.print(p);
    float temp =(float) percent/100;
    temp *= 255;
    percent = (int) temp;  //we convert a /100 rate to a /255 cause the arduino can provide 256 shade of powers.
    //Serial.println("");
    //Serial.print("devient : ");
    //Serial.print(p);
    //Serial.println("");
    //Serial.println(pin);
    analogWrite(pin, percent);		// provide a special power between 0-5V to light up the led
}


void Dispositif::Analyse(String code, int zone)
{
  // if we are still looking to the current zone of the order
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
    orderZone = false;         // Zone checking finished
    //Serial.print(" zone : ");
    //Serial.println(zone);      
  }
  else
  {
    // 
    orderZone = true;
    // if we are in the right zone, we can update the brightness
    if(zoneMatch)
    {
       //Serial.println(code);  // Debug code shows the code that's sent from the internet to arduino for this zone
       int p = code.toInt();  // Code is a string
       Luminaire(p);
    }
  }
}

void Dispositif::Analyse(String codeZone, String code, int zone)
{
  int temp = codeZone.toInt();
  if(temp == zone)
  {
    int p = code.toInt();
  }
}

void Dispositif::Analyse(int codeZone, int code, int zone)
{
  if(codeZone == zone)
  {
    Luminaire(code);
  }
}
