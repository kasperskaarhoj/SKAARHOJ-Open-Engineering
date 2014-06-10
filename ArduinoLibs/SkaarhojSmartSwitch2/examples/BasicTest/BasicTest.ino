/*****************
 * 
 * 
 * 
 * 
 * 
 * - kasper
 */


#include <Wire.h>
#include <Adafruit_GFX.h>
#include <MCP23017.h>

#include <SkaarhojSmartSwitch2.h>
SkaarhojSmartSwitch2 board;




static unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };
  
// width x height = 64,32
static uint8_t imageVarName[] PROGMEM = {
	B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, 
	B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, 
	B00000000, B00000000, B00000000, B00000000, B00111000, B00000000, B00000000, B00000000, 
	B00000000, B00000000, B00000000, B00000001, B11111000, B00000000, B00000000, B00000000, 
	B00000000, B00000000, B00000000, B00000011, B11111000, B00000000, B00000000, B00000000, 
	B00000000, B00000000, B00000000, B00011111, B11111111, B00000000, B00000000, B00000000, 
	B00000000, B00000000, B00000000, B00011111, B11111111, B00000000, B00000000, B00000000, 
	B00000000, B00000000, B00000000, B00011111, B11111111, B00000000, B00000000, B00000000, 
	B00000011, B10000000, B00000000, B11111111, B11111111, B00000000, B00000000, B00000000, 
	B00000011, B11111111, B11000000, B11111111, B11111111, B11100000, B00000001, B11000000, 
	B00000011, B11111111, B11000000, B11111111, B11111111, B11100000, B00000001, B11000000, 
	B00000011, B11111111, B11000111, B11111111, B11111111, B11100000, B00001111, B11000000, 
	B00000011, B11111111, B11000111, B11111111, B11111111, B11100000, B01111111, B11000000, 
	B00000011, B11111111, B11000111, B11111111, B11111111, B11100000, B01111111, B11000000, 
	B00000011, B11111111, B11000111, B11111111, B11111111, B11100011, B11111111, B11000000, 
	B00000011, B11111110, B00111111, B11111111, B11111111, B11100011, B11111111, B11000000, 
	B00000011, B11111110, B00111111, B11111111, B11111111, B11100011, B11111111, B11000000, 
	B00000011, B11111110, B00111111, B11111111, B11111111, B11111111, B11111111, B11000000, 
	B00000011, B11111110, B00111111, B11111111, B11111111, B11111111, B11111111, B11000000, 
	B00000011, B11111110, B00111111, B11111111, B11111111, B11111111, B11111111, B11000000, 
	B00000000, B01111110, B00111111, B11111111, B11111111, B11111111, B11111111, B11000000, 
	B11100000, B01111110, B00111111, B11111111, B11111111, B11100111, B11111111, B11000000, 
	B11100000, B01111110, B00111111, B11111111, B11111111, B11100011, B11111111, B11000000, 
	B00011100, B01111110, B00111111, B11111111, B11111111, B11100011, B11111110, B00000000, 
	B00011100, B01111111, B11000111, B11111111, B11111111, B11100011, B11111110, B00000000, 
	B00011100, B01111111, B11000111, B11111111, B11111111, B11100011, B11111110, B00000000, 
	B00011111, B10001111, B11000111, B11111111, B11111111, B11111111, B11110000, B00000000, 
	B00000011, B10001111, B11000111, B11111111, B11111111, B00111111, B10000000, B00111000, 
	B00000011, B10001111, B11000111, B11111111, B11111111, B00011111, B10000000, B00111000, 
	B00000011, B11110001, B11000111, B11111111, B11111111, B00011100, B01111111, B11000000, 
	B00000000, B01111110, B00111000, B11111111, B11111000, B11100011, B11111111, B11000000, 
	B00000000, B01111110, B00111000, B11111111, B11111000, B11100011, B11111111, B11000000, 
	
};  


void setup() {
  Serial.begin(9600);
  
  Wire.begin();

  
  
  // By default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  board.begin(1);

  // Setting full brightness (range 0-7) of all buttons:
  board.setButtonBrightness(7, B11111111);
  board.setButtonBrightness(7, B11111111);

  // Setting white color for all buttons:
  board.setButtonColor(3,3,3,B11111111);

  // Init done
  board.clearDisplay();   // clears the screen and buffer
  board.display(B11111111);  // Write to all

  board.setRotation(0);

  // text display tests
  board.setTextSize(1);
  board.setTextColor(WHITE);
  board.setCursor(0,0);
  board.println("Hello, world!");
  board.setTextColor(BLACK, WHITE); // 'inverted' text
  board.println(3.141592);
  board.setTextSize(2);
  board.setTextColor(WHITE);
  board.print("0x"); board.println(0xDEADBEEF, HEX);
  board.display(B11111111);
  delay(2000);
  
  
    // miniature bitmap display
  board.clearDisplay();
  board.drawBitmap(30, 16,  logo16_glcd_bmp, 16, 16, 1);
  board.display(B11111111);


  delay(5000);


    // miniature bitmap display
  board.clearDisplay();
  board.drawBitmap(0, 0,  imageVarName, 64, 32, 1);
  board.display(B11111111);

  delay(5000);
  
}
unsigned long lastTime[4];

void loop() {  
  
  for(int i=0; i<8; i++)  {
    board.clearDisplay();   // clears the screen and buffer
    board.setTextSize(1);
    board.setTextColor(WHITE);
    board.setCursor(0,0);
    board.println(millis());
    board.setTextSize(2);
    board.println(millis()-lastTime[i]);
    lastTime[i] = millis();
    board.display(B1 << i);


    if (board.buttonDown(i))  {
      Serial.print("Button #");
      Serial.print(i);
      Serial.print(" was pressed down");
    }  
    if (board.buttonUp(i))  {
      Serial.print("Button #");
      Serial.print(i);
      Serial.println(" was released");
    }  
    
    if (board.buttonIsHeldFor(1, 1000))  {
      Serial.println("1 was held for 1000 ms");
    }
    if (board.buttonIsHeldFor(2, 3000))  {
      Serial.println("2 was held for 3000 ms");
    }
    if (board.buttonIsReleasedAgo(1, 1000))  {
      Serial.println("1 was released 1000 ms ago");
    }
    if (board.buttonIsReleasedAgo(2, 2000))  {
      Serial.println("2 was released 2000 ms ago");
    }

  }
}

