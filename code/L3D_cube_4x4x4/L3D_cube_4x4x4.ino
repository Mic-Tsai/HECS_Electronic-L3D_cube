// ###################################################################################
// # Project: L3D Cube 4x4x4
// # Engineer:  Mic.Tsai
// # Date:  8 July 2017
// # Objective: Dev.board
// # Usage: ATMEL328P
// ###################################################################################

#include <math.h>
#define N_PIXELS  64  // Number of pixels in strand
#define MIC_PIN   A0  // Microphone is attached to this analog pin
#define LED_PIN   A1  // NeoPixel LED strand is connected to this pin
#define SAMPLE_WINDOW   6  // Sample window for average level
#define PEAK_HANG 25 //Time of pause before peak dot falls
#define PEAK_FALL 1 //Rate of falling peak dot
#define INPUT_FLOOR 200 //Lower range of analogRead input
#define INPUT_CEILING 435 //Max range of analogRead input, the lower the value the more sensitive (1023 = max)
                          //800-->5V
                          //300-->3.3V

#include <Adafruit_NeoPixel.h>
#include "WS2812_Definitions.h"
#define PIN A1
#define LED_NUM 64
#define LED_COUNT 64

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_NUM, PIN, NEO_GRB + NEO_KHZ800);
volatile int State;
int time = 6;


byte peak = 16;      // Peak level of column; used for falling dots
unsigned int sample;

byte dotCount = 0;  //Frame counter for peak dot
byte dotHangCount = 0; //Frame counter for holding peak dot

//Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

//===============================================================
//===============================================================
//=================== Matrix Flow ===============================

int DelayTime = 10;
int Flow_ADR;

unsigned char FlowMatrix1[100]  = 
{
  5,10,15,20,25,50,75,100,125,124,123,122,121      ,
};

unsigned char FlowMatrix2[100]  = 
{
  5,30,55,80,105,104,103,102,101,106,111,116,121   ,
};

unsigned char FlowMatrix3[100]  = 
{
  5,4,3,2,1,6,11,16,21,46,71,96,121                ,
};


unsigned char FlowMatrix4[100]  = 
{
   5,10,15,20,25,24,23,22,21      ,
};

unsigned char FlowMatrix5[100]  = 
{
   5,30,55,80,105,110,115,120,125  ,
};

unsigned char FlowMatrix6[100]  = 
{
   5,4,3,2,1,26,51,76,101,106               ,
};

//=================== Matrix Flow ===============================
//===============================================================
//===============================================================

//===============================================================
//===============================================================
//=================== Snake beside Flow =========================

unsigned char FlowSnakeBeside[150]  = 
{
   1,2,3,4,5,10,15,20,25,50,75,100,125,124,123,122,121,116,111,106,101,76,51,26,1,6,11,16,21,22,23,24,25,20,15,10,5,30,55,80,105,104,103,102,101,106,
   111,116,121,96,71,46,21,22,23,24,25,20,15,10,5,4,3,2,1,26,51,76,101,106,111,116,121,122,123,124,125,100,75,50,25,20,15,10,5,30,55,80,105,104,103,
   102,101,76,51,26,1,
};
//=================== Snake beside Flow =========================
//===============================================================
//===============================================================

//===============================================================
//===============================================================
//=================== Snake Random Flow =========================

unsigned char FlowSnakeRandom[300]  = 
{
   1,6,7,8,9,10,35,40,45,44,43,42,41,66,91,116,111,106,101,102,103,104,79,54,29,4,9,14,19,44,69,94,119,114,109,108,83,58,33,38,43,48,73,98,123,118
   ,113,108,83,58,33,28,27,26,31,36,41,46,47,48,49,50,45,40,35,60,59,58,57,62,63,64,65,70,69,68,67,62,57,52,27,26,31,36,41,46,47,48,49,50,45,70,95
   ,90,85,84,83,82,81,86,87,88,89,90,65,64,63,62,61,56,51,26,31,32,37,42,47,48,49,50,45,40,35,30,5,5,13,17,21,46,96,91,86,81,76,101,106,111,116,121,
   122,123,118,113,108,103,102,101,76,51,26,1,6,11,16,21,22,23,24,25,20,15,10,5,4,3,2,1,26,31,36,41,46,47,48,49,50,45,40,35,30,29,28,27,26,51,56,61
   ,66,71,72,73,74,75,70,65,60,55,54,53,52,51,76,81,86,91,96,97,98,99,100,95,90,85,80,79,78,77,76,101,106,111,116,121,122,123,124,125,120,115,110
   ,105,104,103,102,101
};
//=================== Snake beside Flow =========================
//===============================================================
//===============================================================

int led = 13;
const byte SW = 5;
boolean LastState = LOW;
boolean toggle = LOW;
byte Click = 0;

int BootTime = 60;


void setup() {                
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  pinMode(led, OUTPUT);
  pinMode(SW, INPUT);
  digitalWrite(SW, LOW);
  Click = 2;
}

//==========================================================================================
//========================================= S T A R T ======================================
//==========================================================================================

//==========================================================================================
//========================================= L O O P ======================================
//==========================================================================================

void loop() {
  
  boolean b1 = digitalRead(SW);
  
  if (b1 != LastState){
    delay(1);
    boolean b2 = digitalRead(SW);
    
    if (b1 == b2){
      LastState = b1;
      Click ++;    
    }
  }

  //==============================================================================================================

  if (Click == 2){                          //
      unsigned long startMillis= millis();  // Start of sample window
      float peakToPeak = 0;   // peak-to-peak level
    
      unsigned int signalMax = 0;
      unsigned int signalMin = 1023;
      unsigned int c, y;
    
    
      // collect data for length of sample window (in mS)
      while (millis() - startMillis < SAMPLE_WINDOW)
      {
        sample = analogRead(MIC_PIN);
        if (sample < 1024)  // toss out spurious readings
        {
          if (sample > signalMax)
          {
            signalMax = sample;  // save just the max levels
          }
          else if (sample < signalMin)
          {
            signalMin = sample;  // save just the min levels
          }
        }
      }
      peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
     
      // Serial.println(peakToPeak);
    
    
      //Fill the strip with rainbow gradient
      for (int i=0;i<=strip.numPixels()-1;i++){
        strip.setPixelColor(i,Wheel2(map(i,0,strip.numPixels()-1,30,150)));
      }
    
    
      //Scale the input logarithmically instead of linearly
      c = fscale(INPUT_FLOOR, INPUT_CEILING, strip.numPixels(), 0, peakToPeak, 2);
    
      
    
    
      if(c < peak) {
        peak = c;        // Keep dot on top
        dotHangCount = 0;    // make the dot hang before falling
      }
      if (c <= strip.numPixels()) { // Fill partial column with off pixels
        drawLine(strip.numPixels(), strip.numPixels()-c, strip.Color(0, 0, 0));
      }
    
      // Set the peak dot to match the rainbow gradient
      y = strip.numPixels() - peak;
      
      strip.setPixelColor(y-1,Wheel2(map(y,0,strip.numPixels()-1,30,150)));
    
      strip.show();
    
      // Frame based peak dot animation
      if(dotHangCount > PEAK_HANG) { //Peak pause length
        if(++dotCount >= PEAK_FALL) { //Fall rate 
          peak++;
          dotCount = 0;
        }
      } 
      else {
        dotHangCount++; 
      }
  }
  
  //==============================================================================================================
  
  if (Click == 4){                          //
    SnakeBesideFlow(1);
    JPflowC1(3);
    SnakeRandomFlow(1);
  }
  
  //==============================================================================================================
  
  if (Click == 6){                          //
    white();
    red();
    blue();
    green();
    purple();
    colorWipe(strip.Color(255, 0, 0), 2); // Red
    colorWipe(strip.Color(0, 255, 0), 5); // Green
    colorWipe(strip.Color(0, 0, 255), 5); // Blue
    theaterChase(strip.Color(127, 127, 127), 30); // White
    theaterChase(strip.Color(127,   0,   0), 30); // Red
    theaterChase(strip.Color(  0,   0, 127), 30); // Blue
  
    rainbowALL(8);
    rainbowCycle(5);
    theaterChaseRainbow(4);
    
    Click = 2;
  }
}

//==========================================================================================
//========================================= L O O P ========================================
//==========================================================================================

//==========================================================================================
//========================================= S T A R T ======================================
//==========================================================================================
  
  //colorWipe(strip.Color(255, 0, 0), 20); // Red
  //colorWipe(strip.Color(0, 255, 0), 50); // Green
  //colorWipe(strip.Color(0, 0, 255), 50); // Blue
 
  //theaterChase(strip.Color(127, 127, 127), 30); // White
  //theaterChase(strip.Color(127,   0,   0), 30); // Red
  //theaterChase(strip.Color(  0,   0, 127), 30); // Blue

  //rainbowALL(8);
  //rainbowCycle(5);
  //theaterChaseRainbow(40);

//==========================================================================================
//========================================= E F F E C T ====================================
//==========================================================================================

void JPfellWhite(uint8_t wait)
{
  for (int x=80; x<256; x++){
            colorWipe(strip.Color(x, x, x), 0);
            strip.show();
            delay(5);           
          }
}

//===============================================================
//===============================================================
//=================== Flow Loop =================================

void JPflowC1(int x)
{
   for (int i; i<x ;i++){
    MatrixFlow(1);
    //JPflow2(2);
    //JPSetWhite();
    delay(500);
   } 
}

void JPflowC2(int x)
{
   for (int i; i<x ;i++){
    MatrixFlow(1);
    MatrixFlow(1);
    //JPflow2(2);
    //JPSetWhite();
    delay(500);
   } 
}

void JPflowC3(int x)
{
   for (int i; i<x ;i++){
    MatrixFlow(1);
    MatrixFlow(1);
    MatrixFlow(1);
    //JPSetWhite();
    delay(500);
   } 
}

void JPflowC4(int x)
{
   for (int i; i<x ;i++){
    MatrixFlow(1);
    MatrixFlow(1);
    MatrixFlow(1);
    MatrixFlow(1);
    JPSetWhite();
    delay(500);
   } 
}

void JPflowC5(int x)
{
   for (int i; i<x ;i++){
    MatrixFlow(1);
    MatrixFlow(1);
    MatrixFlow(1);
    MatrixFlow(1);
    MatrixFlow(1);
    JPSetWhite();
    delay(500);
   } 
}

void JPflowC6(int x)
{
   for (int i; i<x ;i++){
    MatrixFlow(1);
    MatrixFlow(1);
    MatrixFlow(1);
    MatrixFlow(1);
    MatrixFlow(1);
    MatrixFlow(1);
    JPSetWhite();
    //JPflow2(2);
    delay(500);
   } 
}
//=================== Flow Loop== ===============================
//===============================================================
//===============================================================

//===============================================================
//===============================================================
//=================== Matrix Flow ===============================
void MatrixFlow(uint8_t wait)
{
  uint16_t i, j;
   
   for (uint16_t i=0; i<=strip.numPixels()-104; i++){  //
      for (int x=0; x<strip.numPixels(); x++){
        //colorWipe(strip.Color(50, 50, 50), 0);
        //colorWipefast2(strip.Color(50, 50, 50));
        //strip.setPixelColor(x , 1, 1, 1);
        strip.setPixelColor(x , 0, 0, 0);
      }
      //
      
      strip.setPixelColor(FlowMatrix1[i] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix2[i] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix3[i] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix4[i] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix5[i] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix6[i] -1, 100, 100, 100);
      //
      strip.setPixelColor(FlowMatrix1[i-1] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix2[i-1] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix3[i-1] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix4[i-1] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix5[i-1] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix6[i-1] -1, 100, 100, 100);
      //
      strip.setPixelColor(FlowMatrix1[i-2] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix2[i-2] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix3[i-2] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix4[i-2] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix5[i-2] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix6[i-2] -1, 100, 100, 100);
      //
      strip.setPixelColor(FlowMatrix1[i-3] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix2[i-3] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix3[i-3] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix4[i-3] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix5[i-3] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix6[i-3] -1, 100, 100, 100);
      //
      strip.setPixelColor(FlowMatrix1[i-4] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix2[i-4] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix3[i-4] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix4[i-4] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix5[i-4] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix6[i-4] -1, 100, 100, 100);
      //
      strip.setPixelColor(FlowMatrix1[i-5] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix2[i-5] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix3[i-5] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix4[i-5] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix5[i-5] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix6[i-5] -1, 100, 100, 100);
      //
      strip.setPixelColor(FlowMatrix1[i-6] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix2[i-6] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix3[i-6] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix4[i-6] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix5[i-6] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix6[i-6] -1, 100, 100, 100);
      //
      strip.setPixelColor(FlowMatrix1[i-7] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix2[i-7] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix3[i-7] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix4[i-7] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix5[i-7] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix6[i-7] -1, 100, 100, 100);
      //
      strip.setPixelColor(FlowMatrix1[i-8] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix2[i-8] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix3[i-8] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix4[i-8] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix5[i-8] -1, 100, 100, 100);
      strip.setPixelColor(FlowMatrix6[i-8] -1, 100, 100, 100);
      
      //strip.setPixelColor(FlowMatrix[i-6], 100, 100, 100);
      //strip.setPixelColor(FlowMatrix[i-5], 100, 100, 100);
      //strip.setPixelColor(FlowMatrix[i-4], 100, 100, 100);
      // strip.setPixelColor(FlowMatrix[i-3], 100, 100, 100);
      //strip.setPixelColor(FlowMatrix[i-2], 100, 100, 100);
      //strip.setPixelColor(FlowMatrix[i-1], 100, 100, 100);
      //strip.setPixelColor(FlowMatrix[i], 100, 100, 100);
      //strip.setPixelColor(FlowMatrix[i+1], 100, 100, 100);
      
      strip.show();
      delay(DelayTime);
   }
}
//=================== Matrix Flow ===============================
//===============================================================
//===============================================================

//===============================================================
//===============================================================
//=================== Snake beside Flow =========================
void SnakeBesideFlow(uint8_t wait)
{
  uint16_t i, j;
   
   for (uint16_t i=0; i<=strip.numPixels()-22; i++){  //
      for (int x=0; x<strip.numPixels(); x++){
        //colorWipe(strip.Color(50, 50, 50), 0);
        //colorWipefast2(strip.Color(50, 50, 50));
        //strip.setPixelColor(x , 1, 1, 1);
        strip.setPixelColor(x , 0, 0, 0);
      }
      //     
      strip.setPixelColor(FlowSnakeBeside[i] -1, 100, 100, 100);
     //
      strip.setPixelColor(FlowSnakeBeside[i-1] -1, 100, 100, 100);
      //
      strip.setPixelColor(FlowSnakeBeside[i-2] -1, 100, 100, 100);
      //
      strip.setPixelColor(FlowSnakeBeside[i-3] -1, 100, 100, 100);
      //
      strip.setPixelColor(FlowSnakeBeside[i-4] -1, 100, 100, 100);
      //
      strip.setPixelColor(FlowSnakeBeside[i-5] -1, 100, 100, 100);
      //
      strip.setPixelColor(FlowSnakeBeside[i-6] -1, 100, 100, 100);
      //
      strip.setPixelColor(FlowSnakeBeside[i-7] -1, 100, 100, 100);
      //
      strip.setPixelColor(FlowSnakeBeside[i-8] -1, 100, 100, 100);
          
      strip.show();
      delay(DelayTime);
   }
}
//=================== Snake beside Flow =========================
//===============================================================
//===============================================================

//===============================================================
//===============================================================
//=================== SnakeRandomFlow== =========================
void SnakeRandomFlow(uint8_t wait)
{
  uint16_t i, j;
   
   for (uint16_t i=0; i<=strip.numPixels()+200; i++){  //
      for (int x=0; x<strip.numPixels(); x++){
        //colorWipe(strip.Color(50, 50, 50), 0);
        //colorWipefast2(strip.Color(50, 50, 50));
        //strip.setPixelColor(x , 1, 1, 1);
        strip.setPixelColor(x , 0, 0, 0);
      }
      //     
      strip.setPixelColor(FlowSnakeRandom[i] -1, 100, 100, 100);
     //
      strip.setPixelColor(FlowSnakeRandom[i-1] -1, 100, 100, 100);
      //
      strip.setPixelColor(FlowSnakeRandom[i-2] -1, 100, 100, 100);
      //     
      strip.setPixelColor(FlowSnakeRandom[i-3] -1, 100, 100, 100);
     //
      strip.setPixelColor(FlowSnakeRandom[i-4] -1, 100, 100, 100);
      //
      strip.setPixelColor(FlowSnakeRandom[i-5] -1, 100, 100, 100);
      //     
      strip.setPixelColor(FlowSnakeRandom[i-6] -1, 100, 100, 100);
     //
      strip.setPixelColor(FlowSnakeRandom[i-7] -1, 100, 100, 100);
      //
      strip.setPixelColor(FlowSnakeRandom[i-8] -1, 100, 100, 100);
          
      strip.show();
      delay(DelayTime);
   }
}
//=================== SnakeRandomFlow ===========================
//===============================================================
//===============================================================

void JPSetWhite()
{
      clearLEDs();
      for (int i=0; i<strip.numPixels(); i++)
      {
         strip.setPixelColor(i, 10, 10, 10);
      } 
      strip.show();  
      delay(1000);   
}

//==========================================================================================
//======================================= E F F E C T ======================================
//==========================================================================================

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);      
  }
}

// Fill the dots one after the other with a color2
void colorWipefast2(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);    
  }
}

void rainbowALL(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel2((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel2(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
      delay(wait);
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel2( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();    
        delay(wait);    
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel2(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

//==========================================================================================
//======================================= E F F E C T ======================================
//==========================================================================================

void white ()
{
      clearLEDs();
      for (int i=0; i<strip.numPixels(); i++)
      {
         strip.setPixelColor(i, WHITE);
         strip.show();  
         delay(time);
      }     
}

void red ()
{
      clearLEDs();
      for (int i=0; i<strip.numPixels(); i++)
      {
         strip.setPixelColor(i, RED);
      }
      strip.show();
      delay(time);
}

void green ()
{
      clearLEDs();
      for (int i=0; i<strip.numPixels(); i++)
      {
         strip.setPixelColor(i, GREEN);
         strip.show();  
         delay(time);
      } 
}

void blue ()
{
      clearLEDs();
      for (int i=0; i<strip.numPixels(); i++)
      {
         strip.setPixelColor(i, BLUE);
         strip.show();  
         delay(time);
      }
}

void purple ()
{
      clearLEDs();
      for (int i=0; i<strip.numPixels(); i++)
      {
         strip.setPixelColor(i, PURPLE);
         strip.show();  
         delay(time);
      }
}

//=============================================
//=============  B R E A T H  =================
//=============================================

void BreatheWhite(unsigned int x)
{
  for (; x>0; x--)
  {
    for (int BreatheNum = 0; BreatheNum < 200 ; BreatheNum++ )
    {
      for (int i=0; i<LED_COUNT; i++)
      {
        strip.setPixelColor(i, BreatheNum , BreatheNum , BreatheNum );
      }
      strip.show();  
      delay(2);
    }
    for (int BreatheNum = 200; BreatheNum > 0 ; BreatheNum-- )
    {
      for (int i=0; i<LED_COUNT; i++)
      {
        strip.setPixelColor(i, BreatheNum , BreatheNum , BreatheNum );
      }
      strip.show(); 
      delay(2);
    }    
  } 
}
void BreatheWhite2(unsigned int x)
{
  for (; x>0; x--)
  {
    for (int BreatheNum = 200; BreatheNum > 0 ; BreatheNum-- )
    {
      for (int i=0; i<LED_COUNT; i++)
      {
        strip.setPixelColor(i, BreatheNum , BreatheNum , BreatheNum );
      }
      strip.show(); 
      delay(1);
    }
   }    
} 
void BreatheRed(unsigned int x)
{
  for (; x>0; x--)
  {
    for (int BreatheNum = 0; BreatheNum < 200 ; BreatheNum++ )
    {
      for (int i=0; i<LED_COUNT; i++)
      {
        strip.setPixelColor(i, BreatheNum , 0 , 0 );
      }
      strip.show();  
      delay(2);
    }
    for (int BreatheNum = 200; BreatheNum > 0 ; BreatheNum-- )
    {
      for (int i=0; i<LED_COUNT; i++)
      {
        strip.setPixelColor(i, BreatheNum , 0 , 0 );
      }
      strip.show(); 
      delay(2);
    }    
  } 
}

void BreatheGreen(unsigned int x)
{
    for (int BreatheNum = 0; BreatheNum < 200 ; BreatheNum++ )
    {
      for (int i=0; i<LED_COUNT; i++)
      {
        strip.setPixelColor(i, 0 , BreatheNum , 0 );
      }
      strip.show(); 
      delay(2);
    }
    for (int BreatheNum = 200; BreatheNum > 0 ; BreatheNum-- )
    {
      for (int i=0; i<LED_COUNT; i++)
      {
        strip.setPixelColor(i, 0 , BreatheNum , 0 );
      }
      strip.show();  
      delay(2);
    }    
} 
void BreatheGreen2(unsigned int x)
{
  for (; x>0; x--)
  {
    for (int BreatheNum = 0; BreatheNum < 200 ; BreatheNum++ )
    {
      for (int i=0; i<LED_COUNT; i++)
      {
        strip.setPixelColor(i, 0 , BreatheNum , 0 );
      }
      strip.show(); 
      delay(1);
    }    
  } 
}

void BreatheGreen3(unsigned int x)
{
    for (int BreatheNum = 200; BreatheNum > 0 ; BreatheNum-- )
    {
      for (int i=0; i<LED_COUNT; i++)
      {
        strip.setPixelColor(i, 0 , BreatheNum , 0 );
      }
      strip.show();  
      delay(2);
    }     
}

void BreatheBlue(unsigned int x)
{
  for (; x>0; x--)
  {
    for (int BreatheNum = 0; BreatheNum < 200 ; BreatheNum++ )
    {
      for (int i=0; i<LED_COUNT; i++)
      {
        strip.setPixelColor(i, 0 , 0 , BreatheNum );
      }
      strip.show();  
      delay(2);

    }
    for (int BreatheNum = 200; BreatheNum > 0 ; BreatheNum-- )
    {
      for (int i=0; i<LED_COUNT; i++)
      {
        strip.setPixelColor(i, 0 , 0 , BreatheNum );
      }
      strip.show();  
      delay(2);
    }    
  } 
}

void BreathePurple(unsigned int x)
{
  for (; x>0; x--)
  {
    for (int BreatheNum = 0; BreatheNum < 200 ; BreatheNum++ )
    {
      for (int i=0; i<LED_COUNT; i++)
      {
        strip.setPixelColor(i, BreatheNum , 0 , BreatheNum );
      }
      strip.show();  
      delay(2);
    }
    for (int BreatheNum = 200; BreatheNum > 0 ; BreatheNum-- )
    {
      for (int i=0; i<LED_COUNT; i++)
      {
        strip.setPixelColor(i, BreatheNum , 0 , BreatheNum );
      }
      strip.show();  
      delay(2);
    }    
  } 
}

//=============================================
//============= R A I N B O W =================
//=============================================

void DynamicRainbow (unsigned int x)
{  
  // Ride the Rainbow Road
  for (; x>0; x--)
  {
    for (int i=0; i<LED_COUNT*5; i++)
    {
      rainbow(i);
      delay(40);  // Delay between rainbow slides
    }
  }
}
//----------------------------------------------------------  
// Sets all LEDs to off, but DOES NOT update the display;
// call leds.show() to actually turn them off after this.
void clearLEDs()
{
  for (int i=0; i<LED_COUNT; i++)
  {
    strip.setPixelColor(i, 0);
  }
}
// Prints a rainbow on the ENTIRE LED strip.
//  The rainbow begins at a specified position. 
// ROY G BIV!
void rainbow(byte startPosition) 
{
  // Need to scale our rainbow. We want a variety of colors, even if there
  // are just 10 or so pixels.
  int rainbowScale = 192 / LED_COUNT;
  
  // Next we setup each pixel with the right color
  for (int i=0; i<LED_COUNT; i++)
  {
    // There are 192 total colors we can get out of the rainbowOrder function.
    // It'll return a color between red->orange->green->...->violet for 0-191.
    strip.setPixelColor(i, rainbowOrder((rainbowScale * (i + startPosition)) % 192));
  }
  // Finally, actually turn the LEDs on:
  strip.show();
}

// Input a value 0 to 191 to get a color value.
// The colors are a transition red->yellow->green->aqua->blue->fuchsia->red...
//  Adapted from Wheel function in the Adafruit_NeoPixel library example sketch
uint32_t rainbowOrder(byte position) 
{
  // 6 total zones of color change:
  if (position < 31)  // Red -> Yellow (Red = FF, blue = 0, green goes 00-FF)
  {
    return strip.Color(0xFF, position * 8, 0);
  }
  else if (position < 63)  // Yellow -> Green (Green = FF, blue = 0, red goes FF->00)
  {
    position -= 31;
    return strip.Color(0xFF - position * 8, 0xFF, 0);
  }
  else if (position < 95)  // Green->Aqua (Green = FF, red = 0, blue goes 00->FF)
  {
    position -= 63;
    return strip.Color(0, 0xFF, position * 8);
  }
  else if (position < 127)  // Aqua->Blue (Blue = FF, red = 0, green goes FF->00)
  {
    position -= 95;
    return strip.Color(0, 0xFF - position * 8, 0xFF);
  }
  else if (position < 159)  // Blue->Fuchsia (Blue = FF, green = 0, red goes 00->FF)
  {
    position -= 127;
    return strip.Color(position * 8, 0, 0xFF);
  }
  else  //160 <position< 191   Fuchsia->Red (Red = FF, green = 0, blue goes FF->00)
  {
    position -= 159;
    return strip.Color(0xFF, 0x00, 0xFF - position * 8);
  }
}

//=============================================
//============= R A I N B O W =================
//=============================================

//==========================================================================================
//======================================= E F F E C T ======================================
//==========================================================================================

//==========================================================================================
//======================================= JUMPING ==========================================
//==========================================================================================

//Used to draw a line between two points of a given color
void drawLine(uint8_t from, uint8_t to, uint32_t c) {
  uint8_t fromTemp;
  if (from > to) {
    fromTemp = from;
    from = to;
    to = fromTemp;
  }
  for(int i=from; i<=to; i++){
    strip.setPixelColor(i, c);
  }
}


float fscale( float originalMin, float originalMax, float newBegin, float
newEnd, float inputValue, float curve){

  float OriginalRange = 0;
  float NewRange = 0;
  float zeroRefCurVal = 0;
  float normalizedCurVal = 0;
  float rangedValue = 0;
  boolean invFlag = 0;


  // condition curve parameter
  // limit range

  if (curve > 10) curve = 10;
  if (curve < -10) curve = -10;

  curve = (curve * -.1) ; // - invert and scale - this seems more intuitive - postive numbers give more weight to high end on output 
  curve = pow(10, curve); // convert linear scale into lograthimic exponent for other pow function

  /*
   Serial.println(curve * 100, DEC);   // multply by 100 to preserve resolution  
   Serial.println(); 
   */

  // Check for out of range inputValues
  if (inputValue < originalMin) {
    inputValue = originalMin;
  }
  if (inputValue > originalMax) {
    inputValue = originalMax;
  }

  // Zero Refference the values
  OriginalRange = originalMax - originalMin;

  if (newEnd > newBegin){ 
    NewRange = newEnd - newBegin;
  }
  else
  {
    NewRange = newBegin - newEnd; 
    invFlag = 1;
  }

  zeroRefCurVal = inputValue - originalMin;
  normalizedCurVal  =  zeroRefCurVal / OriginalRange;   // normalize to 0 - 1 float

  // Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine 
  if (originalMin > originalMax ) {
    return 0;
  }

  if (invFlag == 0){
    rangedValue =  (pow(normalizedCurVal, curve) * NewRange) + newBegin;

  }
  else     // invert the ranges
  {   
    rangedValue =  newBegin - (pow(normalizedCurVal, curve) * NewRange); 
  }

  return rangedValue;
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
//==========================================================================================
//======================================= JUMPING ==========================================
//==========================================================================================
