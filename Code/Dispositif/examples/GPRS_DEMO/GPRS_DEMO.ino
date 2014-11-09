//library usefull for an internet connection and the GSM shield.
#include <GSM.h>
//library used for the light control.
#include <Dispositif.h>


// PIN number of the sim card.
#define PINNUMBER "5667"//"5667"// TEST"3981".
// APN information obrained from your network provider.
#define GPRS_APN "mworld.be"//"mworld.be" // replace with your GPRS APN from your operator.
#define GPRS_LOGIN ""    // replace with your GPRS login.
#define GPRS_PASSWORD "" // replace with your GPRS password.

//watch Dog data.
boolean GPRSConnected = false;   //boolean that say if the GPRS is connected.
boolean WEBConnected = false;    //boolean that say if the WebSite is connected.
boolean needAtab = true;         //boolean that say if we need a pre filled tab or not.
boolean readWEB = false;         //boolean that say if we already read what come from the internet (if we connected but not on the good page).


int id = 1;                           //this is the id of the arduinoServer.
int idNode = 1;                       //this is the id of the arduino on the Xbee tree.

Dispositif lampe1(9);            //start a new light to control it easyly.
Dispositif lampe2(5);


// initialize the library instances.
GSMClient client;
GPRS gprs;
GSM gsmAccess;

//IPAddress server(192,168,0,101);           // numeric IP for our web site.
char server[] = "lumiere.lea-valbenoit.eu";  // name address for the web site we will use.

char path[] = "http://lumiere.lea-valbenoit.eu/index.php?section=DEMO"; // the path we need to collect the information.
int port = 80; // the port, 80 for HTTP.

String currentLine = "";              // string to hold the text from server.
String code = "";                     // string to hold the code.
boolean readingCode = false;          // bool to know if we are reading an order.
boolean readingZone = true;          // bool to know if we read the zone or the order.
String currentZone = "";             // tempory, zone of the current order.

boolean readingSudo = false;          // bool to know if we are reading a sudo order.

boolean readZone1 = true;      //boolean use int the DEMO to know if we reading the zone 1 or the zone 2.
int pos = 0;                   //position in the array when we encode the data.
#define taille 20              //define the length of the array.
int tab1 [taille];             //array that will contain the zone 1 datas.
int tab2 [taille];             //array that will contain the zone 2 datas.

void setup()
{
  //resereve space for strings.
  currentLine.reserve(2048);
  code.reserve(2048);

  //Initialize serial to debug the code.
  Serial.begin(9600);

  //Serial.println("Starting Arduino web client.");
  // connection state
  boolean notConnected = true;

  // After starting the modem with GSM.begin().
  // attach the shield to the GPRS network with the APN, login and password.
  int j = 0;      //we coubt the number of try of connection.
  while(notConnected)
  {
    if((gsmAccess.begin(PINNUMBER)==GSM_READY) &
      (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD)==GPRS_READY))
    {
      notConnected = false;  //we break the loop.
      GPRSConnected = true;  //we say that the arduino is connected throug this bool.
      //Serial.println("0!0!");
    }
    else
    {
      //Serial.println("Not connected");  //we say we are not connected.
      GPRSConnected = false;            //and save the bool that say it
      delay(1000);
      j += 1;                           //and add 1 to the try number.
    }
    if (j == 3)              //if we try 3 times.
    {
      notConnected = false; //we stop trying and will take a pre-filled tab.
    }
  }

  //Serial.println("connecting..."); //now we try to connect to the web site.
  if(GPRSConnected)     //if the GPRS is connected
  {
    connectToData();    //we connect to the web site page which contain the data we require.
  }
  else
  {
    WEBConnected = false; // else we say that the Arduino can't connect to the webSite.
  }
}

void loop()
{
  // if there are incoming bytes available from the server, read them and print them:
  while(client.available())
  {
    char c = client.read();  //read the current char send from internet.
    currentLine += c;        //ad the current char to the buffer string.
    //Serial.write(c);         //write what the arduino see.

    if (c == '\n')           //if we got a new line, we clear the buffer.
    {
      //Serial.print(currentLine);
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

    // if you're currently reading the bytes of an order, add them to the code String:
    if (readingCode)
    {
      // if you got a "!" character, you've reached the end of an order.
      if (c != '!') //The order continue.
      {
        if (c == '#') //we reach the end of the zone part of the order.
        {
          int TestZone = code.toInt();  //we save the zone and will see which one it's.
          if(TestZone == 1)  //if it's the zone 1, we will fill the tab1 abd than put the boolean that say it's the zone 1 at true.
          {
            readZone1 = true;
          }
          else              //else, we are reading the zone 2. we will fill the tab2 and put the same bool at false.
          {
            readZone1 = false;
          }

          code = ""; //we clear the code.
          pos = 0;   //and put the position at 0 to start at the beginning of the array.
        }
        else   //we are reading the contain of the array (or the zone) and than we add the current char to the temp.
        {
          code += c;
        }
      }
      else         //we got a value to add to the tab depending on readZone1.
      {
        if(readZone1) //if we are reading the zone of the order, we put it in a temp.
        {
          int temp = code.toInt();  //we convert the string into int.
          tab1 [pos] = temp;        //we insert it at the good position.
          pos += 1;                 //we inc the position to be at the next one.
        }
        else           //else we are reading the order and can send to the library the whole information.
        {
          int temp = code.toInt();  //we convert the string into int.
          tab2 [pos] = temp;        //we insert it at the good position.
          pos += 1;                 //we inc the position to be at the next one.
        }
        code = "";                      //refresh the temp data.
      }
      if (c == '<') //The order list is ended.
      {
        readingCode = false;    //we aren't reding order anymore.
        DEMO();                 //Start the demo.
        code = "";              //we clear all the data to be sure.
      }
    }

    //if you are reading a sudo order.
    else if (readingSudo)
    {
      //digitalWrite(9,HIGH); //put the test led from the wifi shield at HIGH to see when we got an order.
      if (c != '<') //The order continue.
      {
        code += c;
      }
      else         //the order 66 is given !!!
      {
        // if you got a "<" character, you've reached the end of the order.
        readingSudo = false;   //we aren't reding order anymore.
        //Serial.println(code);  //we send the order to debug the code.
        lampe1.Action(code);          //we do the action ask by the sudo.
        lampe2.Action(code);
        Serial.print(code);
        Serial.println("$");
      }
    }
    readWEB = true;   //we say that we already read the text from the website.
  }

  //This is the watchdog part, if we need a tab and something is not connected or if the web page don't send us what we need.
  if(needAtab & (!(WEBConnected) | readWEB | !(GPRSConnected)))
  {
    int temp1 [] = {0,25,50,75,100,100,100,75,50,100,75,50,0,25,50,75,100,75,50,25};
    int temp2 [] = {0,25,50,75,100,75,50,50,50,0,25,50,100,100,100,100,100,100,100,100};
    for(int i = 0; i < 20; i+=1) //we must do a loop to enter the data in the tab.
    {
      tab1[i] = temp1[i];
      tab2[i] = temp2[i];
    }
    DEMO(); //we launch the demo cause nobody will do it after.
    //readWEB = false;   //we clear the data.
    Serial.println("fin du watchdog");
  }
  // if the server's disconnected
  if (!client.connected())
  {
    //We stop the communication with the client.
    if(GPRSConnected)
    {
      client.stop();
      //we wait before retry cause we don't want to ddos the website.
      delay(1000);
      //and try another connection.
      connectToData();
    }
    needAtab = true;
  }
}

//connect to the web site and get the page reserved for the arduino.
void connectToData()
{
  //Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if(GPRSConnected)
  {
    if (client.connect(server, port)) 
    {
      //Serial.println("connected to server");
      // Make a HTTP request from this page
      client.print("GET ");
      client.println(path);
      //client.println(" HTTP/1.1");
      client.println("Host: lumiere.lea-valbenoit.eu");
      // And than close the connection.
      client.println("Connection: close");
      client.println();
      //Serial.println("Connected to Web Site !");
      delay(1000);
      WEBConnected = true;
    }
    else
    {
      WEBConnected = false;
    }
  }
}

void DEMO()
{
 while(1)
 {
   //we start a loop that will Do the loop of controll.
   for(int i = 0; i < taille; i += 1)
   {
     lampe1.Luminaire(tab1[i]); //we put the light intensity.
     lampe2.Luminaire(tab2[i]);
     Serial.print("1!");           //we send the current order to the other light
     Serial.print(tab1[i]);
     Serial.println("!");
     Serial.print("2!");           //we send the current order to the other light
     Serial.print(tab2[i]);
     Serial.println("!");

     delay(30000); //wait 30 sec to give the time to see the light.
     //delay(1000);
     //we clear the tab to be sure we show when we got a disconnection
     needAtab = true;
     readWEB = false;
   }
 }
}
