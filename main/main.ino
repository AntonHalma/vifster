#define USE_ARDUINO_INTERRUPTS true    // Set-up low-level interrupts for most acurate BPM math.

#include <PulseSensorPlayground.h>
#include <SPI.h>
#include <SD.h>

// Pin variables
const int MOTORPIN = 6;
const int HEARTPIN = 0; 
const int GSR=SCL;
const int LED13 = 13;

// Calculation constants
const int VIB_THRESHOLD = 110;
const int COUNT_MEASURES = 25;

// Variables
int threshold = 515;
int counter = 0;
int sensorValue=0;
int gsrAverage=0;
unsigned long time;

PulseSensorPlayground pulseSensor;
File myFile;

// Setup, runs once each time the device is started
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  // open the file. Only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("T,BPM,GSR");
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }

  // Configure the PulseSensor object, by assigning our variables to it. 
  pulseSensor.analogInput(HEARTPIN);   
  pulseSensor.blinkOnPulse(LED13);       //auto-magically blink Arduino's LED with heartbeat.
  pulseSensor.setthreshold(threshold);   

  // Double-check the "pulseSensor" object was created and "began" seeing a signal. 
   if (pulseSensor.begin()) {
    Serial.println("We created a pulseSensor object!");  //This prints one time at Arduino power-up,  or on Arduino reset.  
  }

  // Set the pin controlling the vibration motor to an output
  pinMode(MOTORPIN, OUTPUT);
}

// this function loops while the device is running
void loop() {
  time = millis();
  const int COUNT_MEASURES = 10;
  long sum=0; // used for the GSr sensor"s averageing

  // Collecting data for a certain amount of loops. 
  // End of while loop, then we write data to SD card
  while(counter <= COUNT_MEASURES) {
    // Calls function on our pulseSensor object that returns BPM as an "int"
    int myBPM = pulseSensor.getBeatsPerMinute(); //holds the BPM value for now
    
    if (pulseSensor.sawStartOfBeat()) {            // Constantly test to see if "a beat happened". 
      Serial.println("♥  A HeartBeat Happened ! "); // If test is "true", print a message "a heartbeat happened".
      Serial.print("BPM: ");                        
      Serial.println(myBPM);                        
      counter++;
      sensorValue = analogRead(GSR);
      sum += sensorValue;
      }
    
    // Have a delay for a more stable signal, maybe make this a variable?
    delay(10);

  }

  Serial.println("Loop ended");
  gsrAverage = sum/COUNT_MEASURES;
  Serial.println(time);

  myFile = SD.open("test.txt", FILE_WRITE);
  if (myFile) {
    int myBPM = pulseSensor.getBeatsPerMinute();
    int seconds = time/1000;
    Serial.print("Writing BPM to test.txt...");
    myFile.print(seconds);
    myFile.print(",");
    myFile.print(myBPM);
    myFile.print(",");
    myFile.println(gsrAverage);
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  counter = 0;

}
