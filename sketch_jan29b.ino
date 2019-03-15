#include <SoftwareSerial.h>
SoftwareSerial SwSerial(0, 1); // RX, TX
#include <BlynkSimpleSerialBLE.h>
#include <Servo.h> 
//Blynk:
char auth[] = "27fd9cdd7d9941b8ae07c98ae6ed6b6c";
SoftwareSerial SerialBLE(0, 1); // RX, TX
int SLIDER = HIGH;
int MANUAL = LOW;
int servoPin = 3; 
int trigger=5;
int echo=4;
int tempPin = A0;
int potentiometerPin = A1;
int ledPin = 6;
int led2 = 13;

Servo Servo1; 

int controlState;
int milkState;
int sliderValue;
int manualValue;
int finalValue;
int servoAngle;
int initialMilkDistance = -1;
int safeMode;
//int temperature;

BLYNK_WRITE(V1)
{
  sliderValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("V1 Slider value is: ");
  Serial.println(sliderValue);
}

BLYNK_WRITE(V2)
{
  controlState = param.asInt();
  Serial.print("Mode: ");
  Serial.println(controlState);
  Serial.print("servoAngle");
  Serial.println(servoAngle);
}

BLYNK_WRITE(V3)
{
  milkState = param.asInt();
  Serial.print("Milk state ");
  Serial.println(milkState);
  int i=100;
  while(i--){
    checkDistance();
  }
  if(milkState == HIGH){
     initialMilkDistance = checkDistance();
  }else{
    initialMilkDistance = -1;
  }
  Serial.println(initialMilkDistance);
}
BLYNK_WRITE(V4)
{
  safeMode = param.asInt();
}
unsigned long t = 0;
long checkDistance() {
  // put your main code here, to run repeatedly:
  long d,t;
  digitalWrite(trigger,LOW);
  t = millis();
  while(millis()<t +10){}
  digitalWrite(trigger,HIGH);
  t = millis();
  while(millis()<t +10){}
  digitalWrite(trigger,LOW);
  t=pulseIn(echo,HIGH);
  d=340*t/20000;
  //Serial.print("Distance: ");
  //Serial.println(d);
  return d;

}
int calculateTemp(){
   int temp = analogRead(tempPin);
   // read analog volt from sensor and save to variable temp
   temp = temp * 0.48828125;
   return temp;
}
void setup()
{
  // Debug console
  Serial.begin(9600);
  SerialBLE.begin(9600);
  Blynk.begin(SerialBLE, auth);

  Serial.println("Waiting for connections...");

  Servo1.attach(servoPin); 
  pinMode(ledPin,OUTPUT);
  pinMode(led2,OUTPUT);
  pinMode(potentiometerPin, INPUT);
  pinMode(trigger,OUTPUT);
  pinMode(echo,INPUT);
}
unsigned long timer=0;
void loop()
{
  Blynk.run();
  //checkDistance();
  if(safeMode){
  timer=millis();
  Serial.println(calculateTemp());
  while(safeMode && servoAngle<180 && millis()<10000+timer && calculateTemp()<50){
    Serial.print("M");
      Serial.println(millis());
      Serial.print("t");
      Serial.println(10000+timer);
      Serial.println(calculateTemp());
  }
  if(millis()-timer>10000){
    Servo1.write(180);
    servoAngle=180;
    Blynk.virtualWrite(V1,0);
  }
 }
 else if(milkState){
    
    Serial.println(checkDistance() );
    if(checkDistance() < initialMilkDistance - 3 ){
      Serial.println("check");
      Servo1.write(180);
      servoAngle = 180;
      Blynk.virtualWrite(V1,255);
    }
  }
  
  else if(controlState == SLIDER){
    servoAngle = map(sliderValue, 0, 255, 0, 180);
    servoAngle = 180 - servoAngle;
  }else{
    int potentiometerValue = analogRead(A1);
    manualValue = map(potentiometerValue, 0, 1024, 0, 255);
    servoAngle = map(manualValue, 0, 255, 0, 180);
  }
  Servo1.write(servoAngle);
}
