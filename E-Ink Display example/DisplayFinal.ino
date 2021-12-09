#include <SPI.h>
#include "epd2in9.h"
#include "epdpaint.h"
#include "imagedata.h"

#define COLORED     0
#define UNCOLORED   1

#define DISPLAY_UPDATE_INTERVAL 1000

unsigned long currentTime(0);
unsigned long lastPrintToDisplay(0);

unsigned char image[1024];
Paint paint(image, 0, 0);    // width should be the multiple of 8 
Epd epd;

int wifi(0);
int batteryLevel(100);
int moisture(80);
int waterLevel(60);
int temperature(25);
int humidity(70);

void setup() {
  /***E-Paper Display Initialisation***/
  if (epd.Init(lut_full_update) != 0) {
      Serial.print("e-Paper init failed");
      return;
  }

  epd.ClearFrameMemory(0xFF);   // bit set = white, bit reset = black
  epd.DisplayFrame();
  epd.ClearFrameMemory(0xFF);   // bit set = white, bit reset = black
  epd.DisplayFrame();

  if (epd.Init(lut_partial_update) != 0) {
      Serial.print("e-Paper init failed");
      return;
  }
  /***********************************/
}

void loop() {
  currentTime = millis();

  if (currentTime - lastPrintToDisplay >= DISPLAY_UPDATE_INTERVAL) {
    printToDisplay();
    lastPrintToDisplay = currentTime;
  }
}

void printToDisplay() {
  //296 x 128

  paint.SetRotate(ROTATE_90);

  
  paint.SetWidth(10);
  paint.SetHeight(296);

  paint.Clear(UNCOLORED);
  char line1String[30];
  
  if (wifi == 0)
    sprintf(line1String, "WiFi: Not Connected");
  else
    sprintf(line1String, "WiFi: Connected");
    
  paint.DrawStringAt(2, 2, line1String, &Font12, COLORED);
  char line2String[30];
  sprintf(line2String, "Battery Level: %d%%", batteryLevel);
  paint.DrawStringAt(162, 2, line2String, &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 118, 0, paint.GetWidth(), paint.GetHeight());

  paint.Clear(UNCOLORED);
  char line3String[30];
  sprintf(line3String, "Moisture Level:  %d%%", moisture);
  paint.DrawStringAt(2, 5, line3String, &Font12, COLORED);
  char line4String[30];
  sprintf(line4String, "Temperature: %dC", temperature);
  paint.DrawStringAt(182, 5, line4String, &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 20, 0, paint.GetWidth(), paint.GetHeight());

  paint.Clear(UNCOLORED);
  char line5String[30];
  sprintf(line5String, "Reservoir Level: %d%%", waterLevel);
  paint.DrawStringAt(2, 5, line5String, &Font12, COLORED);
  char line6String[30];
  sprintf(line6String, "Humidity:    %d%%", humidity);
  paint.DrawStringAt(182, 5, line6String, &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight());


  paint.SetWidth(16);
  paint.SetHeight(94);

  paint.Clear(UNCOLORED);
  char line7String[30];
  //clock.getTime()
  //sprintf(line7String, "%d:%d:%d", clock.hour, clock.minute, clock.second);
  sprintf(line7String, "15:58:34");
  paint.DrawStringAt(2, 2, line7String, &Font16, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 100, 101, paint.GetWidth(), paint.GetHeight());


  paint.SetWidth(20);
  paint.SetHeight(296);

  paint.Clear(UNCOLORED);
  char line8String[30];
  sprintf(line8String, "Autonomous Incubator");
  paint.DrawStringAt(8, 10, line8String, &Font20, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 56, 0, paint.GetWidth(), paint.GetHeight());


  if (batteryLevel <= 20) {
    paint.SetWidth(20);
    paint.SetHeight(242);
  
    paint.Clear(UNCOLORED);
    char line9String[30];
    sprintf(line9String, "Battery Level Low");
    paint.DrawStringAt(2, 4, line9String, &Font20, COLORED);
    epd.SetFrameMemory(paint.GetImage(), 76, 27, paint.GetWidth(), paint.GetHeight());
  }


  if (waterLevel <= 20) {
    paint.SetWidth(20);
    paint.SetHeight(270);
  
    paint.Clear(UNCOLORED);
    char line10String[30];
    sprintf(line10String, "Water Reservoir Low");
    paint.DrawStringAt(2, 8, line10String, &Font20, COLORED);
    epd.SetFrameMemory(paint.GetImage(), 36, 13, paint.GetWidth(), paint.GetHeight());
  }


  epd.DisplayFrame();
  epd.ClearFrameMemory(0xFF);
}
