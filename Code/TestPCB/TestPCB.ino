int led1 = 9;    // LED connected to digital pin 9
int led2 = 5;
int delay_time = 1000;

void setup()  { 
  // nothing happens in setup 
} 

void loop()  {
  
  // Both lamp go up in intensity
  for(int fadeValue = 25 ; fadeValue <= 255; fadeValue +=10) { 
    // sets the value (range from 0 to 255):
    analogWrite(led1, fadeValue);
    analogWrite(led2, fadeValue);
    // wait for 30 milliseconds to see the dimming effect    
    delay(delay_time);                    
  } 
  
  delay(delay_time);
  analogWrite(led1, 0);
  analogWrite(led2, 0);
  delay(delay_time);
  delay(delay_time);
  delay(delay_time);
  delay(delay_time);
  
  // Both lamps go down in intensity
  for(int fadeValue = 255 ; fadeValue >= 25; fadeValue -=10) { 
    // sets the value (range from 0 to 255):
    analogWrite(led1, fadeValue);
    analogWrite(led2, fadeValue);     
    // wait for 30 milliseconds to see the dimming effect    
    delay(delay_time);                            
  } 
  
  delay(delay_time);
  analogWrite(led1, 255);
  analogWrite(led2, 255);
  delay(delay_time);
  analogWrite(led1, 0);
  analogWrite(led2, 0);
  delay(10000);
  
  // One goes up while the other goes down
  for(int fadeValue = 25 ; fadeValue <= 255; fadeValue +=10) { 
    // sets the value (range from 0 to 255):
    analogWrite(led1, fadeValue);
    analogWrite(led2, 255 - fadeValue);  
    // wait for 30 milliseconds to see the dimming effect    
    delay(delay_time);                    
  }
  
  analogWrite(led1, 255);
  analogWrite(led2, 255);
  delay(delay_time);
  analogWrite(led1, 0);
  analogWrite(led2, 0);
  delay(delay_time);

  // Same as before, only lamps' roles are inverte
  for(int fadeValue = 255 ; fadeValue >= 25; fadeValue -=10) { 
    // sets the value (range from 0 to 255):
    analogWrite(led1, fadeValue);
    analogWrite(led2, 255 - fadeValue);     
    // wait for 30 milliseconds to see the dimming effect    
    delay(delay_time);                            
  } 
  
}


