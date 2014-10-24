  //library usefull for an internet connection and the GSM shield.
#include <GSM.h>
//library used for the light control.
#include <Dispositif.h>
//-----------------------------------------------------------------------------------------------------------------------
//+++++++++++++++++++++++++++++++++++++++++++++++++modify if you change the GSM provider+++++++++++++++++++++++++++++++++
// PIN number of the sim card if necessary (code pin).
#define PINNUMBER "5667"

// APN information obrained from your network provider.
#define GPRS_APN "mworld.be" // replace with your GPRS APN
#define GPRS_LOGIN ""        // replace with your GPRS login
#define GPRS_PASSWORD ""     // replace with your GPRS password
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++all the specific data of the current arduino++++++++++++++++++++++++++
int id = 1;                           //this is the id of the arduinoServer.
int idNode = 2;                       //this is the id of the arduino on the Xbee tree.

Dispositif dispositif(9);            //start a new light to control it easyly.
int zone = 1;                        // int to know our zone.

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++Data specific to our web site+++++++++++++++++++++++++++++++++++++++++++++++++

//IPAddress server(192,168,0,101);           // numeric IP for the website(no DNS) we can use both to connect (usefull in local test) :).
char server[] = "lumiere.lea-valbenoit.eu";  // name address for our website (using DNS) (usefull when the site will be up).

char path[] = "http://lumiere.lea-valbenoit.eu/index.php?section=Ordre"; // the path where we found the data.
int port = 80; // the port, 80 for HTTP.
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//-----------------------------------------------------------------------------------------------------------------------

// initialize the library instances specific to the GSM/GPRS shield.
GSMClient client;
GPRS gprs;
GSM gsmAccess;

String currentLine = "";              // string to hold the text from server.
String code = "";                     // string to hold the code.
boolean readingCode = false;          // bool to know if we are reading an order.
boolean readingZone = true;          // bool to know if we read the zone or the order.
String currentZone = "";             // tempory, zone of the current order.
boolean readingSudo = false;          // bool to know if we are reading a sudo order.

boolean readingSwap = false;         //bool to know if we are zwapping zone of arduino.
String networkID = "";               //contain the ID of the network which is concerned.
String NodeID = "";                  // contain the node ID of the order.
boolean readingID = true;            // if we read the first step of swaping zone.
boolean readingNode = false;         // the second step.

void setup() 
{
  //resereve space for strings;
  currentLine.reserve(256);
  code.reserve(256);

  //Initialize serial to start the Xbee communication at 9600 bauds.
  Serial.begin(9600); 

  //Serial.println("start GPRS"); //debug code.

  delay(1000);                 //wait 1 sec for leonardo arduino.

  boolean notConnected = true; //bool to know if we are connected to quit the next loop.

  // Start GSM shield
  while(notConnected)//while we are not connected we do this loop.
  {
    if((gsmAccess.begin(PINNUMBER)==GSM_READY) &                             //we try if the GSM is connected
        (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD)==GPRS_READY))  //and if the GPRS is ready to be used.
      notConnected = false;                                                  //we break the loop by putting the bool at true.
    else
    {
      delay(1000);                                                           //else we wait and retry.
    }
  }
  //Serial.println("connected"); //this is a debug part.
  connectToData();               //we connect to the web site page which contain the data we require.
}

void loop()
{
  // if there are incoming bytes available from the server.
  while(client.available()) 
  {
    char c = client.read();  // we read the current char send from internet.
    currentLine += c;        //we ad the current char to the buffer string.

    //Serial.write(c);         //write what the arduino see to the debug windows.

    if (c == '\n')           //if we got a new line, we clear the buffer to be sure we won't saturate the arduino memory.
    {
        currentLine = "";
    }

    // if the current line ends with <order> (or something else if we got a valid tag), it will be followed by the order.
    if ( currentLine.endsWith("<order>"))
    {
      // order is beginning. Clear the code string and say we will get an order.
      readingCode = true;
      code = "";
      return;// break out of the loop so this character isn't added to the order.
    }
    // if the current line ends with <sudorder>, it's a sudo order used to debug the system.
    else if(currentLine.endsWith("<sudorder>"))
    {
       // order is beginning. Clear the code string and say we will get an order.
       readingSudo = true;
       code = "";
       return; //break the loop so the > character won't be added to the code.
    }
    // if the current line ends with <swapzone>, it's a order to change the zone of a specific light.
    else if(currentLine.endsWith("<swapzone>"))
    {
       // order is beginning. Clear the code string and say we will get an order.
       readingSwap = true;
       code = "";
       return; //break the loop so the > character won't be added to the code.
    }

    // if we are reading the classic order sent from the web site.
    if (readingCode) 
    {
      // if you got a "!" character, you've reached the end of an order and you can analyse it.
      if (c != '!') //if we don"t have a '!' we just store the char on the temp value.
      {
        code += c;
      }
      else         //the order 66 is given !!! we reach the end of a part of the regular order.
      {       
        if(readingZone) //if we are reading the zone of the order, we put it in a temp and wait to get the power of the light too.
        {
          currentZone = code;
        }
        else           //else we are reading the order that give th e powe and can send to the library the whole information.
        {
          //dispositif.Analyse(currentZone, code, zone);
          if(zone == currentZone.toInt());      //if we are in the zone of the order.
          {
            dispositif.Luminaire(code.toInt()); //we put the power on the lamp to be updated correctly.
          }
          //send to xbee the current order.
          Serial.print(currentZone); //send the zone.
          Serial.print("!");         //send the separation between the two.
          Serial.print(code);        //send the power of the light.
          Serial.println("!");       //we say that the simple ordre is ended.
          currentZone = "";          //reset the temp data of zone to be sure having only 1 zone a time.
        }
        readingZone = !readingZone;     //be sure we will read the zone than the order
        code = "";                      //refresh the temp data to have storage space.
      }
      if (c == '<') //The order list is ended when we reach the '<' char.
      {
        readingCode = false;              //we aren't reding order anymore.
        //Serial.println("Fin des codes");  //debug code
      }
    }

    //if you are reading a sudo order. We know it with the boolean specialy use for t
    else if (readingSudo) 
    {
      if (c != '<') //The order continue till we don't have the end char '<' cause we can only have one sudo order at time.
      {
        code += c;
      }
      else         //the order 66 is given!!! The end of the code is reach and we can now analyse him.
      {
        readingSudo = false;   //we aren't reding order anymore
        dispositif.Action(code);   //we do the action ask by the sudo

        Serial.print(code);        //we send it to the other arduino to allow us to pilot them easyly.
        Serial.println("$");       //and ad the char that say to other arduino that the sudordre is end.
      }
    }

    //if we are reading a swaping zone
    else if (readingSwap) 
    {
      swapingZone(c);   //we call the specific method
    }
  }
  
  // if the server's disconnected
  if (!client.connected()) 
  {
    //We stop the communication with the client
    client.stop();
    //we wait before retry cause we don't want to ddos the website.
    delay(600000); //we wait 10 mins. Usefull to test if he still connect during long time functioning.
    //delay(300000); //we wait 5 mins.
    //and try another connection
    connectToData();
  }
}

//connect to the web site and get the page reserved for the arduino
void connectToData()
{
  //Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, port)) 
  {
    //Serial.println("connected to server"); //debug part.
    // Make a HTTP request from this page
    client.print("GET ");           //we ask to get the path we ask.
    client.print(path);
    client.print("&id=");
    client.println(id);
    //client.println(" HTTP/1.1");
    client.print("Host: ");         //we show from where we want the page.
    client.println(server);
    // And than close the connection.
    client.println("Connection: close"); //we close the connection.

    //Serial.println("Connected to Web Site !"); //debug code
  }
}

//this method is used to swap the current zone of a specific arduino.
void swapingZone(char c)
{
  // if you got a "!" character, you've reached the end of an order part.
  if (c != '!') //The order continue if we don't have a '!' and we can add the current char in the temp.
  {
     code += c;
  }
  else         //the order 66 is given !!!we reach an end of a part of the swapping zone.
  {
    if(readingID) //if we are reading the zone of the order, we put it in a temp
    {
      networkID = code;   //we save the fisrt part
      readingID = false;  //update information about which part we read
      readingNode = true;
    }
    else if(readingNode) //if we are reading the node that must change zone
    {
      NodeID = code;        //save the second part
      readingNode = false;  //update status of reading part
    }
    else           //else we are reading the new zone and can see if we are in the good id and if other nodes will need this
    {
      if(networkID == String(id)) //id match 
      {
        if(NodeID == String(idNode)) //it's the current node
        {
          zone = code.toInt();  //we change the zone of this node.
        }
        else  //it's the node of a xbee on our tree than we send them the order
        {
          //send to xbee the current order for swaping the zone of an arduino.
          Serial.print(NodeID); //send the id of the node.
          Serial.print("#");    //say we reach a part of the code
          Serial.print(code);   //we send the reste with the new zone.
          Serial.println("#");  //we say taht the ordre of swapping zone is ended.
        }
      }
      readingID = true;           //reset the bool.
      networkID = "";             //reset the temp data.
      NodeID = "";
    }
    code = "";                      //refresh the temp data that contain the code.
  }
  if (c == '<') //The order list is ended
  {
    readingSwap = false;              //we aren't reding order anymore.
  }
}
