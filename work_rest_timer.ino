/*
Author: Misael Lopez
Arduino code for Winston's work/rest timer
Using an encoder w/ a switch, users set a work time, a rest time, and 
a neopixel stick counts down the times set using different colors
State machine:
0 - idle
1 - set work time
2 - set rest time
3 - count down work time
4 - N/A (used for debugging)
5 - count down rest time
6 - exit
*/
//Define Libraries
#include <Arduino.h>
#include <Adafruit_NeoPixel.h> //adafruit neopixel strip driver

//define macros
#define MIN_CNT 0 //Minutes
#define MAX_CNT  15//Minutes
#define BRIGHTNESS 127 // Set BRIGHTNESS to about 1/5 (max = 255)

//define pins
const int ledPin = 13; 

//Encoder gpio definition
const int startButtonPin = 2;
const int enc_A = 3; //encoder clk
const int enc_B = 4; //encoder dt

//Adafruit Neopixel gpio definition
#define LED_PIN     8
#define LED_COUNT  16
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

//variables
int timer_state = 0; //contains timer state machine: 0 = idle, etc....
float timer_val = 0; //contains elapsed time, stored in float to display seconds
int work_time = 0; //work time in minutes
int rest_time = 0; // work time in seconds
int percent = 0; //percentage to display

unsigned long prevtime = 0;
unsigned long curtime = 0;
int aState = 0; 
int aLastState = 0;
int timer_cntr = 0;//buffer variable containing set time in minutes for work/rest
int encoder_movement = 0;
unsigned long encoder_delay = 0;
unsigned long prevencoder_delay = 0;
int prevdisp1 = 0; 
int prevdisp2 = 0;

float mypercent;

unsigned long cur_pulse_time;
unsigned long prev_pulse_time;
uint8_t dir = 1;
unsigned long cur_scan_time;
unsigned long prev_scan_time;


void setup() 
{
  //Serial comms for debugging
  Serial.begin(9600); 
  //set encoder gpio and interrupt for count detection
  pinMode(startButtonPin, INPUT);
  pinMode(enc_B, INPUT);
  attachInterrupt(digitalPinToInterrupt(enc_A), encoder_rotation, FALLING);
  aLastState = digitalRead(enc_A);

  timer_state = 0; 
  strip.begin();//initialize neopixel strip object
  strip.show();//turn off all pixels ASAP
  strip.setBrightness(BRIGHTNESS);
  strip.fill(strip.Color(255,255,255),0,15);
  strip.show();  
  delay(500);
  cur_pulse_time = millis();
  prev_pulse_time = cur_pulse_time;
}

void loop() 
{
  //State machine for timer
  switch(timer_state)
  {
    case 0: //idle
      if(cur_pulse_time - prev_pulse_time > 25)
      {
        pulseWhite();
        prev_pulse_time = cur_pulse_time;
      }
      if(!digitalRead(startButtonPin))
      {
        work_time = 0;
        rest_time = 0;
        timer_state = 1;//set work time
        strip.fill(strip.Color(255,255,255),0,15);
        strip.setBrightness(127);
        strip.show();
        timer_cntr = 0;
        delay(1500);
      }
      break;
    case 1: //set work time
      displaytimeselect(timer_cntr, strip.Color(173,216,230), strip.Color(81,168,212));
      if(!digitalRead(startButtonPin))
      {
        work_time = timer_cntr*5; 
        delay(10);
        timer_state = 2;//set rest time
        timer_cntr = 0;
        strip.fill(strip.Color(255,255,255),0,15);
        strip.setBrightness(127);
        strip.show();
        delay(1500);
      }
      break;
    case 2: //set rest time
      displaytimeselect(timer_cntr, strip.Color(255,0,0), strip.Color(255,155,0));
      if(!digitalRead(startButtonPin))
      {
        rest_time = timer_cntr*5; 
        delay(1);
        timer_cntr = 0;
        timer_state = 3;//count work time
        delay(1500);
        prevtime = millis();//set previous time to calculate elapsed time
        Serial.print("Work Time: ");
        Serial.println(work_time);
        Serial.print("Rest Time: ");
        Serial.println(rest_time);
      }
      break;
    case 3: //count work time
      curtime = millis();
      timer_val = float(curtime - prevtime)/60000;
      percent = 100*(float(work_time)-timer_val)/work_time;
      Serial.println(percent);
      displayprogress(percent, 81,168,212); 
      //Serial.println(timer_val); 
      if(int(timer_val) >= work_time)
      {
        timer_val = 0;
        prevtime = millis();//set previous time to calculate elapsed time
        timer_state = 4;
      }
      break;
    case 4: 
      timer_state = 5;
      break;
    case 5: //count rest time
      curtime = millis();
      timer_val = float(curtime - prevtime)/60000;
      percent = 100*(float(rest_time)-timer_val)/rest_time;
      Serial.println(percent);
      displayprogress(percent, 250,155,0); 
      //Serial.println(timer_val); 
      if(int(timer_val) >= rest_time)
      {
        timer_val = 0;
        prevtime = millis();//set previous time to calculate elapsed time
        timer_state = 3;
      }
      break;
    case 6: //exiting
      strip.fill(strip.Color(255,255,255),0,15);
      strip.show(); 
      delay(500);
      timer_state = 0;
      break;
    default:
      timer_state = 0;
      break;
  }
  //Go back to idle if button is pressed when counting work or rest time
  if(!digitalRead(startButtonPin) && (timer_state == 3 || timer_state == 5))//exit if stop
  {
    timer_cntr = 0;
    timer_state = 6;//exit
  }
  cur_pulse_time = millis();
}

/*
 * Function: encoder_rotation
 * -------------------------
 * Detects encoder rotation and increments or decrements the timer counter based on rotation
 * 
 * Parameters:
 *   N/A
 * 
 * Returns:
 *   N/A
 */
void encoder_rotation()
{
  aState = digitalRead(enc_A); // Reads the "current" state of the outputA
   // If the previous and the current state of the outputA are different, that means a Pulse has occured
   if (aState != aLastState)
   {     
     // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
     //Increment or decrement the timer counter.
     //For simplification, the timer counter will be multiplied by 5 to represent the number of minutes to count down.
     //For example: timer_ctr = 5 would mean the user has set the timer to 25 minutes (5 * 5).
     if (digitalRead(enc_B) != aState) 
     { 
       timer_cntr ++;
     } else 
     {
       timer_cntr --;
     }
     if(timer_cntr > 15)
     {
       timer_cntr = 15;
     }
     if(timer_cntr < 0)
     {
       timer_cntr = 0;
     }
     //Serial.print("Encoder Val: ");
     //Serial.println(timer_cntr);
   } 
   aLastState = aState; // Updates the previous state of the outputA with the current state
}

/*
 * Function: displaytimeselect
 * -------------------------
 * Based on the encoder rotation, turns on set number of leds to a work/rest color based on the timer counter value set.
 * 
 * Parameters:
 *   time: time representing the number of leds to turn on
 *   color1: NOT USED
 *   color2: sets the color of the leds that will turn on
 * 
 * Returns:
 *   N/A
 */
void displaytimeselect(int time, uint32_t color1, uint32_t color2)
{
  strip.fill(strip.Color(255,255,255),0,15);
  if(time == 0)
  {
    strip.fill(strip.Color(255,255,255),0,15);
  }
  else
  {
    for(int x = 0; x < time; x++)
    {
        strip.setPixelColor(x, color2);
    }
    for(int y = time; y < 15; y++)
    {
      strip.setPixelColor(y, strip.Color(255,255,255));
    }
  }
  strip.show();
}

void displayprogress(int percent, int r, int g , int b)
{
  int start = 0;
  int end = 6;
  for(int x = 0; x < 15; x ++)
  {
    long segment_brightness = map(percent, start, end, 0, 255);
    int brightness = int(constrain(segment_brightness, 0, 255));
    strip.setPixelColor(x, strip.Color((brightness*r/255) , (brightness*g/255), (brightness*b/255)));
    start = end;
    end = end + 6;
  }
  strip.show(); 
}

void pulseWhite() 
{
  uint8_t brightness = strip.getBrightness();
  if(brightness >= 127)
  {
    dir = -1;
    brightness--;

  }
  else if (brightness <= 2)
  {
    dir = 1;
    brightness++;
  }
  brightness = brightness + dir;
  //Serial.print("Brightness: ");
  Serial.println(brightness);
  strip.setBrightness(brightness);
  strip.show(); 
}

void scan(int num_leds, uint32_t color)
{
  strip.fill(strip.Color(255,255,255),0,15);
  for(int x = 0; x < num_leds; x++)
  {
    strip.fill(strip.Color(255,255,255),0,15);
    strip.setPixelColor(x, color);
    strip.show(); 
  }
}