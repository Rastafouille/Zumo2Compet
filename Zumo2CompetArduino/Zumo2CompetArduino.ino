#include <Wire.h>
#include <ZumoBuzzer.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>
#define LED 13
float A=6000;
float B=76;
int seuil=0;

// these might need to be tuned for different motor types
#define REVERSE_SPEED     150 // 0 is stopped, 400 is full speed
#define TURN_SPEED        150
#define FORWARD_SPEED     150
#define FORWARD_SPEED2    300
#define REVERSE_DURATION  400 // ms
#define TURN_DURATION     400 // ms

int AvantDistPin = 1;     
int ArriereDistPin =3;
int GaucheDistPin = 0;
int DroiteDistPin = 2;

int distAdv = 40;

int val = 0;           

long timer=0;
int vmax = 350;
int vright = 0;
int vleft = 0;

ZumoMotors motors;
ZumoBuzzer buzzer;
Pushbutton button(ZUMO_BUTTON); // pushbutton on pin 12
ZumoReflectanceSensorArray reflectanceSensors;

// Define an array for holding sensor values.
#define NUM_SENSORS 3
unsigned int sensorValues[NUM_SENSORS];
byte pins[] = {4, 11, 5};

void setup() {
 Serial.println("## INITIALISATION serial com");Serial.begin(9600); delay (2000);
 Serial.println("## INITIALISATION reflectance sensors");reflectanceSensors.init(pins, 3);
 buzzer.playNote(NOTE_G(4), 500, 9);  
 Serial.println("## INITIALISATION calibration");
 calibration ();
 seuil=350;
 buzzer.playNote(NOTE_G(4), 500, 9);  
 Serial.println("## Wait For Button to start");waitForButtonAndCountDown();
   Serial.println("## GOGOGO !!!");
  motors.setSpeeds(FORWARD_SPEED2, FORWARD_SPEED2);
  delay(300);
}
void loop()
{
  if (button.isPressed())
  {Serial.println("## STOP !");
    // if button is pressed, stop and wait for another press to go again
    motors.setSpeeds(0, 0);
    button.waitForRelease();
    Serial.println("## Wait For Button to restart");waitForButtonAndCountDown();
  }
  reflectanceSensors.readLine(sensorValues);
 //Envoie les infos de la centrale
  envoie();
  detectbordure(seuil);
  Serial.println(dist(AvantDistPin));
  if (dist(AvantDistPin)<=distAdv)
    //AVANCE
    {motors.setSpeeds(FORWARD_SPEED2, FORWARD_SPEED2);}
  else {detectadv();}
}

float dist(int pin)
{
   int n=0;
   float mean=0;
   long sensorsum=0;
   while (n<50)
    {n++;int sensorvalue=analogRead(pin);
    sensorsum=sensorsum+sensorvalue;
    }
   mean=A/(sensorsum/n-B);
  return mean; //en cm
}

void calibration()
{
  delay(500);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);        // turn on LED to indicate we are in calibration mode
  unsigned long startTime = millis();
  Serial.println("CALIBRATION...");
  while(millis() - startTime < 10000)   // make the calibration take 10 seconds
  {reflectanceSensors.calibrate();}
  digitalWrite(13, LOW);         // turn off LED to indicate we are through with calibration
  // print the calibration minimum values measured when emitters were on
  Serial.begin(9600);
  for (byte i = 0; i < NUM_SENSORS; i++)
  {Serial.print(reflectanceSensors.calibratedMinimumOn[i]);Serial.print(' ');}
  Serial.println();
  // print the calibration maximum values measured when emitters were on
  for (byte i = 0; i < NUM_SENSORS; i++)
  {Serial.print(reflectanceSensors.calibratedMaximumOn[i]);Serial.print(' ');}
  Serial.println();Serial.println();delay(1000);
  seuil=500;
}

void waitForButtonAndCountDown()
{
  digitalWrite(LED, HIGH);
  button.waitForButton();
  Serial.println("## Button pressed");
  digitalWrite(LED, LOW);
  // play audible countdown
  for (int i = 0; i < 3; i++)
  {delay(1000);buzzer.playNote(NOTE_G(3), 200, 9);}
  delay(1000);
  buzzer.playNote(NOTE_G(4), 500, 9);  
  delay(1000);
}

void envoie()
{
timer = millis();
  Serial.print("!");
  Serial.print("AN: timer ");Serial.print(timer); 
  Serial.print(", vl ");Serial.print (vleft);
  Serial.print (", vr ");Serial.print (vright);
  Serial.print (", av ");Serial.print (dist(AvantDistPin));
  Serial.print (", ar ");Serial.print (dist(ArriereDistPin));
  Serial.print (", le ");Serial.print (dist(GaucheDistPin));
  Serial.print (", ri ");Serial.print (dist(DroiteDistPin));
  Serial.print (", Solleft ");Serial.print (sensorValues[0]);
  Serial.print (", Solright ");Serial.print (sensorValues[2]);
  Serial.println();  
}
void detectbordure(int seuil)
{
if (sensorValues[0] > seuil)
  {
    // if leftmost sensor detects line, reverse and turn to the right
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
    delay(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
  else if (sensorValues[2] > seuil)
  {
    // if rightmost sensor detects line, reverse and turn to the left
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
    delay(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
  else
  {
    // otherwise, go straight
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
}

void detectadv ()
{
 while (dist(AvantDistPin)>=distAdv)
  //ROTATION
    {motors.setSpeeds(TURN_SPEED, -TURN_SPEED);delay(50);Serial.println(dist(AvantDistPin));}
}

