/*****************
 *
 *
 * - kasper
 */


#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <MCP23017.h>
#include <SkaarhojPgmspace.h>
#include <SkaarhojSmartSwitch2.h>
SkaarhojSmartSwitch2 board;




void setup() {
  Serial.begin(115200);

  Wire.begin();

  board.begin(4);  // SETUP: Board address

  // Setting full brightness (range 0-7) of all buttons:
  board.setButtonBrightness(7, B11111111);
  board.setButtonBrightness(7, B11111111);

  // Setting white color for all buttons:
  board.setButtonColor(3, 3, 3, B11111111);

  // Init done
  board.clearDisplay();   // clears the screen and buffer
  board.display(B11111111);  // Write to all

  board.setRotation(0);

  // text display tests
  board.setTextSize(1);
  board.setTextColor(WHITE);
  board.setCursor(0, 0);
  board.println("Hello, world!");
  board.setTextColor(BLACK, WHITE); // 'inverted' text
  board.println(3.141592);
  board.setTextSize(2);
  board.setTextColor(WHITE);
  board.print("0x"); board.println(0xDEADBEEF, HEX);
  board.display(B11111111);
  delay(2000);


}

void loop() {
  static uint16_t lastTime;
  static uint8_t bgColor = 0;

  for (int i = 0; i < 8; i++)  {
    if (board.buttonDown(i+1))	{
      bgColor = random(0, 63);
      board.setButtonColor(bgColor >> 4, bgColor >> 2, bgColor, B1 << i);

      board.clearDisplay();   // clears the screen and buffer
      board.setTextColor(WHITE);
      board.setCursor(0, 0);
      board.setTextSize(1);
      board.print("Display #");
      board.println(i + 1);
      board.print("Col=");
      board.print((bgColor >> 4)&B11);
      board.print(",");
      board.print((bgColor >> 2)&B11);
      board.print(",");
      board.print((bgColor)&B11);
      board.display(B1 << i);
    }
  }
}





