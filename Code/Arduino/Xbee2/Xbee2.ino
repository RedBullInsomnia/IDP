#include <EEPROM.h>
#include <Dispositif.h>

// how much serial data we expect before a newline
const uint8_t MAX_INPUT = 196;

//zone = 1, id = 1, idNode = 2
Dispositif dispositif;

void setup ()
{
  Serial.begin(9600);
  //delay(1000);
  Serial.println("Cool");
} // end of setup

// here to process incoming serial data after a terminator received
void process_data (const char * data, unsigned int input_pos)
{
  uint8_t value = 0;
  uint8_t i = 1;
  uint8_t currentZone;
  char c;
  
  if(data[0] != '<')
  {
    i = peekNextP(data);
  }
  while (data[i] != '>' && i < input_pos - 3)
  {
    // zone
    value = 0;
    i = peekNextDigit(data, i);
    do{
      c = data[i];
      i++;
      if(c >= '0' && c <= '9')        // is c a digit?
        value = value * 10 + c - '0';
    } while(c >= '0' && c <= '9');
    currentZone = value;
    //Serial.print(currentZone);
    //Serial.print(F("("));
    
    // 1st order
    value = 0;
    i = peekNextDigit(data, i);
    do{
      c = data[i];
      i++;
      if(c >= '0' && c <= '9')        // is c a digit?
        value = value * 10 + c - '0';
    } while(c >= '0' && c <= '9');
    //Serial.print(value);
    //Serial.print(F(","));
    
    if (currentZone == dispositif.zone) {
      dispositif.setBrightness(0, value);
    }
    // 2nd order
    value = 0;
    i = peekNextDigit(data, i);
    do{
      c = data[i];
      ++i;
      if(c >= '0' && c <= '9')        // is c a digit?
        value = value * 10 + c - '0';
    } while(c >= '0' && c <= '9');
    Serial.print(value);
    Serial.print(F(","));
    
    if (currentZone == dispositif.zone)
      dispositif.setBrightness(1, value);
      
    // 3nd order
    value = 0;
    i = peekNextDigit(data, i);
    do{
      c = data[i];
      ++i;
      if(c >= '0' && c <= '9')        // is c a digit?
        value = value * 10 + c - '0';
    } while(c >= '0' && c <= '9');
    Serial.print(value);
    Serial.println(F(")"));
    
    if (currentZone == dispositif.zone)
      dispositif.setBrightness(2, value);
  }
  
}  // end of process_data

/*uint8_t parseIntC(const char* data, uint8_t i)
{
  uint8_t value = 0;
  uint8_t c;
  
  i = peekNextDigit(data, i);
  do{
    c = data[i];
    i++;
    if(c >= '0' && c <= '9')        // is c a digit?
      value = value * 10 + c - '0';
  }
  while(c >= '0' && c <= '9');

  return value; 
}*/

uint8_t peekNextDigit(const char* data, uint8_t i)
{
  int c;
  while (1) {
    c = data[i];
    if (c >= '0' && c <= '9') return i;
    ++i;
  }
}

uint8_t peekNextP(const char* data)
{
  uint8_t c;
  uint8_t i = 0;
  while (1) {
    c = data[i];
    ++i;
    if (c == ')') return i;
  }
}

void processIncomingByte (const byte inByte)
{
  static char input_line [MAX_INPUT];
  static unsigned int input_pos = 0;
  
  switch (inByte)
  {
    case '\n':   // end of text
      input_line [input_pos] = 0;  // terminating null byte
      
      // terminator reached! process input_line here ...
      process_data (input_line, input_pos);
      
      // reset buffer for next time
      input_pos = 0;  
      break;
    
    case '\r':   // discard carriage return
      break;
    
    default:
      // keep adding if not full ... allow for terminating null byte
      if (input_pos < (MAX_INPUT - 1))
        input_line [input_pos++] = inByte;
      break;
  
  }  // end of switch
 
} // end of processIncomingByte  

void loop()
{
  
  //Serial.println("Coucou");
  // if serial data available, process it
  if (Serial.available () > 0)
    processIncomingByte (Serial.read ());
    
}  // end of loop
