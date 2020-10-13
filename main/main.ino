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
const int CHECKING_HZ = 10;
const int VIB_THRESHOLD = 110;
const int COUNT_MEASURES = 10;

// Variables
int threshold = 515;
int sensorValue=0;
int gsrAverage=0;
int bpmAverage=0;
int checkingDelay = 1000/CHECKING_HZ;
unsigned long time;

PulseSensorPlayground pulseSensor;

/**
 * Writes to given file.
 *
 * This function finds a file based on the filename and then writes a specified given line to it.
 *
 * @param filename the filename of the file to write to, with extension
 * @param line the lines to write to a file
 */
void writeToFile(String filename, String line){
  File fileToWrite = SD.open(filename, FILE_WRITE);

  // if the file opened okay, write to it:
  if (fileToWrite) {
    String serialPrintString = "Writing to " + filename;
    Serial.print(serialPrintString);

    fileToWrite.println(line);
    // close the file:
    fileToWrite.close();

    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    String serialPrintString = "Error opening " + filename;
    Serial.println(serialPrintString);
  }
}

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

  writeToFile("test.txt", "T,BPM,GSR");

  // Configure the PulseSensor object, by assigning our variables to it. 
  pulseSensor.analogInput(HEARTPIN);   
  pulseSensor.blinkOnPulse(LED13);       //auto-magically blink Arduino's LED with heartbeat.
  pulseSensor.setThreshold(threshold);   

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
  long gsrSum=0; // used for the GSr sensor"s averageing
  long bpmSum=0;

  // Collecting data for a certain amount of loops. 
  // End of while loop, then we write data to SD card
  for(int i = 0; i < COUNT_MEASURES; i++) {
    if (i == 5) { // when the counter is 5, the while loop has had 5 * 100 seconds waited
      analogWrite(MOTORPIN, 0); // turn off the vibration motor
      Serial.println("LOW");
    }
    // Calls function on our pulseSensor object that returns BPM as an "int"
    int myBPM = pulseSensor.getBeatsPerMinute(); // holds the BPM value for now

    sensorValue = analogRead(GSR);
    gsrSum += sensorValue;
    bpmSum += myBPM;

    Serial.print("BPM:"); Serial.print(myBPM); Serial.print(","); Serial.print("GSR:"); Serial.println(sensorValue);
    
    // TODO: Have a delay for a more stable signal, maybe make this a variable?
    // Currently checks at CHECKING_HZ Hz
    delay(checkingDelay);
  }

  Serial.println("Loop ended");

  gsrAverage = gsrSum/COUNT_MEASURES / 10; // TODO: REMOVE /10 !!!
  bpmAverage = bpmSum/COUNT_MEASURES;

  int seconds = time/1000;

  String writeLine = String(seconds) + ", " + String(bpmAverage) + ", " + String(gsrAverage);
  writeToFile("test.txt", writeLine);

  // Make the vibration motor vibrate on a pulse
  // TODO: make variables of the numbers and make them correct
  if (bpmAverage > 90) {
     analogWrite(MOTORPIN, 255);
     Serial.println("HIGH");
  }
}
