const int blueLed = 9;           // the pin that the LED is attached to
const int greenLed = 10;
const int redLed = 11;

const int motionPin1 = A0;
const int motionPin2 = A1;

const int detectPin = 3;


//stuff to change around
const int detectNothingThresh = 60; //for off
const int detectFarThresh = 100; //for blue/red
const int detectMaxThresh = 500; //for max blinkage withing falling off the cliff
const int detectCloseThresh = 400; //for stacks

int motionValue1 = 0;
int motionValue2 = 0;
int mostMotionValue = 0;

int detectVal = 1;

int currentLed = 9;

bool increasing = true;

int detectStacks = 0;

//idk if needed, we'll see...
bool objectAt1;
bool objectAt2;

float multi = 1;

float brightness = 0;
int redBrightness = 255;
int greenBrightness = 255;
int blueBrightness = 255;

int redAim;
int greenAim;
int blueAim;

float redBlueFadeCurrent; //in percent 0 is red, 100 is blue
int redBlueFadeAim;

int fadeFrom;
int fadeTo;

int nothingDelay = 0;
int blueDelay = 0;
int atMaxDelay = 0;
int pirDelay = 0;

int stackLossDelay = 0;
int stackGainDelay = 0;
int stackDecayDelay = 0;


int maxi = 0;


void setup() {
  // put your setup code here, to run once:
  pinMode(blueLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  
  pinMode(detectPin, INPUT);
  //pinMode(motionPin, INPUT);
  analogWrite(greenLed, 255);
  analogWrite(blueLed, 0); //we are starting blue
  redBlueFadeCurrent = 0;
  analogWrite(redLed, 255);
  
  Serial.begin(9600);
  
  
}

void loop() {
  // put your main code here, to run repeatedly:
  motionValue1 = analogRead(motionPin1);
  
  motionValue2 = analogRead(motionPin2);
  detectVal = digitalRead(detectPin);
  
  getMostMotionValue(); 
  
  //***max of distace (8cm)
  if(mostMotionValue > detectMaxThresh)
  {
    atMaxDelay = 100;

  }  
  atMaxDelay--;
  
  
  //***pir sense
  if(!detectVal)//somethings moving
  {
    pirDelay = 250;
  }
  pirDelay--;
  
  
  if(mostMotionValue > detectFarThresh)
  {
    blueDelay = 100;
  }
  blueDelay--;
  
  if(mostMotionValue > detectNothingThresh)
  {
    nothingDelay = 100;
  }
  nothingDelay--;
  
  checkStacks();
  
  //big display logic
  if(blueDelay <= 0) //nothing's there
  {
    blueDelay = 0;
    
    //turnOffLed(true,true,false);//deprec due to fadecolor
    currentLed = blueLed;   
    redBlueFadeAim = 0;
    stackDecay();
    multi = 0; //1 is added later replaced by a decay
    
    if(atMaxDelay<=0)//fix overflow
    {
      atMaxDelay = 0;
    }
    
  }
  else
  {
    //turnOffLed(false,true,true); // deprec due to fadecolor
    currentLed = redLed;
    redBlueFadeAim = 100;
    if(atMaxDelay<=0)
    {
      multi = mostMotionValue / 200;
      atMaxDelay = 0;
    }
    else
    {
      multi = 3;//hard code max
    }
    
  }
  if(detectStacks > 20)//limiter so it doesn't get annoying
  {
    detectStacks = 20;
  }
  multi += (detectStacks) + 1;//plus 1 just cuz
  
  breath(multi);
  
  
  
  if(pirDelay<=0) //avoiding overflow
  {
    pirDelay = 0;
  }
  if(nothingDelay <= 0)//avoiding overflow
  {
    nothingDelay = 0;
  }
  
  if(pirDelay<=0 && nothingDelay <= 0)
  {
    pirDelay = 0;
    nothingDelay = 0;
    turnOffLed(true,true,true);
  }
  else
  {
    //analogWrite(currentLed, brightness);
    fadeColor(brightness);
  }
  //turnOffLed(false,true,false);
  
  //Serial.println(brightness);

  
  Serial.print(multi);
  Serial.print(":");
  Serial.print(brightness);
  Serial.print(":");
  Serial.print(redBrightness);
  Serial.print(":");
  Serial.print(blueBrightness);
  Serial.print("\n");
  

  //Serial.println(detectValue);
  delay(50);
  

}

void breath(float multiplier)
{
  int sign;
  if(increasing)
  {
    sign = 1;
  }
  else
  {
    sign = -1;
  }
  
  if(brightness > 150)
  {
    brightness += (10*multiplier*sign);//1
  }
  else if((brightness >125) && (brightness <151))
  {
    brightness += (9*multiplier*sign); //2
  }
  else if((brightness >100) && (brightness <126))
  {
    brightness += (7*multiplier*sign); //4
  }
  else if((brightness >75) && (brightness <101))
  {
    brightness += (5*multiplier*sign); //5
  }
  else if((brightness >50) && (brightness <76))
  {
    brightness += (4*multiplier*sign); //7
  }
  else if((brightness >25) && (brightness <51))
  {
    brightness += (3*multiplier*sign); //9
  }
  else if((brightness >= 0) && (brightness <26))
  {
    brightness += (2*multiplier*sign); //10
  }
  
  
  if(brightness >= 255)
  {
    brightness = 255;
    increasing = false;
  }
  if(brightness <= 0)
  {
    brightness = 0;
    increasing = true;
  }
  
  
  
}

void turnOffLed(bool red, bool green, bool blue)
{
  if(red)
  {
    
    
    analogWrite(redLed, 255);
  }
  if(green)
  {
    analogWrite(greenLed, 255);
  }
  if(blue)
  {
    analogWrite(blueLed, 255);
  }
  
}

void getMostMotionValue()
{
  mostMotionValue = motionValue1;
  if(motionValue1 < motionValue2)
  {
    mostMotionValue = motionValue2;
  }  
}

void checkStacks()
{
  
  if(mostMotionValue > detectCloseThresh || (!detectVal))
  {
    stackLossDelay = 150;
    
  }
  if(stackLossDelay <= 0)
  {
    stackDecay();
    stackLossDelay = 0;
    if(stackGainDelay <= 0)
    {
      stackGainDelay = 0;
    }
  }
  else
  {
    if(stackGainDelay <= 0)
    {
      detectStacks++;
      stackGainDelay = 75;
    }
  }
  
    
  stackLossDelay--;
  stackGainDelay--;

}

void stackDecay()
{
  if(stackDecayDelay <= 0)
  {
    detectStacks *= .75;
    stackDecayDelay = 50;
  }
  stackDecayDelay--;
}

void fadeColor(float bright)
{
  if(redBlueFadeCurrent != redBlueFadeAim)
  {
    if(redBlueFadeCurrent < redBlueFadeAim)
    {
      redBlueFadeCurrent += 10;
    }
    if(redBlueFadeCurrent > redBlueFadeAim)
    {
      redBlueFadeCurrent -= 10;
    }
  }
  redBrightness = bright * (redBlueFadeCurrent / 100);
  blueBrightness = bright - redBrightness;
  analogWrite(redLed, 255 - redBrightness);
  analogWrite(blueLed, 255 - blueBrightness);
  
}
