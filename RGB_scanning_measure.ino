
// https://www.electronicshub.org/rgb-led-with-arduino/#How_to_Generate_Colors_using_RGB_LED

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Keyboard.h"


int pin_led_r = 21;
int pin_led_g = 22;
int pin_led_b = 23;

int pin_sig_high = 2;
int pin_led_13 = 13;
int pin_led_test = 0;

int pin_cell_read = 31;
int pin_5V = 2;

int cnt = 0;
int end_color_cnt = 640;


int update_time = 100;
int scan_time = 10000; // 10 sec

int i =0;
byte turn;
int mode=0 ;

char inChar;
int cell_val= 0;

float voltage =0.0;

float scan_period =0,end_time =0 ,start_time = 0;
int full = 255;
#define  end_color  1 //0: red2red, 1: violet

// int x;

void setup()
{
  pinMode(pin_led_g, OUTPUT);
  pinMode(pin_led_r, OUTPUT);
  pinMode(pin_led_b, OUTPUT);
  pinMode(pin_led_13, OUTPUT);
  pinMode(pin_led_test, OUTPUT);

  pinMode(pin_sig_high, OUTPUT);

  pinMode(pin_cell_read, INPUT);
  pinMode(pin_5V, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  while (Serial.available() > 0){
    inChar = Serial.read();

    if(inChar=='m'){ //reset
  //         Serial.print(F("Mode change"));
  //         Serial.print(" ");

      digitalWrite(pin_led_test, mode%2);

      mode = mode +1;
      if(mode >3 )
        mode = 0;
    }
    // else if(inChar=='a') //reset
    //   cnt = cnt + 1;

    else if (inChar=='g'){ //reset
      Serial.println(" scan start");
      start_time = millis();
      mode = 4;
      digitalWrite(pin_led_test, LOW);

      if(end_color ==0)
        end_color_cnt=768;
      else
        end_color_cnt=640;

      while(cnt<end_color_cnt){
        RGBscan(10000);
        end_time = millis();
        scan_period = end_time - start_time;

        // Serial.print(" scan_period ");
        // Serial.print(scan_period/1000.0);
        // Serial.print(" ");

        // Serial.print(" ");
        // Serial.print(cnt);
        // Serial.print(" ");

        // if(cnt == red2red){
        if(cnt == end_color_cnt-1){ //violet
          Serial.println(" scan finish");
          Serial.print(" ");
          cnt = 0 ;
          mode = 0;
          break;
        }
      }
    }
    else if (inChar=='1'){ //reset
      analogWrite(pin_led_r, HIGH);   // turn the LED on (HIGH is the voltage level)
      analogWrite(pin_led_g, LOW);   // turn the LED on (HIGH is the voltage level)
      analogWrite(pin_led_b, LOW);   // turn the LED on (HIGH is the voltage level)
    }


    if(inChar=='r'){ //reset
      Serial.println(F("Reset"));
     //_softRestart();

      analogWrite(pin_led_r, LOW);   // turn the LED on (HIGH is the voltage level)
      analogWrite(pin_led_g, LOW);   // turn the LED on (HIGH is the voltage level)
      analogWrite(pin_led_b, LOW);   // turn the LED on (HIGH is the voltage level)
      delay(update_time);
    }

    // if(cnt % 100 ==0){
      // digitalWrite(pin_led_test, );
      //digitalWrite(pin_led_test, mode);

  }// end of while

  // if(mode !=0){
  switch (mode%5) {
    case 1:
          // digitalWrite(pin_led_r, HIGH);   // turn the LED on (HIGH is the voltage level)
          // digitalWrite(pin_led_g, LOW);   // turn the LED on (HIGH is the voltage level)
          // digitalWrite(pin_led_b, LOW);   // turn the LED on (HIGH is the voltage level)

          analogWrite(pin_led_r, full);
          analogWrite(pin_led_g, 0);
          analogWrite(pin_led_b, 0);

          break;

    case 2:
          // digitalWrite(pin_led_r, LOW);   // turn the LED on (HIGH is the voltage level)
          // digitalWrite(pin_led_g, HIGH);   // turn the LED on (HIGH is the voltage level)
          // digitalWrite(pin_led_b, LOW);   // turn the LED on (HIGH is the voltage level)

          analogWrite(pin_led_r, 0);
          analogWrite(pin_led_g, full);
          analogWrite(pin_led_b, 0);

          break;

    case 3:
          // digitalWrite(pin_led_r, LOW);   // turn the LED on (HIGH is the voltage level)
          // digitalWrite(pin_led_g, LOW);   // turn the LED on (HIGH is the voltage level)
          // digitalWrite(pin_led_b, HIGH);   // turn the LED on (HIGH is the voltage level)

          analogWrite(pin_led_r, 0);
          analogWrite(pin_led_g, 0);
          analogWrite(pin_led_b, full);
          break;

    default:

          // digitalWrite(pin_led_r, LOW);   // turn the LED on (HIGH is the voltage level)
          // digitalWrite(pin_led_g, LOW);   // turn the LED on (HIGH is the voltage level)
          // digitalWrite(pin_led_b, LOW);   // turn the LED on (HIGH is the voltage level)

          analogWrite(pin_led_r, 0);
          analogWrite(pin_led_g, 0);
          analogWrite(pin_led_b, 0);

          break;

  }// end of switch


  // }//end of it

//      analogWrite(pin_led_r, LOW);   // turn the LED on (HIGH is the voltage level)
//      analogWrite(pin_led_g, LOW);   // turn the LED on (HIGH is the voltage level)
//      analogWrite(pin_led_b, LOW);   // turn the LED on (HIGH is the voltage level)
//      analogWrite(pin_led_blink, cnt%2);   // turn the LED on (HIGH is the voltage level)

  digitalWrite(pin_sig_high, HIGH);
  digitalWrite(pin_5V, HIGH);

  display(update_time);
}

// https://www.electronicshub.org/rgb-led-with-arduino/#How_to_Generate_Colors_using_RGB_LED
void RGBscan(int scan_time){
  int redBrightness;
  int greenBrightness;
  int blueBrightness;



  // Serial.print(" ");
  // Serial.print(x);
  // Serial.print(" ");

  if (cnt <= 255)
  {
    redBrightness = 255 - cnt;
    greenBrightness = cnt;
    blueBrightness = 0;
  }
  else if (cnt <= 511)
  {
    redBrightness = 0;
    greenBrightness = 255 - (cnt - 256);
    blueBrightness = (cnt - 256);
  }
  else
  {
    redBrightness = (cnt - 512);
    greenBrightness = 0;
    blueBrightness = 255 - (cnt - 512);
  }

  analogWrite(pin_led_r, redBrightness);
  analogWrite(pin_led_b, blueBrightness);
  analogWrite(pin_led_g, greenBrightness);

  //delay(delay_time);
  // display(scan_time/768);
  display(scan_time/end_color_cnt);
  // display();

   cnt= cnt + 1;
}

#define SCB_AIRCR (*(volatile uint32_t *)0xE000ED0C) // Application Interrupt and Reset Control location

void _softRestart()
{
  Serial.println("Reset");
  Serial.end();  //clears the serial monitor  if used
  SCB_AIRCR = 0x05FA0004;  //write value for restart
}

void display(int update_time){

    Serial.print(" mode");
    Serial.print(mode);
    Serial.print(" ");

    cell_val = analogRead(pin_cell_read);
    // Serial.print(cell_val);
    // Serial.print(" ");

    voltage= cell_val * (5.0 / 1023.0);
    // Serial.print(" vol");
    Serial.println(voltage);

    if(mode <4)
      delay(update_time);
    else if(mode ==4)
      delay(scan_time/768);

}
