#include <Wire.h>
#include <ZumoMotors.h>
#include <ZumoReflectanceSensorArray.h>

float A=6000;
float B=76;

int AvantDistPin = 1;     
int ArriereDistPin =3;
int GaucheDistPin = 0;
int DroiteDistPin = 2;
int GaucheReflPin = 4;
int DroiteReflPin = 5;

int val = 0;           


long timer=0;
int vmax = 350;
int vright = 0;
int vleft = 0;

ZumoMotors motors;

void setup() {
 Serial.begin(9600);
 pinMode (DroiteReflPin, INPUT);
 pinMode (GaucheReflPin, INPUT);
 
}
void loop()
{
//  motors.setRightSpeed(vright);
//  motors.setLeftSpeed(vleft);
  delay(10);
 //Envoie les infos de la centrale
  timer = millis();
  Serial.print("!");
  Serial.print("AN: timer ");Serial.print(timer); 
  Serial.print(", vl ");Serial.print (vleft);
  Serial.print (", vr ");Serial.print (vright);
  Serial.print (", av");Serial.print (dist(AvantDistPin));
  Serial.print (", ar");Serial.print (dist(ArriereDistPin));
  Serial.print (", le");Serial.print (dist(GaucheDistPin));
  Serial.print (", ri");Serial.print (dist(DroiteDistPin));
  Serial.print (", Solleft");Serial.print (digitalRead(GaucheReflPin));
  Serial.print (", Solright");Serial.print (digitalRead(DroiteReflPin));
  Serial.println();  
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
  
  return mean;

}

