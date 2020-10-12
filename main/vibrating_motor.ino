
//Code to vibrate the motor every minute
const int motorPin = 6;

void setup()
{
pinMode(motorPin, OUTPUT);
}

void loop()
{
digitalWrite(motorPin, HIGH);
delay(150);
digitalWrite(motorPin, LOW);
delay(100);
digitalWrite(motorPin, HIGH);
delay(150);
digitalWrite(motorPin, LOW);
delay(550);
}
