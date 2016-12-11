//Lahetyspaa
//Version: 0.1
//Date: 11.12.2016

#include "LedControl.h" //  library for controlling MAX7219 controller
LedControl lc = LedControl(10,11,12,1); // Setup MAX7219 controller to right output pins
unsigned long startTime = 0;
volatile unsigned long firstPortTime = 0;
volatile unsigned long secondPortTime = 0;
double timeElapsed = 0.0;
volatile boolean firstPortActivated = false;
volatile boolean secondPortActivated = false;


void setup(){
  //Digital 2 pin that is used to detect the rising edge of pulse
  attachInterrupt(0, detectRising, RISING);
  //Digital 3 pin that is used to detect the falling edge of pulse
  attachInterrupt(1, detectFalling, FALLING);
  lc.shutdown(0,false);// turn off power saving, enables display
  lc.setIntensity(0,15);// sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen
  initLights();
  showHello();
  countdown();

  Serial.begin(9600); //Serial communications
}

void loop(){
  firstPortActivated = false;
  secondPortActivated = false;
  firstPortTime = 0;
  secondPortTime = 0;
  digitalWrite(7,HIGH);
  digitalWrite(6,LOW);
  digitalWrite(5,LOW);
  digitalWrite(9,LOW);
  digitalWrite(8,LOW);

  float buttonVoltage = 0.0;
  float buttonValue = 0.0;
  //Start button that is connected to A0
  //When voltage is higher than 2.5V button was pressed
  do{
    buttonValue = analogRead(A0);
    buttonVoltage = buttonValue * (5.0/1023.0);
  }while(buttonVoltage <= 2.5);

  //Start countdown
  countdown();

  //Take timestamp of starting time
  startTime = millis();
  delay(20);
  firstPortTime = 0;
  secondPortTime = 0;

  //Do if both ports are activated at the same time
  do{
    firstPortActivated = false;
    secondPortActivated = false;
    delay(10);
    //Wait for first port to be activated
    while(firstPortActivated == false){
      //Show new time once every 10 ms
      delay(10);
      timeElapsed = millis();
      timeElapsed = timeElapsed - startTime;
      timeElapsed = timeElapsed / 1000;
      showTime(timeElapsed);
      //Check if stop button (A1) is pressed
      buttonValue = analogRead(A1);
      buttonVoltage = buttonValue * (5.0/1023.0);
      if(buttonVoltage >= 2.5){
        return;
      }
    }

    //Set false so second port activation before first port is ignored
    secondPortActivated = false;

    //Wait for second port to be activated
    while(secondPortActivated == false){
      //Show new time once every 10 ms
      /*delay(10);
      timeElapsed = millis();
      timeElapsed = timeElapsed - startTime;
      timeElapsed = timeElapsed / 1000;
      showTime(timeElapsed);
      */
      //Check if stop button (A1) is pressed
      buttonValue = analogRead(A1);
      buttonVoltage = buttonValue * (5.0/1023.0);
      if(buttonVoltage >= 2.5){
        return;
      }
    }
  }while(secondPortTime == firstPortTime);

  digitalWrite(8,LOW);
  delay(10);

  //Only if port activation times are different then show time and speed
  if(secondPortTime > firstPortTime){
    //Calculate travel time
    //Time between start and first port in seconds
    double travelTime = firstPortTime - startTime;
    travelTime /= 1000;
    showTime(travelTime);


    double timeDifference = secondPortTime - firstPortTime;
    double speed = calculateSpeed(timeDifference); //Calculate speed

    showSpeed(speed);

  }

  firstPortActivated = false;
  secondPortActivated = false;
}

//Function for displaying time on screen
void showTime(double& time){
  int temp;
  //Time calculations
  //Jos tulos on > 99.99 niin otetaan 100:n jakojäännös
  if(time > 99.99){
    temp = (int)time / 100;
    time = time - (temp*100);
    lc.setDigit(0,3,temp,0);

    int timeInt = time /10;
    lc.setDigit(0,2,timeInt,0);
    timeInt = time - (timeInt * 10);
    if(timeInt >= 0 && timeInt < 10){
      //Serial.println(timeInt);
      lc.setDigit(0,1,timeInt,1);
    }

    timeInt = (time - (int)time ) * 100;
    temp = timeInt / 10;
    //Serial.println(temp);
    lc.setDigit(0,0,temp,0);
  }
  else{
    int timeInt = time /10;
    //Serial.println(timeInt);
    lc.setDigit(0,3,timeInt,0);
    timeInt = time - (timeInt * 10);
    if(timeInt >= 0 && timeInt < 10){
      //Serial.println(timeInt);
      lc.setDigit(0,2,timeInt,1);
    }

    timeInt = (time - (int)time ) * 100;
    temp = timeInt / 10;
    //Serial.println(temp);
    lc.setDigit(0,1,temp,0);
    temp = timeInt % 10;
    //Serial.println(temp);
    lc.setDigit(0,0,temp,0);
  }

}

//Shows speed that is given by parameter speed in km/h
void showSpeed(double& speed){
  Serial.print("Speed: ");
  Serial.println(speed);

  int speedInt = (int)speed;
  //Speed calculations
  double lastRoundedDigit = speed - (double)speedInt;
  if(lastRoundedDigit >= 0.5){
    speedInt = speedInt + 1;
  }
  int temp = speedInt / 100;
  int temp1;
  if(speedInt >= 100){
    lc.setDigit(0,6,temp,0);
  }
  temp1 = speedInt - (temp*100);
  temp1 = temp1 / 10;
  if(speed >= 10){
    lc.setDigit(0,5,temp1,0);
  }
  temp = speedInt - (temp * 100) - (temp1 * 10);
  lc.setDigit(0,4,temp,0);

}

//Prings "hello" on to the screen with delay to each letter
void showHello(){
  lc.clearDisplay(0);// clear screen
  //delay(1000);
  lc.setChar(0,3,'H',0);
  delay(1000);
  lc.setChar(0,2,'E',0);
  delay(1000);
  lc.setChar(0,1,'L',0);
  delay(1000);
  lc.setChar(0,0,'L',0);
  delay(1000);
  lc.setDigit(0,6,0,0);
}

//Interrupt function for the first gate activation
void detectRising(){
  firstPortActivated = true;
  firstPortTime = millis();
}

//Interrupt function for the second gate activation
void detectFalling(){
  secondPortActivated = true;
  secondPortTime = millis();
}

//Function for lighting the "christmastree" -lights
//The display is also set to show a countdown sequence from 5 to 1
void countdown(){
  lc.clearDisplay(0);// clear screen
  digitalWrite(7,LOW);
  digitalWrite(6,LOW);
  digitalWrite(5,LOW);
  digitalWrite(9,LOW);
  digitalWrite(8,LOW);

  for(unsigned int i = 5;  i > 0; --i ){
    delay(1000);
    for(unsigned int j = 0; j < 7; ++j){
      lc.setDigit(0,j,i,0);
    }
    if(i == 3){
      digitalWrite(6,HIGH);
    }
    else if(i == 2){
      digitalWrite(5,HIGH);
    }
    else if(i == 1){
      digitalWrite(9,HIGH);
    }
  }
  delay(1000);
  digitalWrite(6,LOW);
  digitalWrite(5,LOW);
  digitalWrite(9,LOW);
  digitalWrite(7,HIGH);
  digitalWrite(8,HIGH);

  lc.clearDisplay(0);// clear screen

}

//Initialise christmastree lights
void initLights(){
  //Light bulbs connected to pins 5-9
  //3rd from the top
  pinMode(5, OUTPUT);
  //2nd from the top
  pinMode(6, OUTPUT);
  //1st from the top
  pinMode(7, OUTPUT);
  //lowest
  pinMode(8, OUTPUT);
  //4th from the top
  pinMode(9, OUTPUT);
}

//Function calculates speed in km/h from milliseconds elapsed between 1m distance
double calculateSpeed(double& timeDifference){
  timeDifference = timeDifference / 1000; //time difference from millis to seconds
  timeDifference = timeDifference / 3600; //time difference from seconds to hours
  double distance = 0.001; //Distance between sensors in km
  double speed = distance / timeDifference;
  return speed;
}
