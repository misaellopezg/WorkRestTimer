# WorkRestTimer
Repo for work/rest timer project.

### Change Log
28-AUG-2023
- Winston requested to only use one button, specifically the encoder
- Winston requested having the timer count down instead of up
- I can't get the LCD screen yet, it needs 5V
31-AUG-2023
- I got the SSD1306 display working with an Arduino Nano. I can switch back to the ESP32 using Vin but have to use logic shifters to go from 5V to 3.3V
- Need to work on how to update display without messing up the input timing of the encoder. 
23-SEP-2023
- Got it working! Now I am switching the OLED display for a 7-segment display.
