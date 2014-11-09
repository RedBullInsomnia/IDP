#include <GSM.h>	      // GSM shield
#include <SPI.h>
#include <SdFat.h>
//#include <MemoryFree.h>
#include <Dispositif.h>	      // Our library

//+++++++++++++++++++ Modify if you change the GSM provider ++++++++++++++++++//
// PIN number of the SIM card
#define PINNUMBER "4632"

// APN information obtained from your network provider.
#define GPRS_APN "mworld.be" // replace with your GPRS APN
#define GPRS_LOGIN ""        // replace with your GPRS login
#define GPRS_PASSWORD ""     // replace with your GPRS password

//++++++++++++++++++ Data specific to the current Arduino ++++++++++++++++++++//
// Zone = 1, id = 1, idNode = 2
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
String code = "";                 // string to hold the code.
//String currentZone = "";          // temporary, zone of the current order.
//boolean readingZone = true;
//boolean readingCode = false;      // true when parsing an <order>
//uint8_t count = 0;

/* ChangeZone() variables */
/*boolean readingSwap = false;
String networkID = "";            // contains the ID of the concerned network
String NodeID = "";               // contains the node ID of the order.
boolean readingID = true;
boolean readingNode = false;*/

void setup() 
{
  // reserve space for string
  code.reserve(6);

  // Initialize serial to start the Xbee communication at 9600 bauds.
  Serial.begin(9600);

  /* 
   * Start GSM shield
   * Since it sometimes has trouble connecting, we do a little loop
   */
  boolean notConnected = true;
  while(notConnected)
  {
    if(gsmAccess.begin(PINNUMBER) == GSM_READY)
    {
      delay(1000);  // doesn't work otherwise
      if (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY)
        notConnected = false;
    }
    else
    {
      delay(2000);	    // wait for 2 secs and then retry
    }
  }
  connectToData();          // We can now retrieve the data
}

void loop()
{
  char c;
  String currentZone ="";
  
  if(client.available()) 
  {
    uint8_t cnt = 3;
    do
    {
      c = client.read();  // we read the current char sent from internet.
      code += c;   // we add the current char to the buffer string.
      cnt--;
    } while (cnt > 0);
    
    if (code.endsWith("<o>"))
    {
      while((c = client.read()) != '<')
      {
        // Zone
        currentZone="";
        while(c != '(')
        {
          currentZone += c;
          c = client.read();
        }
        Serial.print(currentZone); // print out the zone
        Serial.print(F("("));
        
        // Order #1
        code = "";
        c = client.read();
        while(c != ',')
        {
          code += c;
          c = client.read();
        }
        Serial.print(code); // print out the zone
        Serial.print(F(","));
        
        if (currentZone == dispositif.zone)
          dispositif.setBrightness(0, code.toInt());
        
        code = "";
        
        // Order #2
        c = client.read();
        while(c != ',')
        {
          code += c;
          c = client.read();
        }
        Serial.print(code); // print out the zone
        Serial.print(F(","));
        
        if (currentZone == dispositif.zone)
          dispositif.setBrightness(1, code.toInt());
        
        code = "";
        
        // Order #3
        c = client.read();
        while(c != ')')
        {
          code += c;
          c = client.read();
        }
        Serial.print(code); // print out the zone
        Serial.print(F(")"));
        
        if (currentZone == dispositif.zone)
          dispositif.setBrightness(2, code.toInt());
      }
      Serial.prinln("");
    }
  }
  
  // if the server's disconnected
  if (!client.connected()) 
  {
    client.stop();
    delay(60000); // 1 min wait
    connectToData();
  }
  
  delay(600000); // wait for 10 mins before asking for new data.
}

//connect to the website and get the page reserved for the Arduino
void connectToData()
{
  if (client.connect(server, port)) 
  {
    //Serial.println("Connected to server");
    // Make a HTTP request from this page
    client.print(F("GET "));           //we ask to get the path we ask.
    client.println(path);
    //client.print("&id="); // uncomment when Romain has finalised its code, dont forget to remove ln from live above
    //client.println(dispositif.id);// idem
    //client.println(" HTTP/1.1");
    client.print(F("Host: "));         //we show from where we want the page.
    client.println(server);
    // And then close the connection.
    client.println(F("Connection: close")); //we close the connection.

  }
}
