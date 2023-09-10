//define libraries
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//define macros
#define MIN_TIME 0 //Minutes
#define MAX_TIME 99 //Minutes

//display stuff
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

//define pins
const int ledPin = 13; 
const int startButtonPin = 2;
//const int stopButtonPin = 35; 
const int enc_A = 3; //encoder clk
const int enc_B = 4; //encoder dt

//variables
//int state_machine = 0;
int timer_state = 0; //contains timer state machine: 0 = idle, etc....
float timer_val = 0; //contains elapsed time, stored in float to display seconds
int work_time = 0; //work time in minutes
int rest_time = 0; // work time in seconds

unsigned long prevtime = 0;
unsigned long curtime = 0;
int aState = 0; 
int aLastState = 0;
int timer_cntr = 0;//buffer variable containing set time in minutes for work/rest
int prev_timedisp[] = {0,0,0,0}; 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
int encoder_movement = 0;
unsigned long encoder_delay = 0;
unsigned long prevencoder_delay = 0;

void setup() 
{
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT);
  pinMode(startButtonPin, INPUT);
  //pinMode(stopButtonPin, INPUT_PULLDOWN);
  //pinMode(enc_A, INPUT); 
  pinMode(enc_B, INPUT);
  attachInterrupt(digitalPinToInterrupt(enc_A), encoder_rotation, FALLING);
  Serial.begin(9600); 
  aLastState = digitalRead(enc_A);
  timer_state = 0; 
  //OLED Display
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) 
  {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  delay(500);
  drawText("TIMER", 2, (SCREEN_WIDTH-1)/4, (SCREEN_HEIGHT-1)/2);
}

void loop() 
{
  //State machine for timer
  switch(timer_state)
  {
    case 0: //idle
      if(!digitalRead(startButtonPin))
      {
        work_time = 0;
        rest_time = 0;
        timer_state = 1;//set work time
        display.clearDisplay();
        delay(10);
        drawText("Work Time", 2, (SCREEN_WIDTH-1)/8, (SCREEN_HEIGHT-1)/8);
        drawTimerInit();
        delay(1500);
      }
      break;
    case 1: //set work time
      drawTimer(timer_cntr);
      if(!digitalRead(startButtonPin))
      {
        work_time = timer_cntr; 
        display.clearDisplay();
        delay(10);
        timer_state = 2;//set rest time
        drawText("Rest Time", 2, (SCREEN_WIDTH-1)/8, (SCREEN_HEIGHT-1)/8);
        drawTimerInit();
        timer_cntr = MIN_TIME;
        delay(1500);
      }
      break;
    case 2: //set rest time
      drawTimer(timer_cntr);
      if(!digitalRead(startButtonPin))
      {
        rest_time = timer_cntr; 
        display.clearDisplay();
        delay(10);
        timer_cntr = MIN_TIME;
        timer_state = 3;//count work time
        //drawText("Starting...");
        Serial.println("Josh Builds a Wall"); 
        drawTimer(100 + work_time);
        delay(1500);
        prevtime = millis();//set previous time to calculate elapsed time
      }
      break;
    case 3: //count work time
      curtime = millis();
      timer_val = float(curtime - prevtime)/60000;
      if( (float(work_time)- timer_val) < 1.0)
      {
        drawTimer(float((work_time)- timer_val)*60.0);
      }
      else
      {
        drawTimer(work_time - int(timer_val)); 
      }
      if(int(timer_val) >= work_time)
      {
        timer_val = 0;
        prevtime = millis();//set previous time to calculate elapsed time
        timer_state = 4;
      }
      break;
    case 4: //count rest time
      curtime = millis();
      timer_val = float(curtime - prevtime)/60000;
      if( (float(rest_time)- timer_val) < 1.0)
      {
        drawTimer(float((rest_time)- timer_val)*60.0);
      }
      else
      {
        drawTimer(rest_time - int(timer_val)); 
      }
      if(int(timer_val) >= rest_time)
      {
        timer_val = 0;
        prevtime = millis();
        timer_state = 3;
      }
      break;
    case 5: //exiting
      display.clearDisplay();
      delay(1500);
      drawText("TIMER", 2, (SCREEN_WIDTH-1)/4, (SCREEN_HEIGHT-1)/2);
      timer_state = 0;
      break;
    default:
      timer_state = 0;
      break;
  }
  if(!digitalRead(startButtonPin) && (timer_state == 3 || timer_state == 4))//exit if stop
  {
    timer_cntr = MIN_TIME;
    timer_state = 5;//exit
  }
}

//tasks
void encoder_rotation()
{
  aState = digitalRead(enc_A); // Reads the "current" state of the outputA
   // If the previous and the current state of the outputA are different, that means a Pulse has occured
   if (aState != aLastState)
   {     
     // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
     if (digitalRead(enc_B) != aState) 
     { 
       timer_cntr ++;
     } else 
     {
       timer_cntr --;
     }
     if(timer_cntr > MAX_TIME)
     {
       timer_cntr = MAX_TIME;
     }
     if(timer_cntr < MIN_TIME)
     {
       timer_cntr = MIN_TIME;
     }
     Serial.print("Timer Val: ");
     Serial.println(timer_cntr);
   } 
   aLastState = aState; // Updates the previous state of the outputA with the current state
}

void drawText(String text, int size, int x, int y) 
{
  display.clearDisplay();
  display.setTextSize(size);// Set pixel scale
  display.setTextColor(SSD1306_WHITE);// Draw white text
  display.setCursor(x,y);// Start at top-left corner
  display.println(text);
  display.display();
  delay(250);
}

void drawTimerInit()
{
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);

  //display.setCursor(15, (SCREEN_HEIGHT-1)/2);
  //display.println(0);
  //delay(1);

  display.setCursor(47, (SCREEN_HEIGHT-1)/2);
  display.println(0);
  delay(1);
  
  display.setCursor(79, (SCREEN_HEIGHT-1)/2);
  display.println(0);
  delay(1);

  //display.setCursor(111, (SCREEN_HEIGHT-1)/2);
  //display.println(0);
  //delay(1);
}

void drawTimer(int time) //
{
  int timedisp[] = {0,0,0,0};
  int i = 0;
  while(time > 0 && i < 2)
  {
    timedisp[i+1] = time%10;
    time = (time - time%10)/10;
    i++;
  }
  /*
  if(timedisp[3] != prev_timedisp[3])
  {
    clearDisplay3();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(15, (SCREEN_HEIGHT-1)/2);
    display.println(timedisp[3]);
    delay(1);
  }
  */
  if(timedisp[2] != prev_timedisp[2])
  {
    clearDisplay2();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(47, (SCREEN_HEIGHT-1)/2);
    display.println(timedisp[2]);
    delay(1);
  }
  if(timedisp[1] != prev_timedisp[1])
  {
    clearDisplay1();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(79, (SCREEN_HEIGHT-1)/2);
    display.println(timedisp[1]);
    delay(1);
  }
  /*
  if(timedisp[0] != prev_timedisp[0])
  {
    clearDisplay0();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(111, (SCREEN_HEIGHT-1)/2);
    display.println(timedisp[0]);
    delay(1);
  }
  */
  
  display.display();
  for(int x = 0; x < sizeof(prev_timedisp); x++)
  {
    prev_timedisp[x] = timedisp[x];
  }
}

void clearDisplay3()
{
  int start = (SCREEN_HEIGHT-1)/2;
  int stop = start + 15;
  for(int y = start; y < stop; y++)
  {
    display.drawLine(15, y, 46, y, SSD1306_BLACK);
  }
  display.display();
}

void clearDisplay2()
{
  int start = (SCREEN_HEIGHT-1)/2;
  int stop = start + 15;
  for(int y = start; y < stop; y++)
  {
    display.drawLine(47, y, 63, y, SSD1306_BLACK);
  }
  display.display();
}

void clearDisplay1()
{
  int start = (SCREEN_HEIGHT-1)/2;
  int stop = start + 15;
  for(int y = start; y < stop; y++)
  {
    display.drawLine(79, y, 110, y, SSD1306_BLACK);
  }
  display.display();
}

void clearDisplay0()
{
  int start = (SCREEN_HEIGHT-1)/2;
  int stop = start + 15;
  for(int y = start; y < stop; y++)
  {
    display.drawLine(111, y, 127, y, SSD1306_BLACK);
  }
  display.display();
}

void drawHorizontalLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) 
{
  //display.clearDisplay(); // Clear display buffer
  display.drawLine(x0, y0, x1, y1, SSD1306_WHITE);
  display.display(); // Update screen with each newly-drawn line
  delay(1);
}

void drawProgressBar(int16_t x0, int16_t y0, int16_t x1, int16_t size)
{
  for(int16_t i = 0; i < size-1; i++)
  {
    drawHorizontalLine(x0, y0+i, x1, y0+i);
    delay(1);
  }
}