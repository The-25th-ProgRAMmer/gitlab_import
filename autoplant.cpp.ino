/*starting code. Feel free to comment 
and give me pointers on where I should change it and why it doesn't work*/

#include <Wire.h>
#include "DS1307.h" //add library for RTC DONE! 
#include <dht.h> //add library for the temperature + moisture sensor DONE!
#include "HX711.h" //add library for the weight sensor DONE!

DS1307 clock;               //define a object of DS1307 class
dht DHT;                  //define a temperature object. 
HX711 WeightSensor;        //define a weight sensor object. 

//we can calibrate the sensor but for now, I will use a placeholder figure that we can change later on. 
#define calibration_factor -7050.0


const int RELAY      = 2;        // Relay Pin to turn the pump on set to D2
const int BUZZER     = 7;        // Buzzer set to D7
const int MOISTURE_SENSOR = A0;  // Moisture Sensor set to A0

const int LED_STRIP = 6;         //LED STRIP on and off  
const int TEMP_SENS = 5;         //temperature pin set to D5

const int WEIGHT_CLK = A4;        //CLK Pin and Data out pin to the nanoboard. 
const int WEIGHT_DOUT = A5; 

int moistValue = 0;              // read moisture value 
int battery_value = 0;            //read the battery level. 


int dryLimit    = 420;      // how dry you will allow your plants to be on average
int waterLimit  = 420;      //how wet we will have the enclosure 
int EnclosureTemp = 30;     //how hot we will have the enclosure 
float WeightValue = 35.90;      //place value which we will test and change. 

void setup()
{
  //some of the intiliasation can be done in seperate files to keep the main part of the code clean. 
  
  Serial.begin(9600);       // start the serial at 9600 baud
  clock.begin();            // start reading the RTC
  //setup stuff can be done in a header file to keep the code clean. 
  pinMode(RELAY, OUTPUT);   // set relay pin to output
  pinMode(LED_STRIP, OUTPUT);     // set led pin to output

  //Weight Sensor Setup. 
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare();   //reset scale to 0 assuming no weight. 
  
}

void loop()
{
  WaterPump();             // Water Pump + Relay Function 
  TempCheck();            //checks the temperature
  WeightCheck();         //checks the weight of the water cannister. 
  //BatteryLevel();         //reads the battery level and displays it. 
}


void WaterPump()
{
  clock.getTime(); //grab the time from the RTC every clock cycle

  // Check every minute to see that the moisture level is where you want it, water if it's too dry
  switch (clock.second) // When the time on the RTC reads a value in seconds
  {
    case 30: // at the 30 second mark
      delay(2);
      // read the analog value of the first sensor:
      moistValue = analogRead(MOISTURE_SENSOR);
      // print the results to the serial monitor:
      Serial.print("sensor 0 = ");
      Serial.print(moistValue);
      Serial.println("  ");
      delay(2);

      // if the plants are on average too dry, run the pump for 10 seconds.
      if(moistValue >= dryLimit)
      {
        digitalWrite(LED_STRIP, HIGH);     // turn the LED on (HIGH is the voltage level)
        digitalWrite(RELAY, LOW);   // turn the Relay on 
        delay(10000);                // wait for 10 seconds
        digitalWrite(RELAY, LOW);    // turn the Relay off by making the voltage HIGH
        digitalWrite(LED_STRIP, LOW);      // turn the LED off by making the voltage LOW
        delay(1000);                 // wait for 1 second
      }
      delay(1000); // let the second pass to avoid multiple readings over serial
  }

  // Once a day at 11:00:01, water the plants and give it light regardless of moisture levels indicated
  switch (clock.hour)
  {
    case 11: //at 11AM
      switch (clock.minute)
      {
        case 0: //at 0 minutes
          switch (clock.second)
          {
            case 1: //at 1 second
              digitalWrite(LED_STRIP, HIGH);     // turn the LED on (HIGH is the voltage level)
              digitalWrite(RELAY, LOW);   // turn the Relay on
              delay(10000);                // wait for 10 seconds
              digitalWrite(LED_STRIP, LOW);      // turn the LED off by making the voltage LOW
              digitalWrite(RELAY, HIGH);    // turn the Relay off. 
              delay(1000);                 // wait for 1 second
          }
      }
  }
}

void TempCheck()
{
  if (DHT.temperature > EnclosureTemp)//change value depending on what we find in testing. 
  {
      digitalWrite(BUZZER, HIGH); //buzzer goes off to warn that the temperature is too high! 
      Serial.println("Temperature is too high!");
      Serial.println(DHT.temperature); 
      Serial.println(DHT.humidity); 
      
      //Rogier can programme the E-ink Sectiom. 
  
      //cut off nutrient supply of water and light. 
      digitalWrite(LED_STRIP, LOW); 
      digitalWrite(RELAY, HIGH); 
  }
  else
  {
    //once the temp is normal, cut the buzzer and resume normal service. 
    digitalWrite(BUZZER, LOW);
    Serial.println("Temperature is ok");
  }
}



void WeightCheck()
{
      float weight = scale.get_units()

      //if the water bottle isn't heavy enough, it won't have enough water to seed the plants. 
      if (weight < WeightValue)
      {
        digitalWrite(BUZZER, HIGH); 
        Serial.println("Add more water to the water cannister please!") 
        //Display this on the E-ink display too. 

        //cut off water supply obviously 
        digitalWrite(RELAY, HIGH); 
        digitalWrite(LED_STRIP, LOW); 
      }
      else 
      {
        digitalWrite(BUZZER, HIGH); 
        Serial.println("Water Bottle is ok!"); 
      }
}