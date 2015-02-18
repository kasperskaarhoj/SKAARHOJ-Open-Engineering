#include <MemoryFree.h>
#include <Streaming.h>
#include <SkaarhojTools.h>

SkaarhojTools sTools(0);

void setup() {
  Serial.begin(115200);
}

void loop() {

  static unsigned long timer = millis();
  if (sTools.hasTimedOut(timer,1000,true))  {  // timeout of 1000 ms
    //timer = millis()+1000;  // Reset the timer (next trigger is this value + the timeout value of 1000 ms). So setting it to millis()+1000 will actually make the delay 2000 ms. However, we don't have to call this because hasTimedOut() will update the timer variable itself when the 3rd argument is true
    
    Serial.println(millis());   
    delay(10);
  }
}




