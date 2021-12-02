#include <Wire.h> 
#include "DS1307.h" //oorrect DC1307 grove lobrary 
#include "dht.h" 
#include "HX711.h" 
#include "constants.h"



//defining sensor objects
DS1307 clock = DS1307();               
DHT dht(TEMP_SENS, DHT11);              //fixed notation  
HX711 scale = HX711();           //weight object name changed. 

//placeholder figure to calibrate sensor (has potential to cause bugs)
#define calibration_factor -7050.0

int moistValue = 0;              // read moisture value 
int batteryValue = 0;            //read the battery level. 

int dryLimit    = 750;      //most of these, the user should be able to set in app
int waterLimit  = 420;       
int EnclosureTemp = 30;     
float WeightValue = 35.90;
int wateringTick = 0;
int tempTick = 0;
int LEDTick = 0;
int weightTick = 0;      
int tickDelayPump = 0;
int tickDelayLED = 0;
int tickDelayTemp = 0;
int tickDelayWeight = 0;

float t;
float h;

float weight;

int waterHour = 11;//sync with IoT to store the variables.
int waterMinute = 00;

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
  clock.begin();            //TODO: beginning the clock every time it starts can mess with it. Need to have oscillating if statement
  
  dht.begin();

  pinMode(2, OUTPUT);   // set relay pin to output
  pinMode(LED_STRIP, OUTPUT);     // set led pin to output
  //Weight Sensor Setup. 
  //scale.begin(WEIGHT_CLK, WEIGHT_DOUT); //uncommented the constants to get this working. 
  scale.set_scale(calibration_factor);
  scale.tare();   //reset scale to 0 assuming no weight. 
  
}

void loop(){
  //Display();              
  //TempCheck();            
  //WeightCheck();          
  //BatteryLevel();
  //digitalWrite(2,HIGH);
  //delay(1000);
  //digitalWrite(2,LOW);
  //delay(1000);
  //LEDCheck();
  WaterPump();        
}

//added semicolons where it needed to be done. 
void WaterPump(){
  // Water Pump + Relay Function 
  moistValue = analogRead(MOISTURE_SENSOR);
  Serial.print("Moisture:");
  Serial.print(moistValue);
  clock.getTime();
  //Serial.print("sensor 0 = \n");
  //Serial.print(moistValue);
  Serial.println("\n");

  if(moistValue <= dryLimit || (clock.hour == waterHour && clock.minute == waterMinute)){
    // enter if moisture low
    if (tickDelayPump == 0) {
      // enter if pump not turned on in last 100 ticks
      wateringTick = 100;
      digitalWrite(PUMP, LOW);   
      Serial.print("Pump on"); 
      tickDelayPump = 100; 
    }
    else if(tickDelayPump ==1){
      digitalWrite(PUMP,HIGH);
      Serial.print("PUMP COOLDOWN");
      }
  }

  if(wateringTick == 1){
    // if pump on ticker reaches 1, send pump off signal
    digitalWrite(PUMP, HIGH);    //pump high = off 
    Serial.print("Pump off");
  }
  Serial.print("Tickdelay:");
  Serial.print(tickDelayPump);
  Serial.print("\n");
  Serial.print("wateringtick:");
  Serial.print(wateringTick);
  tickDelayPump = constrain(tickDelayPump-1, 0 , 1000);
  wateringTick = constrain(wateringTick-1, 0, 1000);//decrements tick by 1 and prevents it from going into minus
  delay(200);
}
  
//humidity check

void LEDCheck(){
  //write a function to check the current time and minute then turn on led if the user set time is 
  clock.getTime();
  if(clock.hour == LEDHour && clock.minute == LEDMinute){
    if(tickDelayLED == 0){
      LEDTick = 100;
      digitalWrite(LED_STRIP,HIGH);
      tickDelayLED = 100;
    }
    
    if(LEDTick == 1){
      digitalWrite(LED_STRIP,LOW);
    }
  }
  tickDelayLED = constrain(tickDelayLED-1, 0 ,1000);
  LEDTick = constrain(LEDTick-1,0,1000);
}
void TempCheck(){
  //checks the temperature
  t = dht.readTemperature();
  h = dht.readHumidity();
  if(t > EnclosureTemp)
  //tag = 3;
  {
    if(tickDelayTemp == 0){
      tempTick = 30;
      digitalWrite(BUZZER, HIGH); //buzzer goes off to warn that the temperature is too high!
      tickDelayTemp = 100;
    }
    if(tempTick == 1){
      digitalWrite(BUZZER,LOW);
    } 
    Serial.println("Temperature is too high!");//To be displayed
    Serial.println(t); 
    Serial.println(h); 
    Display();
  }
  tickDelayTemp = constrain(tickDelayTemp-1,0,1000);
  tempTick = constrain(tempTick-1,0,1000);
}



void WeightCheck(){
  //checks the weight of the water canister.
  //tag = 1;
  weight = scale.get_units();
  while (weight < WeightValue)
  {
    if(tickDelayWeight == 0){
      
      weightTick = 30;
      digitalWrite(BUZZER, HIGH); //buzzer goes off to warn that the temperature is too high!
      tickDelayWeight = 30;
    }
    if(weightTick == 1){
      digitalWrite(BUZZER,LOW);
    }   
    Serial.println("Add more water to the water canister please!");//to be displayed 
    Display();
  }
  tickDelayWeight = constrain(tickDelayWeight-1,0,1000);
  weightTick = constrain(weightTick-1,0,1000);
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
