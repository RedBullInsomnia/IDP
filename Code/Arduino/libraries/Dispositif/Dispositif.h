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
                 *
                 */
                void Analyse(String code, int zone);

                /*
                 *
                 */
                void Analyse(String codeZone, String code, int zone);

                /*
                 *
                 */
                void Analyse(int codeZone, int code, int zone);

        private:
                // Ouput pin on which the output will be located
                int pin;

                // True if Led is on, while on SUDO mode
                boolean ledOn;

                // To know if we see the zone of order or the order.
                boolean orderZone;

                // To know if the current zone order is the same as arduino zone.
                boolean zoneMatch;
};

#endif
