/*
 * Mobile Home Water hight measurement 
 * TTGO T Display with 240(h)  x 135(w) screen 
 *
 */

#include <TFT_eSPI.h>       // Hardware-specific library
#include "Adafruit_VL53L0X.h"
#include <LittleFS.h>
#include <Adafruit_MPU6050.h>

#define iHeight             130
#define iQuarter            (130-2*2-3*5)/4
#define iWidth              48
#define ix                  44
#define iy                  80
#define istep               24
#define AUX_BUTTON          35

int meten[3];
byte count                  = 0;
int  perc                   = 0;
int  perc_prev              = 0;
unsigned int Vmin;
unsigned int Vmax;
long Vgem                   = 0;
long Vdelta                 = 240;
volatile unsigned long  Tpressed = 0;
volatile byte           pressed = 0;

TFT_eSPI tft = TFT_eSPI();  // Invoke custom library
Adafruit_VL53L0X vl53 = Adafruit_VL53L0X();

Adafruit_MPU6050 mpu;
sensors_event_t a, g, temp;

void DrawSets(bool Vmin){
    char temp[10];
    
    tft.setViewport(0, 0, 135, 50);
    tft.fillScreen(TFT_BLACK);
//    tft.frameViewport(TFT_RED, 1);
    if (Vmin) {
      snprintf(temp,10,"Vmin: %d",Vmin);
      tft.drawCentreString(temp, 67, 15, 2);
    }
    else {
      snprintf(temp,10,"Vmax: %d",Vmax);
      tft.drawCentreString(temp, 67, 15, 2);
    }
    tft.resetViewport();
    delay(2500);
    DrawPerc(); //restore normal screen
}

void PressedClear(){
    char temp[2];
    snprintf(temp,2,"%d",pressed);
    tft.setViewport(115, 210, 20, 30);
    tft.fillScreen(TFT_BLACK);
    tft.resetViewport();
}

void Pressed(){
    char temp[2];
    snprintf(temp,2,"%d",pressed);
    tft.setViewport(115, 210, 20, 30);
    tft.fillScreen(TFT_BLACK);
//    tft.frameViewport(TFT_NAVY, 1);
    tft.drawString(temp, 2, 15);
    tft.resetViewport();
}

void DrawPerc(){
    char temp[6];
    snprintf(temp,6," %d%%",perc);
    tft.setViewport(0, 0, 135, 50);
    tft.fillScreen(TFT_BLACK);
//    tft.frameViewport(TFT_NAVY, 1);
    tft.drawCentreString(temp, 67, 15, 2);
    tft.resetViewport();
}

void DrawLevel(){

//round
//    tft.fillRoundRect(ix, iy, iWidth,iHeight, iWidth/2,TFT_WHITE);
//    tft.fillRoundRect(ix+3, iy+3, iWidth-6,iHeight-6, iWidth/2-3,TFT_BLACK);
//    tft.fillRoundRect(ix+5, iy+5, iWidth-10,iHeight-10, iWidth/2-5,TFT_BLUE);

//    tft.drawRoundRect(ix, iy, iWidth,iHeight, iWidth/2,TFT_WHITE);
//    tft.drawRoundRect(ix-1, iy-1, iWidth+2,iHeight+2, iWidth/2+1,TFT_WHITE);
//    tft.drawRoundRect(ix-2, iy-2, iWidth+3,iHeight+3, iWidth/2+2,TFT_WHITE);


//rectangle
    float   fill      = perc/100.0;
    int     collor    = TFT_BLUE; //default
    byte    fill_h    = (1-fill) * (iHeight-10);
    byte    fill_h_   = iHeight - 10 - fill_h;
    
    tft.fillRect(ix+5, iy+5, iWidth-10, fill_h,TFT_BLACK); //upper part
    
    if (perc < 25) collor = TFT_RED;
    else if  (perc < 50) collor = TFT_ORANGE;
    
    tft.fillRect(ix+5, iy+5+fill_h, iWidth-10,fill_h_,collor); //lower part
}

void setup()
{
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(0);
  setupAuxButton();
  Wire.begin();
  if (!vl53.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
  while (1); }

//  if (!mpu.begin()) {
//    Serial.println("Failed to find MPU6050 chip");
//    while (1) {
//      delay(10);
//    }
//  }
//  Serial.println("MPU6050 Found!");
  
  FSBegin();
  PrefBegin();
  SetupWebServer();

  // Swap the colour byte order when rendering
  tft.setSwapBytes(true);

  // Draw the icons
//    tft.pushImage(  0,   0,  135,  240, img_data);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.fillScreen(TFT_BLACK);
    DrawPerc();
    
    //draw graphic outline
    tft.drawRect(ix, iy, iWidth,iHeight,TFT_WHITE);
    tft.drawRect(ix+1, iy+1, iWidth-2,iHeight-2,TFT_WHITE);
    tft.fillRect(ix,iy+2+1*iQuarter+0*5,iWidth,5,TFT_BLACK );
    tft.fillRect(ix,iy+2+2*iQuarter+1*5,iWidth,5,TFT_BLACK);
    tft.fillRect(ix,iy+2+3*iQuarter+2*5,iWidth,5,TFT_BLACK);
    //initial value
    DrawLevel();
    esp_register_shutdown_handler(ShutDownHandler);

}

void loop()
{
  handleButtonPressed();
//  httpServer.handleClient();
  if (pressed) Pressed();
  
  VL53L0X_RangingMeasurementData_t measure; 
  vl53.rangingTest(&measure, false);
  if (Vmax && Vmin) {
        perc = 100 - ((Vgem - Vmax ) * 100 / Vdelta);
        if (perc > 100) perc = 100;
        else if (perc < 0) perc = 0;
//        Serial.printf("perc: %.0d\n", perc);
  }
  if (measure.RangeStatus != 4) {
    meten[count] = measure.RangeMilliMeter;
    Vgem = (meten[0]+meten[1]+meten[2])/3;
    Serial.printf("Distance (mm): %i | g:%i | min:%i | max:%i | delta:%i | perc :%i\n", meten[count], Vgem ,Vmin, Vmax, Vdelta, perc);
    if (count++ == 3) count = 0;
    }; 
//    else Serial.println("Out of range!");
  
  if (perc != perc_prev) {
//    tft.fillScreen(TFT_BLACK);
    DrawPerc();
    DrawLevel();
    perc_prev=perc;
  }

//  mpu.getEvent(&a, &g, &temp);
 yield();
}
