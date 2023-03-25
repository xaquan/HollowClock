#include <AccelStepper.h>
#include <Encoder.h>
// Motor pin definitions:
#define motorPin1  10      // IN1 on the ULN2003 driver
#define motorPin2  11      // IN2 on the ULN2003 driver
#define motorPin3  12     // IN3 on the ULN2003 driver
#define motorPin4  13     // IN4 on the ULN2003 driver

#define encPinCLK 3
#define encPinDT 4
#define encPinSW 2
#define btnPin 8
#define ledPin 6

#define MotorInterfaceType 8

#define MODE_RUN 0
#define MODE_ADJUST 1

int stepsPerRevolution = 4096;
float speedForMinute = stepsPerRevolution * 5 / 60; // 5 circle of 12 gear to complete the 60 teeth gear. Divide it to 60 seconds, the result is steps per second.
float speedForHour = speedForMinute / 60; //Slow the speed down to 60 times. 0

float speed = 5;

int moveTo = 0;

AccelStepper stepper = AccelStepper(MotorInterfaceType, motorPin1, motorPin3, motorPin2, motorPin4);


int mode = MODE_RUN;
int direction = -1;
long timeOfLastBlinking = 0;
int dealyOfBlinking = 500;
int ledState = LOW;

int encCLK;

void setup() {
  stepper.setMaxSpeed(1000);
  pinMode(ledPin, OUTPUT);
  pinMode(encPinSW, INPUT_PULLUP);
  pinMode(encPinCLK, INPUT);
  attachInterrupt(digitalPinToInterrupt(encPinCLK), encoder_detect, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encPinSW), button_detect, FALLING);
  
  Serial.begin(9600);

  encCLK = digitalRead(encPinCLK);
  // encDT = digitalRead(encPinDT);
}

void loop() {
  // put your main code here, to run repeatedly:

  if(mode == MODE_RUN){
    led(HIGH);
    speed = speedForHour;
    stepper.setSpeed(speed * direction);
    stepper.runSpeed();
    
  }else{
    led(getLedState());
    if(direction != 0){      
      speed = 1000;      
      stepper.setSpeed(speed * direction);
      stepper.runSpeed();
    }
  }    
}

int getLedState(){
  long currentTime = millis();
  // Serial.println(currentTime - timeOfLastBlinking );
  if(currentTime - timeOfLastBlinking > dealyOfBlinking){
    // Serial.println(ledState);
    led(ledState);
    ledState = !ledState;
    timeOfLastBlinking = currentTime;
  }

  return ledState;
}

void led(int state){
  digitalWrite(ledPin, state);
}

void encoder_detect ()  {
  if(mode == MODE_ADJUST){
    
    int currentCLK = digitalRead(encPinCLK);
    Serial.println(currentCLK);
    if(currentCLK != encCLK)
    {       
      if(digitalRead(encPinDT) != currentCLK)
      { 
        moveTo++;
        direction = -1;
      }
      else
      {
        moveTo--;
        direction = 1;
      }
    } 
      encCLK = currentCLK; 
  }
}

void button_detect()
{
  // mode = !mode;  
  Serial.println("Mode changing to");
  if(mode == MODE_ADJUST){
    Serial.println("Running");    
    direction = -1;    
    mode = MODE_RUN;
  }else{
    Serial.println("Adjusting");
    stepper.stop();
    direction = 0;
    mode = MODE_ADJUST;    
    timeOfLastBlinking = millis();
  }
}



