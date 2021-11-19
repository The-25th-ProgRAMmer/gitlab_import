#include <Wire.h> 
#include "DS1307.h" 
#include <dht.h> 
#include "HX711.h" 
#include "constants.h"

//defining sensor objects
DS1307 clock = DS1307();               
dht DHT = dht();                 //fixed notation  
HX711 WeightSensor = HX711();        

//placeholder figure to calibrate sensor (has potential to cause bugs)
#define calibration_factor -7050.0

int moistValue = 0;              // read moisture value 
int battery_value = 0;            //read the battery level. 

int dryLimit    = 420;      //most of these, the user should be able to set in app
int waterLimit  = 420;       
int EnclosureTemp = 30;     
float WeightValue = 35.90;      

int waterHour;//sync with IoT to store the variables.
int waterMinute;

int LEDHour;//sync with IoT to store the variables.
int LEDMinute;

void setup(){ 
  Serial.begin(9600);       // start the serial at 9600 baud
  clock.begin();            //TODO: beginning the clock every time it starts can mess with it. Need to have oscillating if statement
 
  pinMode(PUMP, OUTPUT);   // set relay pin to output
  pinMode(LED_STRIP, OUTPUT);     // set led pin to output

  //Weight Sensor Setup. 
  WeightSensor.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  WeightSensor.set_scale(calibration_factor);
  WeightSensor.tare();   //reset scale to 0 assuming no weight. 
  
}

void loop(){
  Display();              
  TempCheck();            
  WeightCheck();          
  BatteryLevel();
  LEDCheck();
  WaterPump();         
}


void WaterPump(){
  // Water Pump + Relay Function 
  moistValue = analogRead(MOISTURE_SENSOR);
  Serial.print("sensor 0 = ");
  Serial.print(moistValue);
  Serial.println("  ");
  if(moistValue >= dryLimit)
  {
    digitalWrite(PUMP, LOW);   
    Serial.print("Pump on"); 
    //Ticker function here.
    digitalWrite(PUMP, HIGH);    //pump high = off 
    Serial.print("Pump off");     
  }
  // Once a day at 11:00:01, water the plants and give it light regardless of moisture levels indicated
  clock.getTime();
  if(clock.hour == waterHour && clock.minute == waterMinute){
    digitalWrite(PUMP, LOW);   
    //Ticker function to be called here
    digitalWrite(PUMP, HIGH);    
  }
}
  


void LEDCheck(){
  //write a function to check the current time and minute then turn on led if the user set time is 
  clock.getTime();
  if(clock.hour == LEDHour && clock.minute == LEDMinute){
    digitalWrite(LED_STRIP,HIGH);
    //Ticker function
    digitalWrite(LED_STRIP,LOW);
}
void TempCheck(){
  //checks the temperature
  while(DHT.temperature > EnclosureTemp)
  {
    digitalWrite(BUZZER, HIGH); //buzzer goes off to warn that the temperature is too high! 
    Serial.println("Temperature is too high!");//To be displayed
    Serial.println(DHT.temperature); 
    Serial.println(DHT.humidity); 
    Display();
    digitalWrite(LED_STRIP, LOW); //Is these two lines a good idea?
    digitalWrite(PUMP, HIGH); //Basically means the user cannot use device if temp sensor is faulty.
  }
  digitalWrite(BUZZER, LOW);//Buzzer is turned off whenever while is not active

}



void WeightCheck(){
  //checks the weight of the water canister.
  float weight = WeightSensor.get_units()
  while (weight < WeightValue)
  {
    digitalWrite(BUZZER, HIGH); 
    Serial.println("Add more water to the water canister please!")//to be displayed 
    Display();
    digitalWrite(PUMP, HIGH); 
    digitalWrite(LED_STRIP, LOW); 
  }
  digitalWrite(BUZZER, LOW);
}

void BatteryLevel(){
   //reads the battery level and displays it.
  //likely will have to allocate a number of "bars" to a discrete battery level and display this by fault on E-INK display.
}

void Display(){
  //E-ink to be written by Rogier
  //display time and date by default via RTC
}

void Ticker(){
  //ticker to be written by Rob
  //its basically a delay function that doesnt affect the loop too much.
}