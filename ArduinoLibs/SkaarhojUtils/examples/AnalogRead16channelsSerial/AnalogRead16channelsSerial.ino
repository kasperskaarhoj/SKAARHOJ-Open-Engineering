
#include <Streaming.h>

void setup() {

  Serial.begin(115200);

  Serial << F("\n   A0   A1   A2   A3   A4   A5   A6   A7   A8   A9  A10  A11  A12  A13  A14  A15\n");
}

void loop() {


  Serial 
    << _DECPADL(analogRead(A0),5," ")
    << _DECPADL(analogRead(A1),5," ")
    << _DECPADL(analogRead(A2),5," ")
    << _DECPADL(analogRead(A3),5," ")
    << _DECPADL(analogRead(A4),5," ")
    << _DECPADL(analogRead(A5),5," ")
    << _DECPADL(analogRead(A6),5," ")
    << _DECPADL(analogRead(A7),5," ")
    << _DECPADL(analogRead(A8),5," ")
    << _DECPADL(analogRead(A9),5," ")
    << _DECPADL(analogRead(A10),5," ")
    << _DECPADL(analogRead(A11),5," ")
    << _DECPADL(analogRead(A12),5," ")
    << _DECPADL(analogRead(A13),5," ")
    << _DECPADL(analogRead(A14),5," ")
    << _DECPADL(analogRead(A15),5," ")
    << F("\n");

  delay(100);
}
