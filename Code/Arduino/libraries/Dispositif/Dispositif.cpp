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
  else if("harlem" == action)      // Execute an harlem shake
  {
    Serial.println("AND THAN WE DO THE HARLEM SHAKE !!!!!");
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

        i -=1;
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
    //Serial.print("intensite à emmetre : ");  //Debug code show the initial value
    //Serial.print(p);                         //Debug code
    float temp =(float) percent/100;
    temp *= 255;
    percent = (int) temp;  //we convert a /100 rate to a /255 cause the arduino can provide 256 shade of powers.
    //Serial.println("");                      //Debug code
    //Serial.print("devient : ");              //Debug code
    //Serial.print(p);                         //Debug code show the real output of arduino
    //Serial.println("");                      //Debug code
    //Serial.println(pin);                   //Debug code show the output led
    analogWrite(pin, percent);                  // provide a special power between 0-5V to light up the led
}

//This method get the zone and the power of the lamp one after one, this can be use to analyse part after part
void Dispositif::Analyse(String code, int zone)
{
  // if we are still looking to the current zone of the order
  if(orderZone)
  {
    //Serial.println(code);    //Debug code show the order zone area
    int temp = code.toInt(); // Convert a string to a int cause the zone is an string
    // we check we are in the good zone, if it's true, the next part can be read
    if(temp == zone)
    {
      zoneMatch = true;
    }
    else
    {
      zoneMatch = false;
    }
    orderZone = false;         // Say we have finished looking the zone
    //Serial.print(" zone : ");  //Debug code Show the zone of the arduino
    //Serial.println(zone);      //Debug code
  }
  else
  {
    // we are reading the order and the next step is the a new patern than we put orderZone to true than
    orderZone = true;
    //we are in the good zone so we execute the order
    if(zoneMatch)
    {
       //Serial.println(code);  //Debug code show the code that's send from internet to arduino for this zone
       int p = code.toInt();  // Convert a string to a int cause we need a % in int
       Luminaire(p);
    }
  }
}

//this method is a method that get the zone of the arduino, the zone of the ordrer and the order. he will get the result quiqly but need some works from the user
void Dispositif::Analyse(String codeZone, String code, int zone)
{
  int temp = codeZone.toInt();
  if(temp == zone)
  {
    int p = code.toInt();
  }
}
//this method is a method that get the zone of the arduino, the zone of the ordrer and the order. he will get the result quiqly and only use int so the user must convert information himself
void Dispositif::Analyse(int codeZone, int code, int zone)
{
  if(codeZone == zone)
  {
    Luminaire(code);
  }
}
