/*
  Testing noise on Arduino Analog input with a slider or T-bar connected.
  This sketch doesn't use the SkaarhojUtils library, but might come in handy 
  when assessing the noise on an analog input which can inform setting the right 
  parameters for a slider / T-bar
 */

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
}

int prevSensorValue;
int recentDiffValues[40];
uint16_t recentDiffValuesCounter = 0;

void loop() {
  // Read the input on analog pin 0:
  int sensorValue = analogRead(A0);  // Change this to the analog input pin used by the T-bar/Slider (if different)

  // Print out the value you read:
  Serial.print(sensorValue);

  // Print padding:
  if (sensorValue<1000)  {
    Serial.print(" ");
  }
  if (sensorValue<100)  {
    Serial.print(" ");
  }
  if (sensorValue<10)  {
    Serial.print(" ");
  }

  Serial.print(": ");


  // Min/Max:
  int maxValue=0;
  int minValue=0;
  for(int i=0; i<40; i++)  {
    if (recentDiffValues[i] > maxValue)  {
        maxValue = recentDiffValues[i];
    }
    if (recentDiffValues[i] < minValue)  {
        minValue = recentDiffValues[i];
    }
  }
  Serial.print("[ ");
  Serial.print(minValue);
  Serial.print(" | ");
  Serial.print(maxValue);
  Serial.print(" ] ");
  
  if (minValue>-1000)  {
    Serial.print(" ");
  }
  if (minValue>-100)  {
    Serial.print(" ");
  }
  if (minValue>-10)  {
    Serial.print(" ");
  }
  if (minValue==0)  {
    Serial.print(" ");
  }
  if (maxValue<1000)  {
    Serial.print(" ");
  }
  if (maxValue<100)  {
    Serial.print(" ");
  }
  if (maxValue<10)  {
    Serial.print(" ");
  }

  Serial.print(" ");
  

  // Meter:
  Serial.print(sensorValue-prevSensorValue+15+1 <= 0 ? "!" : " ");
  for(int i=15; i>0; i--)  {
    Serial.print(sensorValue-prevSensorValue+i <= 0 ? "=" : " ");
  }

  Serial.print("|");

  for(int i=1; i<=15; i++)  {
    Serial.print(sensorValue-prevSensorValue-i >= 0 ? "=" : " ");
  }
  Serial.print(sensorValue-prevSensorValue-15-1 >= 0 ? "!" : " ");

  Serial.print(" (");
  Serial.print(sensorValue-prevSensorValue);
  Serial.print(" )");
  Serial.println("");

  recentDiffValues[(recentDiffValuesCounter%40)]=sensorValue-prevSensorValue;
  recentDiffValuesCounter++;

  prevSensorValue = sensorValue;
  
  delay(20);        // delay in between reads for stability
}


