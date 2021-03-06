#include <GSM.h>	          // GSM shield
#include <SPI.h>
#include <SD.h>
#include "Dispositif.h"		  // Our library

//+++++++++++++++++++ Modify if you change the GSM provider ++++++++++++++++++//
// PIN number of the SIM card
#define PINNUMBER "4632"

// APN information obtained from your network provider.
#define GPRS_APN "mworld.be" // replace with your GPRS APN
#define GPRS_LOGIN ""        // replace with your GPRS login
#define GPRS_PASSWORD ""     // replace with your GPRS password

//++++++++++++++++++ Data specific to the current Arduino ++++++++++++++++++++//
// Zone = 1, id = 1, idZone = 2
Dispositif dispositif("1", "1", "2");

//++++++++++++++++++ Data specific to our web site +++++++++++++++++++++++++++//
//IPAddress server(192,168,0,101);// IP for the website, useful when using localhost)
//char server[] = "http://dilui.eripm.be/";  // name address of website, thx DNS
//char path[] = "http://dilui.eripm.be/"; // the path where we found the data.
#define server "dilui.eripm.be"  // name address of website, thx DNS
#define path "http://dilui.eripm.be/?src=arduino" // the path where we found the data.
#define port 80 // the port, 80 for HTTP.

// initialize the library instances specific to the GSM/GPRS shield.
GSMClient client;
GPRS gprs;
GSM gsmAccess;

/* Variables used in the parsing */
String currentLine = "";
String code = "";                 // string to hold the code.
String currentZone = "";          // temporary, zone of the current order.
boolean readingZone = true;
boolean readingCode = false;      // true when parsing an <order>
boolean readingSudo = false;      // true when parsing an <sudorder>
uint8_t count = 0;

/* ChangeZone() variables */
boolean readingSwap = false;

void setup() 
{
  // reserve space for strings;
  currentLine.reserve(256);
  code.reserve(256);

  // Initialize serial to start the Xbee communication at 9600 bauds.
  Serial.begin(9600); 

  //Serial.println("start GPRS"); // Debug

   
  /*Start GSM shield
  Since it sometimes has trouble connecting, we do a little loop
  */
  boolean notConnected = true;
  while(notConnected)
  {
    if((gsmAccess.begin(PINNUMBER) == GSM_READY) &                             
        gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY)
        {
          notConnected = false; // connected, we can go on !
	}
    else
    {
      delay(2000);			// wait for 2 secs and then retry
    }
  }
  //Serial.println("connected");
  connectToData();          // We can now retrieve the data*/
}

void loop()
{
  while(client.available()) 
  {
    char c = client.read();  // we read the current char send from internet.
    currentLine += c;        // we add the current char to the buffer string.

    Serial.write(c);       // Debug
	
    // Clear buffer for nextline
    if (c == '\n')
        currentLine = "";

    // What are we gonna parse ?
    if (currentLine.endsWith("<order>"))
    {
      readingCode = true;
      code = "";
      return; // break out of the loop so 'c' isn't added to the order
    }
    else if(currentLine.endsWith("<sudorder>"))
    {
       readingSudo = true;
       code = "";
       return;
    }
    else if(currentLine.endsWith("<changeZone>"))
    {
      readingSwap = true;
      code = "";
      return;
    }
    
    // Parsing an <order>
    if (readingCode)
    {
      if ('(' == c) // when getting a '(' it means we finished getting the zone#
      {
        // Repeat order on Xbee
        Serial.print(code);
        Serial.print(F("("));
        
        currentZone = code;
        code = "";
        readingCode = true;
      }
      
      else if (',' == c)
      {
        // Repeat to Xbee
        Serial.print(code);
        Serial.print(F(","));
        
        if (currentZone == dispositif.zone)
        {
          dispositif.setBrightness(count, code.toInt());
          code = "";
        }
        count++;
      }
      
      else if (')' == c)
      {
        // Repeat to Xbee
        Serial.print(code);
        Serial.print(F(")"));
        
        if (currentZone == dispositif.zone)
        { 
          dispositif.setBrightness(count, code.toInt());
          code = "";
        }
        count = 0;
      }
      
      else if ('<' == c) // end of order
        readingCode = false;
      
      code += c;
    }

    // parsing a <sudorder>
    else if (readingSudo) 
    {      
      if ('<' == c)
      {
        readingSudo = false;
        dispositif.Action(code);
        
        Serial.print(code);     // Send the order on Xbee
        Serial.println("$");    // '$' needs to be added
      }
      code += c;
    }

    // <swapZone>
    else if (readingSwap) 
    {
      changeZone(c);   //we call the specific method
    }
  }
  
  // if the server's disconnected
  if (!client.connected()) 
  {
    client.stop();
    delay(600000); // 10 min wait
    connectToData();
  }
}

//connect to the website and get the page reserved for the Arduino
void connectToData()
{
  //Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, port)) 
  {
    //Serial.println("Connected to server");
    // Make a HTTP request from this page
    client.print(F("GET "));           //we ask to get the path we ask.
    client.print(path);
    //client.print("&id="); // uncomment when Romain has finalised its code
    //client.println(dispositif.id);// idem
    /*client.println(" HTTP/1.1");*/
    client.print(F("Host: "));         //we show from where we want the page.
    client.println(server);
    // And then close the connection.
    client.println(F("Connection: close")); //we close the connection.

    //Serial.println("Connected to Web Site !");
  }
}

//this method is used to swap the current zone of a specific arduino.
void changeZone(char c)
{
  String networkID = "";            // contains the ID of the concerned network
String NodeID = "";               // contains the node ID of the order.
boolean readingID = true;
boolean readingNode = false;
  // if you got a "!" character, you've reached the end of an order part.
  if (c != '!')
  {
     code += c;
  }
  else         //we reach an end of a part of the swapping zone.
  {
    if(readingID) // if we are reading the zone of the order, we put it in a temp
    {
      networkID = code;   // we save the first part
      readingID = false;  // update information about which part we read
      readingNode = true;
    }
    else if(readingNode) //if we are reading the node that must change zone
    {
      NodeID = code;        //save the second part
      readingNode = false;  //update status of reading part
    }
    else           // reading the new zone and can see if we are in the good id and if other nodes will need this
    {
      if(networkID == dispositif.id) // id match 
      {
        if(NodeID == dispositif.idNode) // it's the current node
        {
          dispositif.zone = code;  // we change the zone of this node.
        }
        else  //it's the node of a xbee on our tree than we send them the order
        {
          //send to xbee the current order for swaping the zone of an arduino.
          Serial.print(NodeID); //send the id of the node.
          Serial.print(F("#"));    //say we reach a part of the code
          Serial.print(code);   //we send the reste with the new zone.
          Serial.println(F("#"));  //we say taht the ordre of swapping zone is ended.
        }
      }
      readingID = true;           //reset the bool.
      networkID = "";             //reset the temp data.
      NodeID = "";
    }
    code = "";                    //refresh the temp data that contain the code.
  }
  
  if (c == '<') //The order list is ended
  {
    readingSwap = false;              //we aren't reding order anymore.
  }
}
