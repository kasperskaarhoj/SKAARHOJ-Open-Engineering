#include <MemoryFree.h>

// Reported free memory with str commented out:
// 1848 bytes
//
// Reported free memory with str and Serial.println(str) uncommented:
// 1834
//
// Difference: 14 bytes (13 ascii chars + null terminator)

// 14-bytes string
//char str[] = "Hallo, world!";


void setup() {
    Serial.begin(115200);
}


void loop() {
    //Serial.println(str);

    Serial.print("freeMemory()=");
    Serial.println(freeMemory());

    delay(1000);
}