#ifndef Dispositif_h
#define Dispositif_h

#include "Arduino.h"


class Dispositif
{
        public:
                Dispositif(int pin);
                void Action(String action);
                void Luminaire(int p);
                void Analyse(String code, int zone);
                void Analyse(String codeZone, String code, int zone);
                void Analyse(int codeZone, int code, int zone);
        private:
                int lampe;
                boolean ledon;
                boolean orderZone;
                boolean zoneMatch;
};

#endif
