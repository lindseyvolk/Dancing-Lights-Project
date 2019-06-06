// This is the code that was uploaded to the Arduino Due. This code computes the FFT of the input signal.

// Libraries
#include "arduinoFFT.h" 
// We have utilized the arduinoFFT library by Enrique Condes
#include "math.h"
// We have utilized the math library by Arduino
#include <Wire.h>
// We have utilized the Wire library by Arduino
#include <Adafruit_GFX.h>
// We have utilized the Adafruit_GFX library by Adafruit

// Variables
#define INTERVAL 16
#define ARRAYSAMPLE 8
#define SAMPLES 128 // This value must be a power of 2
#define SAMPLING_FREQUENCY 9999 // This value must be less than 10,000 due to ADC
#define LOG20K 9.90349 // ln(20000)
#define analogpin A0
#define WIDTH 8
const int sampleWindow = 50; // Sample window width in ms (50 ms = 20 Hz)
unsigned int sample;
 
arduinoFFT FFT = arduinoFFT(); // Creates an FFT sample
 
unsigned int sampling_period_us;
unsigned long microseconds;
 
double vReal[SAMPLES];
double vImag[SAMPLES]; // We do not utilize this for our project
int sens = 1;

void setup() { 
    Serial.begin(9600); // For communication with the ESP8266
    Serial2.begin(115200); // For debugging                                                                                                                                                                              
    sampling_period_us = round(1000000*(1.0/SAMPLING_FREQUENCY)); // Time intervals
    pinMode(analogpin, INPUT); 
}
 
void loop() {

    for(int i=0; i<SAMPLES; i++)
    {
        microseconds = micros();

        vReal[i] = analogRead(analogpin);
        //Serial.println(vReal[i],1);
        vImag[i] = 0;
     
        while(micros() < (microseconds + sampling_period_us)){
        }
    }
// Computation of the FFT
    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
    double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);
 
 
     for(int i=0; i<(SAMPLES/2); i++) // DC OFFSET SO THERE IS A BIG PEAK AT ZERO
    {         
        //Serial.print((i * 1.0 * SAMPLING_FREQUENCY) / SAMPLES, 1);
        //Serial.print(" ");
        //Serial.println(vReal[i], i);
    } 
    delay(125); // Repeat 12.5 times a second
    int input = vRealToBit(vReal); 
} 

// This function encodes a value that is being sent to the ESP8266

int vRealToBit(double* input)
{
  double logVReal[SAMPLES];
  double averages[WIDTH];
  int toMerge[WIDTH];
  
  
  for(int i=0; i<(SAMPLES/2); i++)
  {
    logVReal[i]=log(input[i]);//take log
    // Trim the values so that they lie between 0 and 8 since the majority of the data falls into this area (95%)
    if(logVReal[i]>8.00)
    {
      logVReal[i]=7.99;
    }
    else if(logVReal[i]<0.00)
    {
      logVReal[i]=0.01;
    }
    // Averaging the amplitudes of the sections
    for(int j=0;j<SAMPLES/WIDTH/2;j++)
    {
      averages[j]=0;
      for(int k=0;k<WIDTH;k++)
      {
        averages[j]+=(logVReal[8*j+k]/WIDTH);
      }
    }
  }
  // Encoding the value
  // 24 bit number where every 3 bits represents how many lights are ON in a column
  int magicNumber=0;
  for(int i=0; i<WIDTH; i++)
  {
    double x=averages[i];
    if(x<2.00)
    {
      x==2.00;
    }
    if(x>6.00)
    {
      x==6.00;
    }
    // Magnification of the data where the majority of data falls between 2 and 6
    // We remap the data so that it falls between 0 and 8 by subtracting 2 from it and multiplying it by 2
    x=(x-2)*2;
    toMerge[i]=(int)x;
    magicNumber+=(toMerge[i]<<3*(7-i));
  }
  Serial.println(magicNumber); // For debugging
  sendData(magicNumber); // Sends data to the receiver
  return 0;
}
// Sends the data to the ESP8266 using Serial
void sendData(int data)
{  
  byte buf[3];
  buf[0] = data & 255;
  buf[1] = (data >> 8 )  & 255;
  buf[2] = (data >> 16) & 255;
  Serial2.write(buf, sizeof(buf));
}
