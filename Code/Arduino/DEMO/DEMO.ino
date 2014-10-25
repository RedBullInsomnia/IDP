#include <Dispositif.h>

int pos = 0;
#define taille 20 
int tab1 [] = {0  , 20, 40, 60, 80, 100, 100, 100, 100, 100,  75,  50,  25,   0,   0, 100, 100,   0,   0, 100};
int tab2 [] = {100, 80, 60, 40, 20, 0  , 25 , 50 , 75 , 100, 100, 100, 100, 100, 100,   0,   0, 100, 100,   0};
Dispositif lampe2(9);
Dispositif lampe1(5);

void setup() 
{
  Serial.begin(9600); 
}

void loop()
{
 //we start a loop that will Do the loop of controll 
 for(int i = 0; i < taille; i += 1)
 {
   lampe1.Luminaire(tab1[i]); //we put the light intensity 
   lampe2.Luminaire(tab2[i]);
   Serial.print("2!");           //we send the current order to the other light
   Serial.print(tab2[i]);
   Serial.println("!");
   Serial.print("1!");
   Serial.print(tab1[i]);
   Serial.println("!");
   
   delay(30000); //wait 30 sec to give the time to see the light
 }
}
