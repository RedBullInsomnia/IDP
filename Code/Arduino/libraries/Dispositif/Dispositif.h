/*
  Dispositif.hpp
  Library for the IDP project "Dispositif Lumineux"
  Created on 24/10/2014.

  *************************** Description **************************************
  This library contains several methods to simplify the main code of the Arduino
  Multiple zones can easily be controlled by the same Arduino because the zone
  of the Arduino is asked at every request that make the arduino completely
  compatible with other technology than Wi-Fi if we use the same information
  packaging.
*/

#ifndef DISPOSITIF_H
#define DISPOSITIF_H

#include "Arduino.h"

class Dispositif
{
  public:
    /*
     * Constructor of a led driver on pins 5, 6 and 9 and assigns the zone that
     * is written on the SD card.
     *
     */
    Dispositif();

    /*
     * SUDO operation
             * Execute one of the following sudo actions (string) :
     * 	- jour : turn the light on
     *  - nuit : turn the light off
     *  - blink : invert brightness each time the Arduino connects 
     *				to the server 
     *  - harlem : turn the light on then off on a special frequency
     *           which progressively gets lower.
     */
      void Action(String action);

    /*
     * Set Brightness of Led Driver (percentage as input)
     */
    void setBrightness(int number, int percent);

    /*
     * Parses the message sent from the remote server.
     * Message takes the following form : 
     * zone#(r%, g%, b%)zone#(r%, g%, b%)zone#(r%, g%, b%)...
     */
    void parseMessage(String code, int zone);

  private:
    // Pins on which the output will be located
    int pin1, pin2, pin3;
    
    // zone of the Driver
    int zone;

    // True if Led 0 is on, while on SUDO mode
    boolean ledOn;

    // To know if we see the zone of order or the order.
    boolean orderZone;

    // To know if the current zone order is the same as Arduino zone.
    boolean zoneMatch;
};

#endif
