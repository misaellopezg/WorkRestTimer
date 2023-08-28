//define headers

//define macros
#define MIN_TIME 0
#define MAX_TIME 100

//define pins
const int ledPin = 2; 
const int startButtonPin = 34;
const int stopButtonPin = 35; 
const int enc_A = 25; //encoder clk
const int enc_B = 33; //encoder dt

//variables
int state_machine = 0;
int timer_state = 0;
int timer_val = 0;
unsigned long prevtime = 0;
unsigned long curtime = 0;
int aState = 0; 
int aLastState = 0;
int timer_cntr = 0;
int work_time = 0; 
int rest_time = 0;

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

void setup() 
{
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT);
  pinMode(startButtonPin, INPUT);
  pinMode(stopButtonPin, INPUT_PULLDOWN);
  pinMode(enc_A, INPUT); 
  pinMode(enc_B, INPUT);
  Serial.begin(9600); 
  aLastState = digitalRead(enc_A);
  timer_state = 0; 
  Serial.println("Idle...");

}

void loop() 
{
  // put your main code here, to run repeatedly:
  switch(timer_state)
  {
    case 0: //idle
      if(digitalRead(startButtonPin))
      {
        work_time = 0; 
        rest_time = 0;
        timer_state = 1;//set work time
        Serial.println("Set work time..."); 
        delay(1500);
      }
      break;
    case 1: //set work time
      encoder_rotation();
      if(digitalRead(startButtonPin))
      {
        work_time = timer_cntr; 
        Serial.print("Work time set to: ");
        Serial.print(work_time); 
        Serial.println(" sec"); 
        timer_state = 2;//set rest time
        Serial.println("Set rest time..."); 
        timer_cntr = MIN_TIME;
        delay(1500);
      }
      break;
    case 2: //set rest time
      encoder_rotation();
      if(digitalRead(startButtonPin))
      {
        rest_time = timer_cntr; 
        Serial.print("Rest time set to: ");
        Serial.print(rest_time); 
        Serial.println(" sec"); 
        timer_cntr = MIN_TIME;
        timer_state = 3;//count work time
        prevtime = millis();//set previous time to calculate elapsed time
        Serial.println("Starting..."); 
        delay(1500);
      }
      break;
    case 3: //count work time
      curtime = millis();
      timer_val = int((curtime - prevtime)/1000);
      Serial.print("Work Time: "); 
      Serial.print(timer_val); 
      Serial.println(" sec");
      if(timer_val >= work_time)
      {
        timer_val = 0;
        prevtime = millis();//set previous time to calculate elapsed time
        timer_state = 4;
      }
      break;
    case 4: //count rest time
      curtime = millis();
      timer_val = int((curtime - prevtime)/1000);
      Serial.print("Rest Time: "); 
      Serial.print(timer_val); 
      Serial.println(" sec");
      if(timer_val >= rest_time)
      {
        timer_val = 0;
        prevtime = millis();
        timer_state = 3;
      }
      break;
    case 5: //exiting
      Serial.println("Exiting...");
      delay(1500);//settling delay before exiting
      Serial.println("Idle...") ;
      timer_state = 0;
      break;
    default:
      timer_state = 0;
      break;
  }
  if(digitalRead(stopButtonPin) && timer_state != 0)//exit if stop
  {
    timer_cntr = MIN_TIME;
    timer_state = 5;//exit
  }
}
