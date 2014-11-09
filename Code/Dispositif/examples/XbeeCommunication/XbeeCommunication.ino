#include <Dispositif.h> // Our custom library

int idNode = 2; 		// ID of the Dispositif in our tree

int zone = 1;         	// Our Arduino is in zone 1
Dispositif lampe(9);  	// LED is connected to PIN 9

String currentLine = "";              // string to hold the text from server.
String code = "";                     // string to hold the code sent from server.
String currentZone = "";              // temporary, zone of the current order.
boolean readingCode = false;          // bool to know if we are reading an order.
boolean readingZone = true;           // bool to know if we read the zone or the order.
boolean readingSudo = false;          // bool to know if we are reading a sudo order.

/* Variables used for swapping */
String node = "";                     
boolean readingNode = true;           

void setup()
{
  //reserve space for strings;
  currentLine.reserve(256);
  code.reserve(256);
  
  // start serial port at 9600 bps:
  Serial.begin(9600);
}

void loop()
{
  if (Serial.available() > 0)
  {
    char c = Serial.read();  //read the current char sent from internet.
    currentLine += c;        //add the current char to the buffer string.

    // if you got a "!" character, you've reached the end of an order and must analyse the situation.
    if (c != '(' && c!= ')' && c != ',') //The order continue if we don't see a '!'.
    {
      if(c == '$') //we saw an action code because the $ says it's an action.
      {
        lampe.Action(code);   //we analyse the code we received.
        Serial.print(code);   //and send it to the other arduinos.
        Serial.println("$");  //and say it's an action to the other arduinos.
        
        code = "";            //clear the temp message to allow another to be read.
      }
      else if(c == '#')       //we see a swaping zone code cause we intercept a #.
      {
        if(readingNode)       //we first catch the node id of the order to know if are this node.
        {
          node = code;        //we store the node id on the temp 
        }
        else                 //we already got the node ID now we must have the zone.
        {
          if(idNode == node.toInt())  //if the order id and node id match, we are the node they search.
          {
            swap(code);             //we change our zone.
          }
          else                       //else if we are not the arduino affected by the swapping zone, we send the data to other arduino trought the Xbee.
          {
            Serial.print(node);   //send the id that muste change.
            Serial.print("#");    //send the first # to stop the id part.
            Serial.print(code);   //send the new zone of an arduino.
            Serial.println("#");  //end the line with the second # to conclude the swapping zone data sending.
          }
          node = ""; //we clear the temp string to allow other id data for the other swapping zone to be stored.
        }
        code = "";   //we clear the temp string to allow other data to be stored.
        readingNode = !readingNode;      //we reset the temp data.
      }
      else  //if we don't ended a special type of data, we just add the current caracter to the temp to be analysed whend we reach a special caracter.
      {
        code += c; //we store it to the temp.
      }
    }
    else     // The '!' is reached and we reach the end of a part of an usual code.
    {       
      if(readingZone) //if we are reading the zone of the order, we put it in a temp.
      {
        currentZone = code;
      }
      else   //else we are reading the order that say the power of the light and can send to the library the whole information.
      {
        if(currentZone.toInt() == zone)    //if we are in the zone that's affect the current order.
        {
          lampe.Luminaire(code.toInt());  //we modify the power of the lamp to be updated and work normaly.
        }
        Serial.print(currentZone); //we send to the other arduino the zone that will be affected.
        Serial.print("!");         //we say that the end of the zone
        Serial.print(code);        //we show the power of the light for the zone send previously.
        Serial.println("!");       //
        currentZone = "";             //reset the temp data.
      }
      readingZone = !readingZone;     //be sure we will read the zone than the order
      code = "";                      //refresh the temp data
    }
  }
}

// this method change the zone of the current node.
void swap(String newzone)
{
  zone = newzone.toInt();
}
