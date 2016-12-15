//Vastaanottopaa
//Version: 0.1
// Date: 15.12.2016

boolean firstPortActivated = false;
boolean secondPortActivated = false;

void setup()
{
  attachInterrupt(0, firstPort, RISING);  //Interrupt for firt gate on Digital 2-pin
  attachInterrupt(1, secondPort, RISING); //Interrupt for second gate on Digital 3-pin
  pinMode(8, OUTPUT); //Pin 8 works as the signal to lähetyspää

  pinMode(12,OUTPUT); //LED for first gate
  pinMode(13,OUTPUT); //LED for second gate
  //Serial.begin(9600);
}

void loop(){
  //Wait for first port to activate
  while(!firstPortActivated){
    delay(1);
  }
  digitalWrite(8, HIGH); //START PULSE Pulse signal = HIGH

  secondPortActivated = false; //Make sure second port is not yet activated

  unsigned long currentTime =  millis();
  //If crossing gates takes more than three seconds
  //Send crossing signal to display slowest speed of 1 km/h (= 1/3 m/s)
  unsigned long firstPortTime = millis();
  firstPortTime += 3000;
  while(!secondPortActivated){
    if(currentTime > firstPortTime){
      secondPortActivated = true;
    }
    currentTime = millis();
  }

  digitalWrite(8, LOW); //END PULSE Pulse signal = LOW
  firstPortActivated = false;
  secondPortActivated = false;
}

void firstPort(){
  //Debug lights
  digitalWrite(13, HIGH);
  digitalWrite(12, LOW);
  firstPortActivated = true;
}

void secondPort(){
  //Debug lights
  digitalWrite(12, HIGH);
  digitalWrite(13, LOW);
  secondPortActivated = true;
}
