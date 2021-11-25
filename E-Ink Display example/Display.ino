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

int line1(1);
int line2(2);
int line3(3);
int line4(4);
int line5(5);
int line6(6);
int line7(7);

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
  char line1String[20];
  sprintf(line1String, "Line 1 = %d", line1);
  paint.DrawStringAt(2, 2, line1String, &Font12, COLORED);
  char line2String[20];
  sprintf(line2String, "Line 2 = %d", line2);
  paint.DrawStringAt(162, 2, line2String, &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 118, 0, paint.GetWidth(), paint.GetHeight());

  paint.Clear(UNCOLORED);
  char line3String[20];
  sprintf(line3String, "Line 3 = %d", line3);
  paint.DrawStringAt(2, 6, line3String, &Font12, COLORED);
  char line4String[20];
  sprintf(line4String, "Line 4 = %d", line4);
  paint.DrawStringAt(160, 6, line4String, &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight());


  paint.SetWidth(30);
  paint.SetHeight(220);

  paint.Clear(UNCOLORED);
  char line5String[20];
  sprintf(line5String, "Line 5 = %d", line5);
  paint.DrawStringAt(0, 6, line5String, &Font24, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 52, 56, paint.GetWidth(), paint.GetHeight());


  paint.SetWidth(20);
  paint.SetHeight(148);

  paint.Clear(UNCOLORED);
  char line6String[20];
  sprintf(line6String, "Line 6 = %d", line6);
  paint.DrawStringAt(2, 2, line6String, &Font12, COLORED);
  char line7String[20];
  sprintf(line7String, "Line 7 = %d", line7);
  paint.DrawStringAt(2, 12, line7String, &Font12, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 88, 0, paint.GetWidth(), paint.GetHeight());


  epd.DisplayFrame();
  epd.ClearFrameMemory(0xFF);
}
