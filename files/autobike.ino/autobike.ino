#include <LiquidCrystal_I2C.h>
#define reed A0//pin connected to read switch

LiquidCrystal_I2C lcd(0x3F, 16, 2);

#include <Servo.h>

Servo servo1;  // create servo object to control a servo
Servo servo2;
// twelve servo objects can be created on most boards

int pos1, pos2, gear, newGear;

float radius = 7.415;// tire radius (in inches)- CHANGE THIS FOR YOUR OWN BIKE

int pos = 0;
int reedVal;
long time = 0;// time between one full rotation (in ms)
float rpm = 0.00;
float rpm1, rpm2, rpm3;
float circumference;
boolean backlight;
int xCorrect;

int maxReedCounter = 100;//min time (in ms) of one rotation (for debouncing)
int reedCounter;

//#define seven1 180
//#define seven2 80
//#define six1 167
//#define six2 71
//#define five1 154
//#define five2 62
//#define four1 141
//#define four2 53
//#define three1 128
//#define three2 44
//#define two1 115
//#define two2 35
//#define one1 102
//#define one2 26


void setup() {

  reedCounter = maxReedCounter;
  circumference = 2 * 3.14 * radius;
  pinMode(reed, INPUT);

  //checkBacklight();

  Serial.write(12);//clear

  // TIMER SETUP- the timer interrupt allows preceise timed measurements of the reed switch
  //for mor info about configuration of arduino timers see http://arduino.cc/playground/Code/Timer1
  cli();//stop interrupts

  // TIMER SETUP- the timer interrupt allows preceise timed measurements of the reed switch
  //for mor info about configuration of arduino timers see http://arduino.cc/playground/Code/Timer1
  cli();//stop interrupts

  //set timer1 interrupt at 1kHz
  TCCR2A = 0;// set entire TCCR1A register to 0
  TCCR2B = 0;// same for TCCR1B
  TCNT3  = 0;//initialize counter value to 0;
  // set timer count for 1khz increments
  OCR3B = 1999;// = (16*10^6) / (1000*8) - 1
  // turn on CTC mode
  TCCR3B |= (1 << WGM12);
  // Set CS11 bit for 8 prescaler
  TCCR3B |= (1 << CS11);   
  // enable timer compare interrupt
  TIMSK3 |= (1 << OCIE3B);
  
  sei();//allow interrupts
  //END TIMER SETUP

  Serial.println("Setup completed");

  lcd.init(); // initialize the lcd
  lcd.setBacklight(2);
  Serial.begin(9600);

  servo1.attach(34);
  servo2.attach(52);

//  pos1 = servo1.read();
//
//  switch (pos1) {
//    case seven1:
//      gear = 7;
//      break;
//    case six1:
//      gear = 6;
//      break;
//    case five1:
//      gear = 5;
//      break;
//    case four1:
//      gear = 4;
//      break;
//    case three1:
//      gear = 3;
//      break;
//    case two1:
//      gear = 2;
//      break;
//    case one1:
//      gear = 1;
//      break;
//  }

  //gear = 7;
//  servo1.write(180);
//  servo2.write(80);
//
//  for (int i = 0; i <= 7; i++){
//    if (i <= 5){
//      delay(1500);
//      servo1.write(180 - (i * 9)); // started as 25 -- too far
//      servo2.write(80 - (i * 9)); // started as 11 -- too far
//      delay(300);
//      servo1.write(180 - (i * 6)); // started as 25 -- too far
//      servo2.write(80 - (i * 6)); // started as 11 -- too far
//    }
//    if (i > 5){
//      delay(1500);
//      servo1.write(180 - (i * 7)); // started as 25 -- too far
//      servo2.write(80 - (i * 7)); // started as 11 -- too far
//      delay(100);
//      servo1.write(180 - (i * 6)); // started as 25 -- too far
//      servo2.write(80 - (i * 6)); // started as 11 -- too far
//    }
//  }
  gear = 7;
}

ISR(TIMER3_COMPB_vect) {//Interrupt at freq of 1kHz to measure reed switch
//ISR(TIMER1_COMPB_vect)  {
  reedVal = digitalRead(reed);//get val of A0
  if (reedVal) { //if reed switch is closed
    if (reedCounter == 0) { //min time between pulses has passed
      rpm = (60000/float(time)); //calculate miles per hour
      time = 0;//reset timer
      reedCounter = maxReedCounter;//reset reedCounter
    }
    else {
      if (reedCounter > 0) { //don't let reedCounter go negative
        reedCounter -= 1;//decrement reedCounter
      }
    }
  }
  else { //if reed switch is open
    if (reedCounter > 0) { //don't let reedCounter go negative
      reedCounter -= 1;//decrement reedCounter
    }
  }
  if (time > 2000) {
    rpm = 0;//if no new pulses from reed switch- tire is still, set mph to 0
  }
  else {
    time += 1;//increment timer
  }
}

float displayRPM_GEAR() {
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print(String(rpm) + " RPM");
  lcd.setCursor(2, 1);
  lcd.print("Gear: " + String(gear));
  return rpm;
}

void upshift(int x) {
    xCorrect = x;
    if (x < 0){
      xCorrect = 0;
    }
    if (x > 7){
      xCorrect = 7;
    }
    if (xCorrect <= 5) { // && (x >= 0)){
      servo1.write(130 - ((7-x) * 9)); // started as 25 -- too far
      servo2.write(80 - ((7-x) * 9)); // started as 11 -- too far
      delay(100);
      servo1.write(130 - ((7-x) * 8)); // started as 25 -- too far
      servo2.write(80 - ((7-x) * 8)); // started as 11 -- too far
    }
    if (xCorrect > 5){// && (x <= 7)){
      servo1.write(180 - ((7-x) * 8)); // started as 25 -- too far
      servo2.write(130 - ((7-x) * 8)); // started as 11 -- too far
      delay(100);
      servo1.write(180 - ((7-x) * 8)); // started as 25 -- too far
      servo2.write(130 - ((7-x) * 8)); // started as 11 -- too far
    }
    
    gear = xCorrect;
    Serial.println(String(gear));
//  servo1.write(102 + ((x) * 13)); // started as 25 -- too far
//  servo2.write(26 + ((x) * 9)); // started as 11 -- too far
}

void downshift(int x) {

    servo1.write(180 - ((7-x) * 25)); // started as 25 -- too far
    servo2.write(130 - ((7-x) * 18)); // started as 11 -- too far

    gear = x;
    Serial.println(String(gear));
//  servo1.write(102 + ((x) * 13)); // started as 25 -- too far
//  servo2.write(26 + ((x) * 9)); // started as 11 -- too far
}

void loop() {

  delay(3000);
  rpm = displayRPM_GEAR();

  if ((rpm <= 35) && (rpm != 0)){ //if in a high gear and rpms are decreasing
    newGear = gear-1;
    Serial.print("downshift");
    if (newGear >= 0) {
      downshift(newGear);
    }
  }
  if (rpm >= 70){ //if in a low gear and rpms are increasing
    newGear = gear+1;
    Serial.print("upshift");
    if (newGear <= 7) {
      downshift(newGear);
    }
  }

//servo2.write(130); 7TH GEAR
//servo1.write(180); 7TH GEAR

}
