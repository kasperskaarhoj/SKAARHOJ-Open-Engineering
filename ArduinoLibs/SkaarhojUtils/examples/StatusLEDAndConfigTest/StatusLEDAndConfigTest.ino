/*****************
 * Touch Power Management Test
 * 
 * - kasper
 */

  // Pins for Arduino Mega Extended Breakout Shield:
int greenLEDPin = 22;
int redLEDPin = 23;





// no-cost stream operator as described at 
// http://arduiniana.org/libraries/streaming/
template<class T>
inline Print &operator <<(Print &obj, T arg)
{  
  obj.print(arg); 
  return obj; 
}

bool isConfigMode;

void setup() { 

    
  // Start the Ethernet, Serial (debugging) and UDP:
  Serial.begin(9600);
  Serial << F("\n- - - - - - - -\nSerial Started\n");  

  pinMode(A1,INPUT_PULLUP);
  delay(100);
  isConfigMode = (analogRead(A1) > 500) ? false : true;
  
  Serial << F("Config Mode: ") << isConfigMode << "\n";  
    
  // Sets the Bi-color LED to off = "no connection"
  digitalWrite(redLEDPin,false);
  digitalWrite(greenLEDPin,false);
  pinMode(redLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);
}


void loop() {

  Serial << "RED\n";
  digitalWrite(greenLEDPin,false);
  digitalWrite(redLEDPin,true);
  delay(1000);

  Serial << "GREEN\n";
  digitalWrite(greenLEDPin,true);
  digitalWrite(redLEDPin,false);
  delay(1000);

  Serial << "ORANGE\n";
  digitalWrite(greenLEDPin,true);
  digitalWrite(redLEDPin,true);
  delay(1000);

  Serial << "Off\n";
  digitalWrite(greenLEDPin,false);
  digitalWrite(redLEDPin,false);
  delay(1000);
}

