#include <Wire.h>
#include <SPI.h>
#include "epd2in9.h"
#include "epdpaint.h"
#include "imagedata.h" 
#include "DS1307.h" //oorrect DC1307 grove lobrary 
#include "dht.h" 
#include "HX711.h" 
#include "constants.h"
#include <iostream>

//defining sensor objects
//DS1307 clock = DS1307();               
DHT dht(TEMP_SENS, DHT11);              //fixed notation  
HX711 scale = HX711();           //weight object name changed. 

//describes the battery object. 

unsigned long currentTime(0);
unsigned long lastPrintToDisplay(0);

unsigned char image[1024];
Paint paint(image, 0, 0);    // width should be the multiple of 8 
Epd epd;

//placeholder figure to calibrate sensor (has potential to cause bugs)
float calibration_factor = -83886.00;

int moistValue = 430;              // read moisture value 
int battery_value = 100;            //read the battery level. 

int dryLimit    = 720;      //most of these, the user should be able to set in app
int waterLimit  = 420;       
int EnclosureTemp = 18;
int wifi = 0;
float weight = 0;
float temperature = 0;
int humidity = 0;     
float WeightValue = 1;
int wateringTick = 0;
int tempTick = 0;
int LEDTick = 0;
int weightTick = 0;      
int tickDelayPump = 0;
int tickDelayLED = 0;
int tickDelayTemp = 0;
int tickDelayWeight = 0;

int waterHour;//sync with IoT to store the variables.
int waterMinute;

int LEDHour;//sync with IoT to store the variables.
int LEDMinute;

// Pushingbox API
/*
char *api_server = “api.pushingbox.com”;
char *deviceId1 = “v7D2202342900F3C”;
char *deviceId2 = “vC4F10125FFD871F”;
char *deviceId3 = “v85B903287356B3A”;

int tag = -1; //Relating the sensor to notifications
*/
void setup(){ 
  Serial.begin(9600);       // start the serial at 9600 baud
  //clock.begin();            //TODO: beginning the clock every time it starts can mess with it. Need to have oscillating if statement
  dht.begin();  //running default constructor 

  pinMode(PUMP, OUTPUT);   // set relay pin to output
  pinMode(LED_STRIP, OUTPUT);     // set led pin to output
  pinMode(WEIGHT_CLK,INPUT);
  pinMode(WEIGHT_DOUT,INPUT);
  digitalWrite(PUMP,HIGH);


  //Weight Sensor Setup. 
  scale.begin(WEIGHT_DOUT,WEIGHT_CLK); //uncommented the constants to get this working. 
  scale.set_scale(calibration_factor);
  scale.tare();   //reset scale to 0 assuming no weight. 
  
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
}

void loop(){
 //digitalWrite(PUMP,LOW);
 currentTime = millis();

  if (currentTime - lastPrintToDisplay >= DISPLAY_UPDATE_INTERVAL) {
    Display();
    lastPrintToDisplay = currentTime;
  }
 //Display();              
 //TempCheck();          
 WeightCheck();          
 //BatteryLevel();
 //LEDCheck();
 //digitalWrite(PUMP,HIGH);
 //WaterPump();
//digitalWrite(BUZZER,HIGH);
//digitalWrite(BUZZER,LOW);
}

//added semicolons where it needed to be done. 
void WaterPump(){
  // Water Pump + Relay Function 
  moistValue = analogRead(MOISTURE_SENSOR);
  //clock.getTime();
  Serial.print("moisture = ");
  Serial.print(moistValue);
  Serial.print("\n");

  if(moistValue <= dryLimit /*|| (clock.hour == waterHour && clock.minute == waterMinute)*/){
    // enter if moisture 
    
    if (tickDelayPump == 0) {//set to 0 at start
      // enter if pump not turned on in last 100 ticks
      wateringTick = 20;//no. of ticks to water with
      digitalWrite(PUMP, LOW);//pump turns on    
      digitalWrite(BUZZER,HIGH);//buzzer turns on
      Serial.print("Pump on"); 
      tickDelayPump = 1; 
    }
     
  }

  if(wateringTick == 1){
    // if pump on ticker reaches 1, send pump off signal
    digitalWrite(PUMP, HIGH);    //pump high = off 
    digitalWrite(BUZZER,LOW); //turn buzzer off
    Serial.print("Pump off");
    tickDelayPump = 100;
  }
   
  tickDelayPump = constrain(tickDelayPump-1, 0 , 1000);//decrement each loop stop it at 0
  wateringTick = constrain(wateringTick-1, 0, 1000);//decrements tick by 1 and prevents it from going into minus 
  Serial.print("tick delay: ");
  Serial.print(tickDelayPump);
  Serial.print("\nwateringTick: ");
  Serial.print(wateringTick);
}
  
//humidity check

void LEDCheck(){
  //write a function to check the current time and minute then turn on led if the user set time is 
  //clock.getTime();
  //if(clock.hour == LEDHour && clock.minute == LEDMinute){
  Serial.print(tickDelayLED);
  Serial.print("\n");
    if(tickDelayLED == 0){
      LEDTick = 100;
      digitalWrite(LED_STRIP,HIGH);
      tickDelayLED = 100;
    }
    
    if(LEDTick == 1){
      digitalWrite(LED_STRIP,LOW);
      Serial.print("strip off");
      delay(1000);
    }
  //}
  tickDelayLED = constrain(tickDelayLED-1, 0 ,1000);
  LEDTick = constrain(LEDTick-1,0,1000);
}
void TempCheck(){
  //checks the temperature
  temperature = trunc(dht.readTemperature());
  humidity = dht.readHumidity();
  if(temperature > EnclosureTemp)
  //tag = 3;
  {
    
    /*if(tickDelayTemp == 0){
      tempTick = 30;
      digitalWrite(BUZZER, HIGH); //buzzer goes off to warn that the temperature is too high!
      tickDelayTemp = 100;
    }
    if(tempTick == 1){
      digitalWrite(BUZZER,LOW);
    } */
    Serial.println("Temperature is too high!");//To be displayed
    Serial.println(dht.readTemperature()); 
    Serial.println(dht.readHumidity()); 
    //Display();
  }
  tickDelayTemp = constrain(tickDelayTemp-1,0,1000);
  tempTick = constrain(tempTick-1,0,1000);
}



void WeightCheck(){
  //checks the weight of the water canister.
  weight = scale.get_units();
  Serial.print("weight= ");
  Serial.print(weight);
  Serial.print("\n");
  if (weight < WeightValue)
  {
    if(tickDelayWeight == 0){
      
      weightTick = 30;
      digitalWrite(BUZZER, HIGH); //buzzer goes off to warn that the temperature is too high!
      Serial.print("Refill water");
      Serial.print("\n");
      tickDelayWeight = 1;
    }
  }
    if(weightTick == 1){
      digitalWrite(BUZZER,LOW);
      tickDelayWeight = 100;
    }   
    //Serial.println("Add more water to the water canister please!");//to be displayed 
    //Display();
  if (weight >= WeightValue){
    Serial.print("sufficient water");
    Serial.print("\n");
    }
  tickDelayWeight = constrain(tickDelayWeight-1,0,1000);
  weightTick = constrain(weightTick-1,0,1000);
  Serial.print("tickdelayWeight: ");
  Serial.print(tickDelayWeight);
  Serial.print("\n");
  Serial.print("weightTick: ");
  Serial.print(weightTick);
  Serial.print("\n");
}

void BatteryLevel(){
  //tag = 2;
   //reads the battery level and displays it.
  //likely will have to allocate a number of "bars" to a discrete battery level and display this by fault on E-INK display.
   
   //float battery_analog = analogueRead(A7);
   //float input_volt = (battery_analog * 3.3)/1024 //converts the battery level into an analog voltage

   //float battery_percentage = ((((battery_analog/1023) * 3.3) - 2.66)/0.62)*100; 
   //Serial.println("Battery Percantage is: ");  
  
}

void Display(){
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

//Code programmed by Anthony A
/*void sendNotification(float sensorValue, int tag) {
Serial.println(“Sending notification to ” + String(api_server));
if (client.connect(api_server, 80)) {
Serial.println(“Connected to the server”);
String message;
switch(tag) {
  case 1:
    // code block
    message = “devid=” + String(deviceId1) + “\r\n\r\n”;
    break;
  case 2:
    // code block
    message = “devid=” + String(deviceId2) + “\r\n\r\n”;
    break;
    case 3:
    // code block
    message = “devid=” + String(deviceId3) + “\r\n\r\n”;
    break;
}

client.print(“POST /pushingbox HTTP/1.1\n”);
client.print(“Host: api.pushingbox.com\n”);
client.print(“Connection: close\n”);
client.print(“Content-Type: application/x-www-form-urlencoded\n”);
client.print(“Content-Length: “);
client.print(message.length());
client.print(“\n\n”);
client.print(message);
}
client.stop();
Serial.println(“Notification sent!”);
}*/
