//library usefull for an internet connection.
#include <SPI.h>
#include <WiFi.h>
//library used for the light control.
#include <Dispositif.h>

int id = 1;                           //this is the id of the arduinoServer
int idNode = 1;                       //this is the id of the arduino on the Xbee tree

Dispositif dispositif(9);            //start a new light to control it easyly
int zone = 1;                        // int to know our zone

char ssid[] = "VOO-948564";//"SatCamp";            //  your network SSID (name) VOO-948564
char pass[] = "";              // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                      // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;          //Show the current status of the WiFi shield

//IPAddress server(192,168,0,101);        // numeric IP for Google (no DNS) we can use both to connect (usefull in local test) :)
char server[] = "lumiere.lea-valbenoit.eu";  // name address for Google (using DNS) (usefull when the site will be up)

char path[] = "http://lumiere.lea-valbenoit.eu/index.php?section=index"; // the path
int port = 80; // the port, 80 for HTTP

String currentLine = "";              // string to hold the text from server
String code = "";                     // string to hold the code
boolean readingCode = false;          // bool to know if we are reading an order
boolean readingZone = true;          // bool to know if we read the zone or the order
String currentZone = "";             // tempory, zone of the current order
boolean readingSudo = false;          // bool to know if we are reading a sudo order

WiFiClient client;                    // Initialize the Ethernet client library

boolean readZone1 = true;
int pos = 0;
#define taille 20
int tab1 [taille];
int tab2 [taille];

boolean needAtab = true;
boolean readWEB = false;
boolean WEBConnected = true;

void setup()
{
  //resereve space for stings;
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

  connectToData();    //we connect to the web site page which contain the data we require.
}

void loop()
{
  // if there are incoming bytes available from the server, read them and print them:
  while(client.available())
  {
    char c = client.read();  //read the current char send from internet
    currentLine += c;        //ad the current char to the buffer string
    Serial.write(c);         //write what the arduino see

    if (c == '\n')           //if we got a new line, we clear the buffer
    {
        currentLine = "";
        //Serial.print(currentLine);
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

    // if you're currently reading the bytes of an order, add them to the code String:
    if (readingCode)
    {
      // if you got a "!" character, you've reached the end of an order
      if (c != '!') //The order continue
      {
        if (c == '#')
        {
          int TestZone = code.toInt();
          if(TestZone == 1)
          {
            readZone1 = true;
          }
          else
          {
            readZone1 = false;
          }

          code = "";
          pos = 0;
        }
        else
        {
          code += c;
        }
      }
      else         //we got a value to add to the tab depending on readZone1
      {
        if(readZone1) //if we are reading the zone of the order, we put it in a temp
        {
          int temp = code.toInt();
          tab1 [pos] = temp;
          pos += 1;
        }
        else           //else we are reading the order and can send to the library the whole information
        {
          int temp = code.toInt();
          tab2 [pos] = temp;
          pos += 1;
        }
        code = "";                      //refresh the temp data
        needAtab = false;
      }
      if (c == '<') //The order list is ended
      {
        readingCode = false;              //we aren't reding order anymore
        DEMO();  //Start the demo
        code = "";
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
    readWEB = true;
  }

  if(needAtab && (readWEB || !(WEBConnected)))
  {
    int temp1 [] = {100,0,100,0,100,0,100,0, 100,0, 100, 0, 100,0, 100,0,100,0,100,0};
    int temp2 [] = {0,25,50,75,100,75,50,50,50,0,25,50,100,100,100,100,100,100,100,100};
    for(int i = 0; i < 20; i+=1)
    {
      tab1[i] = temp1[i];
      tab2[i] = temp2[i];
    }
    DEMO();
    readWEB = false;
  }

  // if the server's disconnected
  if (!client.connected())
  {
    //Serial.println();
    //Serial.println("disconnecting from server.");
    //We stop the communication with the client
    client.stop();
    //and try another connection
    needAtab = true;
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
  if (client.connect(server, port))
  {
    //Serial.println("connected to server");
    // Make a HTTP request from this page
    client.print("GET ");
    client.println(path);
    //client.println(" HTTP/1.1");
    client.println("Host: lumiere.lea-valbenoit.eu");
    // And than close the connection
    client.println("Connection: close");
    client.println();
    Serial.println("Connected to Web Site !");
    WEBConnected = true;
  }
  else
  {
    WEBConnected = false;
  }
}

void DEMO()
{
 //we start a loop that will Do the loop of controll
  for(int i = 0; i < taille; i += 1)
  {
    dispositif.Luminaire(tab1[i]); //we put the light intensity

    Serial.print("2!");           //we send the current order to the other light
    Serial.print(tab2[i]);
    Serial.println("!");
    Serial.print("cet arduino va afficher une intensité de :");
    Serial.println(tab1[i]);
    Serial.println("++++++++++++++++++++++++++++++++++++++++++++++++++");

    delay(30000); //wait 30 sec to give the time to see the light

    //we clear the tab to be sure we show when we got a disconnection
    tab1[i] = 0;
    tab2[i] = 0;
  }
}
