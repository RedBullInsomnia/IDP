//library usefull for an internet connection
#include <SPI.h>
#include <WiFi.h>
//library used for the light control
#include <Dispositif.h>

int id = 1;                           //this is the id of the arduinoServer
int idNode = 1;                       //this is the id of the arduino on the Xbee tree

Dispositif dispositif(9);            //start a new light to control it easyly
int zone = 1;                        // int to know our zone

char ssid[] = "SatCamp";            //  your network SSID (name) VOO-948564
char pass[] = "";              // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                      // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;          //Show the current status of the WiFi shield

IPAddress server(192,168,0,101);        // numeric IP for Google (no DNS) we can use both to connect (usefull in local test)
//char server[] = "thanatheos.verygames.be";  // name address for Google (using DNS) (usefull when the site will be up)

String currentLine = "";              // string to hold the text from server
String code = "";                     // string to hold the code
boolean readingCode = false;          // bool to know if we are reading an order
boolean readingZone = true;          // bool to know if we read the zone or the order
String currentZone = "";             // tempory, zone of the current order
boolean readingSudo = false;          // bool to know if we are reading a sudo order

boolean readingSwap = false;         //bool to know if we are zwapping zone of arduino
String networkID = "";               //contain the ID of the network which is concerned
String NodeID = "";                  // contain the node ID of the order
boolean readingID = true;            // if we read the first step of swaping zone
boolean readingNode = false;         // the second step

WiFiClient client;                    // Initialize the Ethernet client library

void setup()
{
  //resereve space for stings
  currentLine.reserve(256);
  code.reserve(256);


  //Initialize serial to debug the code
  Serial.begin(9600);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD)
  {
    Serial.println("WiFi shield not present");
    // The shield is not connected so we can't continue and need a reset
    while(true);
  }

  // attempt to connect to Wifi network while we're not connected
  while (status != WL_CONNECTED)
  {
    //debug code for connecting at wifi
    //Serial.print("Attempting to connect to SSID: ");
    //Serial.println(ssid);
    // Connect to network. Change this line if using another network
    status = WiFi.begin(ssid, pass); //status = WiFi.begin(ssid, pass, keyIndex);

    // wait 10 seconds for another try of connection
    delay(10000);
  }
  //Serial.println("Connected to wifi");
  //printWifiStatus();

  connectToData();    //we connect to the web site page which contain the data we require
}

void loop()
{
  // if there are incoming bytes available from the server, read them and print them:
  while(client.available())
  {
    char c = client.read();  //read the current char send from internet
    currentLine += c;        //ad the current char to the buffer string
    //Serial.write(c);         //write what the arduino see

    if (c == '\n')           //if we got a new line, we clear the buffer
    {
        currentLine = "";
    }

    // if the current line ends with <order> (or something else if we got a valid tag), it will be followed by the order
    if ( currentLine.endsWith("<order>"))
    {
      // order is beginning. Clear the code string and say we will get an order
      readingCode = true;
      code = "";
      return;// break out of the loop so this character isn't added to the order
    }
    // if the current line ends with <sudorder>, it's a sudo order used to debug the system
    else if(currentLine.endsWith("<sudorder>"))
    {
       // order is beginning. Clear the code string and say we will get an order
       readingSudo = true;
       code = "";
       return; //break the loop so the > character won't be added to the code
    }
    // if the current line ends with <swapzone>, it's a order to change the zone of a specific light
    else if(currentLine.endsWith("<swapzone>"))
    {
       // order is beginning. Clear the code string and say we will get an order
       readingSwap = true;
       code = "";
       return; //break the loop so the > character won't be added to the code
    }

    // if you're currently reading the bytes of an order, add them to the code String:
    if (readingCode)
    {
      // if you got a "!" character, you've reached the end of an order
      if (c != '!') //The order continue
      {
        code += c;
      }
      else         //the order 66 is given !!!
      {
        if(readingZone) //if we are reading the zone of the order, we put it in a temp
        {
          currentZone = code;
        }
        else           //else we are reading the order and can send to the library the whole information
        {
          dispositif.Analyse(currentZone, code, zone);
          //Serial.println("retour");     //debug code
          //send to xbee the current order
          Serial.print(currentZone);
          Serial.print("!");
          Serial.print(code);
          Serial.println("!");
          currentZone = "";             //reset the temp data
        }
        readingZone = !readingZone;     //be sure we will read the zone than the order
        code = "";                      //refresh the temp data
      }
      if (c == '<') //The order list is ended
      {
        readingCode = false;              //we aren't reding order anymore
        //Serial.println("Fin des codes");  //debug code
      }
    }

    //if you are reading a sudo order
    else if (readingSudo)
    {
      //digitalWrite(9,HIGH); //put the test led from the wifi shield at HIGH to see when we got an order
      if (c != '<') //The order continue
      {
        code += c;
      }
      else         //the order 66 is given !!!
      {
        // if you got a "<" character, you've reached the end of the order
        readingSudo = false;   //we aren't reding order anymore
        //Serial.println(code);  //we send the order to debug the code
        dispositif.Action(code);          //we do the action ask by the sudo
        Serial.print(code);
        Serial.println("$");
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
    //Serial.println();
    //Serial.println("disconnecting from server.");
    //We stop the communication with the client
    client.stop();
    //we wait before retry cause we don't want to ddos the website
    //delay(1);
    //and try another connection
    connectToData();
  }
}

//usefull to get informations of the connection
void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

//connect to the web site and get the page reserved for the arduino
void connectToData()
{
  //Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 80))
  {
    //Serial.println("connected to server");
    // Make a HTTP request from this page
    client.print("GET /IDP/index.php?section=Ordre&id=");
    client.println(id);
    client.println("Host: 192.168.0.101");
    // And than close the connection.
    client.println("Connection: close");
    client.println();
  }
}

void swapingZone(char c)
{
  // if you got a "!" character, you've reached the end of an order part
  if (c != '!') //The order continue
  {
     code += c;
  }
  else         //the order 66 is given !!!
  {
    if(readingID) //if we are reading the zone of the order, we put it in a temp
    {
      networkID = code; //we save the fisrt part
      readingID = false;//update information about which part we read
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
          zone = code.toInt();  //we change the zone of this node
        }
        else  //it's the node of a xbee than we send them the order
        {
          //send to xbee the current order
          Serial.print(NodeID);
          Serial.print("#");
          Serial.print(code);
          Serial.println("#");
        }
      }
      readingID = true;
      networkID = "";             //reset the temp data
      NodeID = "";
    }
    code = "";                      //refresh the temp data
  }
  if (c == '<') //The order list is ended
  {
    readingSwap = false;              //we aren't reding order anymore
  }
}
