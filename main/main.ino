#define USE_ARDUINO_INTERRUPTS true    // Set-up low-level interrupts for most acurate BPM math.
#include <PulseSensorPlayground.h>     // Includes the PulseSensorPlayground Library.   

//  Variables
const int motorPin = 6;
const int vibThreshold = 110;
const int PulseWire = 0; 
const int LED13 = 13;
int Threshold = 515;
const int CountMeasures = 25;
int counter = 0;
const int GSR=SCL;
int sensorValue=0;
int gsr_average=0;
unsigned long time;
#include <SPI.h>
#include <SD.h>
PulseSensorPlayground pulseSensor;  // Creates an instance of the PulseSensorPlayground object called "pulseSensor"
File myFile;

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

  // open the file. note that only one file can be open at a time,
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

Serial.begin(9600);          // For Serial Monitor

  // Configure the PulseSensor object, by assigning our variables to it. 
  pulseSensor.analogInput(PulseWire);   
  pulseSensor.blinkOnPulse(LED13);       //auto-magically blink Arduino's LED with heartbeat.
  pulseSensor.setThreshold(Threshold);   

  // Double-check the "pulseSensor" object was created and "began" seeing a signal. 
   if (pulseSensor.begin()) {
    Serial.println("We created a pulseSensor Object !");  //This prints one time at Arduino power-up,  or on Arduino reset.  
  }
  pinMode(motorPin, OUTPUT);
}


void loop() {
  time = millis();
 const int countMeasures = 10;
   // Calls function on our pulseSensor object that returns BPM as an "int".
   long sum=0; //used for the GSr sensor"s averageing
  while(counter <= countMeasures) {
    int myBPM = pulseSensor.getBeatsPerMinute(); //holds the BPM value for now
    
    if (pulseSensor.sawStartOfBeat()) {            // Constantly test to see if "a beat happened". 
      Serial.println("♥  A HeartBeat Happened ! "); // If test is "true", print a message "a heartbeat happened".
      Serial.print("BPM: ");                        
      Serial.println(myBPM);                        
      counter++;
      sensorValue = analogRead(GSR);
      sum += sensorValue;
      }
    delay(10);   
} //end of while loop, writes data to SD card
gsr_average = sum/countMeasures;
Serial.println("Loop ended");
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
    myFile.println(gsr_average);
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  counter = 0;

}
