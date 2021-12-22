# Autonomous Plant Incubator Project

This is a repository that holds all the relevant libraries and files needed to make an Autonomous Plant Incubator using an Arduino Nano 33 IoT, a Moisture sensor, a Temperature & Humidity sensor, Water pump, Full spectrum LED strip and an E-Ink Display.

## Features
The Autonomous Plant incubator can do all of the following:
* Allow the user to set watering and lighting times
* Adjust the expected dryness for their specific Plant
* Water automatically dispenses if soil is too dry or it is the user-defined time.
* Emit Red/Blue light if it is the user-defined time.
* Display the realtime readings on the display as well as remotely on app.
* Warnings when Water or Battery is low
* Remotely control via Arduino IoT cloud

## Structure
The autoplant.cpp file holds all of the code that is run by the Arduino. It contains a setup() and loop() function.

The setup() function is ran when the Arduino is first turned on.
From then onwards, whatever is in the loop() is executed recursively.

Display() is a function written by Rogier Fransen, which is called at the start of the loop to update the E-Ink display with the relevant information. This shows useful information like the time, temperature, battery level, moisture level, water tank level, humidity, and if the device is connected to the internet.

BatteryLevel(), WeightCheck(), TempCheck(), LEDCheck(), WaterPump() and wifiConnection() all run continuous checks to see if they respectively call their own conditions to trigger a change.
For example, wifiConnection() will continuously look for a wifi signal and once it is found it will report it is connected.
Another example, If the lighting hours are met then LEDCheck() will turn on the LED strip for a number of ticks.

We have also used a ticker function that allows us to create 'delays' without interupting the flow of the program. An alternative to this would be to use Millis but in some cases that would require more lines of code, although potentially less variables to store tickers. This is an adaptation we might include in the future.

## Prerequisites
Please find the following prerequisites inside the repo:
*  DS1307.h & DS1307.cpp
*  HX711.h & HX711.cpp
*  constants.h
*  dht.h & dht.cpp
*  epd2in9.h & epd2in9.cpp
*  epdif.h & epdif.cpp
*  epdpaint.h & epdpaint.cpp
*  fonts.h font8.cpp, font12.cpp, font16.cpp, font20.cpp & font24.cpp
*  imagedata.h & imagedata.cpp
