// This is the code that was uploaded to the ESP8266. It decodes a value that it receieves through Serial. 
// It sends this information to the LED matrix.
// It connects the ESP8266 to a cellphone. It controls the LED matrix through WIFI communication.

// Libraries
#include "math.h"
// We utilized the math library by Arduino
#include <Wire.h>
// We utilized the Wire library by Arduino
#include <Adafruit_GFX.h>
// We utilized the Adafruit_GFX library by Adafruit
#include "Adafruit_LEDBackpack.h"
// We utilized the Adafruit_LEDBackpack library by Adafruit
#include <ESP8266WiFi.h>
// We utilized the ESP8266WiFi library by earlephilhower
// https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi
#include <BlynkSimpleEsp8266.h>
// We utilized BlynkSimpleEsp8266 library by Blynk
// https://github.com/blynkkk/blynk-library


// Variables
#define BLYNK_PRINT Serial
#define SAMPLES 128             
#define SAMPLING_FREQUENCY 9999
#define WIDTH 8

int incomingByte = 0;   // For incoming serial data
int incomingByte2 = 0;   // For incoming serial data
int incomingByte3 = 0;   // For incoming serial data

int ori = 2; // Orientation
boolean state = true; // ON or OFF

// Adafruit LED Matrix
uint16_t bottom_x0 = 0;
uint16_t bottom_x1 = 1;
uint16_t bottom_x2 = 2;
uint16_t bottom_x3 = 3;
uint16_t bottom_x4 = 4;
uint16_t bottom_x5 = 5;
uint16_t bottom_x6 = 6;
uint16_t bottom_x7 = 7;

uint16_t bottom_y0 = 0;
uint16_t bottom_y1 = 0;
uint16_t bottom_y2 = 0;
uint16_t bottom_y3 = 0;
uint16_t bottom_y4 = 0;
uint16_t bottom_y5 = 0;
uint16_t bottom_y6 = 0;
uint16_t bottom_y7 = 0;

uint16_t top_x0 = 0;
uint16_t top_x1 = 1;
uint16_t top_x2 = 2;
uint16_t top_x3 = 3;
uint16_t top_x4 = 4;
uint16_t top_x5 = 5;
uint16_t top_x6 = 6;
uint16_t top_x7 = 7;

// These are the values that will change depending on the FFT
uint16_t top_y0 = 0;
uint16_t top_y1 = 0;
uint16_t top_y2 = 0;
uint16_t top_y3 = 0;
uint16_t top_y4 = 0;
uint16_t top_y5 = 0;
uint16_t top_y6 = 0;
uint16_t top_y7 = 0;

Adafruit_8x8matrix matrix = Adafruit_8x8matrix();
char auth[] = "e1d72bf1e7654794bf2486db653a3e9a"; // For WIFI

// Your WIFI credentials
// Set password to "" for open networks
char ssid[] = "Lindsey";
char pass[] = "12345678";
void setup() {
        pinMode(LED_BUILTIN,OUTPUT);
        Serial.begin(115200);     
        matrix.begin(0x70); 
        digitalWrite(LED_BUILTIN,HIGH);
        Blynk.begin(auth, ssid, pass);
        digitalWrite(LED_BUILTIN,LOW);
}

void loop() {
        Blynk.run();
        uint8_t buf[3]={0,0,0};
        // Send data when data is received
        if (Serial.available() > 0) 
        {
          // Read value
          Serial.readBytes(buf,3);
          // Value
          Serial.print("I received: ");
          
          int i=(int)buf[0];
          int j=(int)buf[1];
          int k=(int)buf[2];
          int result=i+256*j+65536*k; // Decode
          Serial.println(result, DEC); // Debug
          if(state) // ON or OFF
          {
            magicNumberDecoder(result);
          }
          else
          {
            matrix.clear();
            matrix.writeDisplay();
          }
        }       
}
void magicNumberDecoder(int magicNumber)
{
  int numbers[WIDTH];
  for(int i=0;i<WIDTH;i++)
  {
    numbers[i]=((magicNumber>>3*(7-i))&7); // Decode
    Serial.print(numbers[i]);
    Serial.print(" ");
  }
  Serial.println();
  top_y7 = numbers[0];
  top_y6 = numbers[1];
  top_y5 = numbers[2];
  top_y4 = numbers[3];
  top_y3 = numbers[4];
  top_y2 = numbers[5];
  top_y1 = numbers[6];
  top_y0 = numbers[7];
  
  matrix.clear(); // Clear matrix
  
  // Matrix Routing
  // First Column 
  matrix.drawLine(bottom_x0, bottom_y0, top_x0, top_y0, LED_ON);

  // Second Column
  matrix.drawLine(bottom_x1, bottom_y1, top_x1, top_y1, LED_ON);

  // Third Column
  matrix.drawLine(bottom_x2, bottom_y2, top_x2, top_y2, LED_ON);

  // Fourth Column
  matrix.drawLine(bottom_x3, bottom_y3, top_x3, top_y3, LED_ON);

  // Fifth Column
  matrix.drawLine(bottom_x4, bottom_y4, top_x4, top_y4, LED_ON);

  // Sixth Column
  matrix.drawLine(bottom_x5, bottom_y5, top_x5, top_y5, LED_ON);

  // Seventh Column
  matrix.drawLine(bottom_x6, bottom_y6, top_x6, top_y6, LED_ON);

  // Eighth Column
  matrix.drawLine(bottom_x7, bottom_y7, top_x7, top_y7, LED_ON);

  // Write
  matrix.writeDisplay();
  matrix.setRotation(ori);
}
// Control state via Blynk
BLYNK_WRITE(V0)
{
  if(param.asInt())
  {
    state=true;
  }
  else
  {
    state=false;
  }
}
// Control Orientation via Blynk
BLYNK_WRITE(V1)
{
  ori=param.asInt();
}
