#include <Wire.h> 
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

//placeholder figure to calibrate sensor (has potential to cause bugs)
float calibration_factor = -83886.00;

int moistValue = 430;              // read moisture value 
int battery_value = 0;            //read the battery level. 

int dryLimit    = 720;      //most of these, the user should be able to set in app
int waterLimit  = 420;       
int EnclosureTemp = 18;     
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
  
}

void loop(){
 //digitalWrite(PUMP,LOW);
 
 //Display();              
 //TempCheck();          
 WeightCheck();          
 //BatteryLevel();
 //LEDCheck();
 //digitalWrite(PUMP,HIGH);
 //WaterPump();
//digitalWrite(BUZZER,HIGH);
//digitalWrite(BUZZER,LOW);
delay(200);
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
  if(trunc(dht.readTemperature()) > EnclosureTemp)
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
  float weight = scale.get_units();
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
  //E-ink to be written by Rogier
  //display time and date by default via RTC
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
