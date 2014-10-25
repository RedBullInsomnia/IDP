#ifndef Dispositif_h
#define Dispositif_h

#include "Arduino.h"


class Dispositif
{
        public:
                /*
                 * Constructs a led driver on pin : pin
                 */
                Dispositif(int pin);

                /*
                 * Execute one of the following : jour, nuit, blink, harlem
                 */
                void Action(String action);

                /*
                 * Set Brightness of Led Driver (percentage as input)
                 */
                void setBrightness(int percent);

                /*
                 * This method get the zone and the power of the lamp one after one, this can be use to analyse part after part
                 */
                void Analyse(String code, int zone);

                /*
                 * This method is a method that get the zone of the arduino, the zone of the ordrer and the order. he will get the result quiqly but need some works from the user
                 */
                void Analyse(String codeZone, String code, int zone);

                /*
                 * Gets the zone of the driver, the zone of the order and the order itself. 
				 * he will get the result quickly and only use int so the user must convert information himself
                 */
                void Analyse(int codeZone, int code, int zone);

        private:
                // Ouput pin on which the output will be located
                int pin;

                // True if Led is on, while on SUDO mode
                boolean ledOn;

                // To know if we see the zone of order or the order.
                boolean orderZone;

                // To know if the current zone order is the same as Arduino zone.
                boolean zoneMatch;
};

#endif
