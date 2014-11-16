#include <Dispositif.h>
#include <EEPROM.h>

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define pin1 5
#define pin2 6
#define pin3 9

Dispositif::Dispositif()
{
  pinMode(pin1, OUTPUT); // absolutely necessary because we use a custom PWM
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);

  setBrightness(0, 50);
  setBrightness(1, 50);
  setBrightness(2, 50);

  zone = EEPROM.read(0);
  id = EEPROM.read(1);
  idNode = EEPROM.read(2);
}

Dispositif::Dispositif(uint8_t zone, uint8_t id, uint8_t idNode)
{
  zone = zone;
  id = id;
  idNode = idNode;

  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);

  setBrightness(0, 50);
  setBrightness(1, 50);
  setBrightness(2, 50);

  EEPROM.write(0, zone);
  EEPROM.write(1, id);
  EEPROM.write(2, idNode);
}

/*void Dispositif::Action(String action)
{
  if(action == "jour")             // Turn the light on
  {
    digitalWrite(pin1, HIGH);
    ledOn = true;
  }
  else if(action == "nuit")        // Turn the light off
  {
    digitalWrite(pin1, LOW);
    ledOn = false;
  }
  else if(action == "harlem")      // Harlem Shake
  {
    uint8_t i = 3;
    uint8_t j = 10;
    while(i > 0)
    {
        while(j > 0)
        {
            digitalWrite(pin1, HIGH);
            delay(j * 8);
            digitalWrite(pin1, LOW);
            delay(j * 8);
            j -= 1;
        }
        while(j < 10)
        {
            digitalWrite(pin1, HIGH);
            delay(j * 8);
            digitalWrite(pin1, LOW);
            delay(j * 8);
            j += 1;
        }

        i -= 1;
    }
  }
  else if(action == "blink")	// LED blinks at the speed of page refresh
  {
     if(ledOn)
       digitalWrite(pin1, LOW);
     else
       digitalWrite(pin1, HIGH);

	 ledOn = !ledOn;
  }
}*/

void Dispositif::setBrightness(uint8_t number, uint8_t value)
{
    uint8_t last_val = OCR0A;
    
    while(value - last_val > 25)
    {
        last_val = last_val + 25;
        setPWM(number, last_val);
        delay(1000);
    }
    setPWM(number, value);
}

void Dispositif::setPWM(uint8_t number, uint8_t val)
{
    if (1 == number) // pin 6
    {
      sbi(TCCR0A, COM0A1);
      OCR0A = last_value; // set pwm duty
    }
    else if (2 == number) // pin 9
    {
      sbi(TCCR1A, COM1A1);
      OCR1A = last_value; // set pwm duty
    }
    else // pin 5
    {
      sbi(TCCR0A, COM0B1);
      OCR0B = last_value; // set pwm duty
    }
}

/*void Dispositif::parseMessage(String code)
{
  if(orderZone)
  {
    //Serial.println(code);
    //int temp = code.toInt();
    // we check we are in the good zone, if it's true, the next part can be read
    if(code == zone)
    {
      zoneMatch = true;
    }
    else
    {
      zoneMatch = false;
    }
    orderZone = false;
  }
  else
  {
    orderZone = true;
    // if we are in the right zone, we can update the brightness
    if(zoneMatch)
    {
       //int p = code.toInt();  // Code is a string
       setBrightness(0, code.toInt());
    }
  }
}*/

void Dispositif::changeZone(String newZone)
{
  zone = newZone;
}

/*void Dispositif::digitalWriteC(uint8_t pin, uint8_t val)
{
    uint8_t timer = digitalPinToTimer(pin);
	uint8_t bit = digitalPinToBitMask(pin);
	uint8_t port = digitalPinToPort(pin);
	volatile uint8_t *out;

	out = portOutputRegister(port);

	uint8_t oldSREG = SREG;
	cli();

	if (val == LOW) {
		*out &= ~bit;
	} else {
		*out |= bit;
	}

	SREG = oldSREG;
}*/
