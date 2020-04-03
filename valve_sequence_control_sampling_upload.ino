/*
Project: EOS spectrophotometer
Subroutine: led sensing and valve control
Version: <1.0 (development) rx.x (released)>
Date: <2020/03/31>
Author/Editor:  Jinseong Lee

Notes: +
//valve sequene

  1. valve open
  2. sensor read
  3. sensor sensor
  4. valve open
*/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Keyboard.h"

#include "Adafruit_TSL2591.h"
#include <TimerOne.h>

IntervalTimer myTimer;

// pin number assignment
int pin_sig_high = 2; // test
int pin_led_test = 0;
int pin_led_r = 20;
int pin_led_g = 21;
int pin_led_b = 22;
int pin_cell_read2 = 34; // pin to read a light sensor

// valve pins
int valve_algae_water= 14;
int valve_clean_water= 15;
int valve_drain_water= 16;

char inChar;
int mode= 0;
char state;

int valve1,valve2,valve3; //read valve on/off state

float data_arr[1000] = {0.0}; // store sample data
float data_avg = 0.0;
float data_sum = 0.0;

#define CLOSE LOW
#define OPEN HIGH

int update_time = 100;
// int scan_time = 10000; // 10 sec
int scan_time = 10000; // 10 sec

float scan_period = 0, end_time = 0 , start_time = 0;

int i = 0; //index
byte turn;

int cell_val = 0; // sensor reading
float voltage = 0.0;

float hold_end , hold_start = 0.0; // to hold led for start sampling

int data_cnt = 1;
int max_sample_cnt = 100;// max data

#define  end_color  0 //0: red2red, 1: violet

// RGB LED end color mode
#if end_color == 0
  #define end_color_cnt 768 // from red to red
#else
  #define end_color_cnt 640 //from red to red
#endif

int color_cnt = 1; //color code


// valve state
enum state {IDLE,DRAIN_BEFORE,CLEAN_BEFORE,FILL,SAMPLE,DRAIN_AFTER,CLEAN_AFTER};

void setup()
{
  pinMode(pin_sig_high, OUTPUT);

  pinMode(valve_algae_water, OUTPUT);
  pinMode(valve_clean_water, OUTPUT);
  pinMode(valve_drain_water, OUTPUT);

  pinMode(pin_led_r, OUTPUT);

  pinMode(pin_led_g, OUTPUT);
  pinMode(pin_led_r, OUTPUT);
  pinMode(pin_led_b, OUTPUT);

    Timer1.initialize(10000000); //1 sec
  }


void loop() {
    // delay(500);

    while (Serial.available() > 0) {
      inChar = Serial.read();

      if (inChar == 'i') { //reset
        mode = 0;
        color_cnt = 0;
        data_cnt = 0;
        analogWrite(pin_led_r, 0);   // turn the LED on (HIGH is the voltage level)
        analogWrite(pin_led_g, 0);   // turn the LED on (HIGH is the voltage level)
        analogWrite(pin_led_b, 0);   // turn the LED on (HIGH is the voltage level)
        Serial.println(F(" initialized"));
      }

      else if(inChar == '1'){
        mode = mode + 1;
      }
      else if(inChar == '2'){
        // mode = mode + 1;
        sampling_valve_test();
      }
      else if(inChar == 's'){ // manual sampling by user command
        state = 's';
        if(state = 's')
          valve_sequence();
        }

      else if(inChar == 'm'){ // manual check for each state
        state = 'm';
        if(state = 'm'){
          mode = mode +1;
          valve_manual(mode);
        }
      }
      if (inChar == 'r') { //reset
         Serial.println(F("Reset"));
         _softRestart();

         analogWrite(pin_led_r, LOW);
         analogWrite(pin_led_g, LOW);
         analogWrite(pin_led_b, LOW);
         delay(update_time);
      }
      else
        state ='n';
    }// end of while
}

//void display_led(int mode){
void display_led(void){
  mode = mode +1;
  //Serial.print(mode%2);
  Serial.print(mode);

  if(mode % 2 ==0){
      digitalWrite(pin_led_r, HIGH);   // turn the LED on (HIGH is the voltage level)
      Serial.println(" turned on");
  }
  else{
      digitalWrite(pin_led_r, LOW);   // turn the LED on (HIGH is the voltage level)

      Serial.println(" turned off");
  }
}


// valve manual test
void valve_manual(int mode){

  switch(mode){
    case 1:

      Serial.print(" DRAIN_BEFORE");
      Serial.print(DRAIN_BEFORE);

      digitalWrite(valve_algae_water, CLOSE); //0V
      digitalWrite(valve_clean_water, CLOSE); //0V
      digitalWrite(valve_drain_water, OPEN); //3.3V

      Serial.println(" 001  ");

      valve1 = digitalRead(valve_algae_water);
      valve2 = digitalRead(valve_clean_water);
      valve3 = digitalRead(valve_drain_water);

      Serial.print(" v1:  ");Serial.println(valve1);
      Serial.print(" v2:  ");Serial.println(valve2);
      Serial.print(" v3:  ");Serial.println(valve3);

      digitalWrite(pin_led_r, mode % 2);   // turn the LED on (HIGH is the voltage level)
      delay(2000);

      break;


    case 2:

      Serial.print(" CLEAN_BEFORE");
      Serial.print(CLEAN_BEFORE);

      digitalWrite(valve_algae_water, CLOSE);
      digitalWrite(valve_clean_water, OPEN);
      digitalWrite(valve_drain_water, CLOSE);

      Serial.println(" 010  ");


      digitalWrite(pin_led_r, mode % 2);   // turn the LED on (HIGH is the voltage level)

      delay(2000);
      break;

    case 3:

      Serial.print(" FILL");
      Serial.print(FILL);

      digitalWrite(valve_algae_water, OPEN);
      digitalWrite(valve_clean_water, CLOSE);
      digitalWrite(valve_drain_water, CLOSE);

      Serial.println(" 011  ");


      digitalWrite(pin_led_r, mode % 2);   // turn the LED on (HIGH is the voltage level)

      delay(1000);
      break;

    case 4:

      Serial.print(" SAMPLE");
      Serial.print(SAMPLE);

      digitalWrite(valve_algae_water, CLOSE);
      digitalWrite(valve_clean_water, CLOSE);
      digitalWrite(valve_drain_water, CLOSE);

      Serial.println(" 100  ");


      digitalWrite(pin_led_r, HIGH);   // turn the LED on (HIGH is the voltage level)

      delay(1000);
      break;

    case 5:

      Serial.print(" DRAIN_AFTER");
      Serial.print(DRAIN_AFTER);

      digitalWrite(valve_algae_water, CLOSE);
      digitalWrite(valve_clean_water, CLOSE);
      digitalWrite(valve_drain_water, OPEN);

      Serial.println(" 101  ");

      digitalWrite(pin_led_r, mode % 2);   // turn the LED on (HIGH is the voltage level)

      delay(1000);
      break;

    case 6:

        Serial.print(" CLEAN_AFTER");
        Serial.print(CLEAN_AFTER);

        digitalWrite(valve_algae_water, CLOSE);
        digitalWrite(valve_clean_water, OPEN);
        digitalWrite(valve_drain_water, OPEN);

        Serial.println(" 110  ");


        digitalWrite(pin_led_r, mode % 2);   // turn the LED on (HIGH is the voltage level)

        delay(1000);
        break;

    default:
      digitalWrite(pin_led_r, LOW);   // turn the LED on (HIGH is the voltage level)

      Serial.println(" IDLE ");
      Serial.print(IDLE);
      break;
  }// end of switch

}// end of valve_sequence

// auto sampling
void valve_sequence(){// sampling

  state = DRAIN_BEFORE; // start

  switch(state){
    case DRAIN_BEFORE:

      Serial.print(DRAIN_BEFORE);
      Serial.println(": DRAIN_BEFORE for 2 sec");

      digitalWrite(valve_algae_water, CLOSE);
      digitalWrite(valve_clean_water, CLOSE);
      digitalWrite(valve_drain_water, OPEN);

      mode = mode +1;
      digitalWrite(pin_led_r, mode % 2);   // turn the LED on (HIGH is the voltage level)
      delay(2000);

      state = CLEAN_BEFORE;

    case CLEAN_BEFORE:
      Serial.print(CLEAN_BEFORE);
      Serial.println(": CLEAN_BEFORE for 2 sec");

      digitalWrite(valve_algae_water, CLOSE);
      digitalWrite(valve_clean_water, OPEN);
      digitalWrite(valve_drain_water, CLOSE);

      mode = mode +1;
      digitalWrite(pin_led_r, mode % 2);   // turn the LED on (HIGH is the voltage level)

      delay(2000);
      state = FILL;

    case FILL:
      Serial.print(FILL);
      Serial.println(": FILL for 3 sec");

      digitalWrite(valve_algae_water, OPEN);
      digitalWrite(valve_clean_water, CLOSE);
      digitalWrite(valve_drain_water, CLOSE);

      mode = mode +1;
      digitalWrite(pin_led_r, mode % 2);   // turn the LED on (HIGH is the voltage level)

      delay(3000);
      state = SAMPLE;

    case SAMPLE:
      Serial.print(SAMPLE);
      Serial.println(" SAMPLE for 10 sec");

      digitalWrite(valve_algae_water, CLOSE);
      digitalWrite(valve_clean_water, CLOSE);
      digitalWrite(valve_drain_water, CLOSE);

      digitalWrite(pin_led_r, HIGH);   // turn the LED on (HIGH is the voltage level)

      led_sampling();
      mode = mode +1;
      state = DRAIN_AFTER;


    case DRAIN_AFTER:
      Serial.print(DRAIN_AFTER);
      Serial.println(" DRAIN_AFTER for 2 sec");

      digitalWrite(valve_algae_water, CLOSE);
      digitalWrite(valve_clean_water, CLOSE);
      digitalWrite(valve_drain_water, OPEN);

      digitalWrite(pin_led_r, mode % 2);   // turn the LED on (HIGH is the voltage level)

      delay(2000);
      mode = mode +1;
      state = CLEAN_AFTER;


    case CLEAN_AFTER:
        Serial.print(CLEAN_AFTER);
        Serial.println(" CLEAN_AFTER for 2 sec");

        digitalWrite(valve_algae_water, CLOSE);
        digitalWrite(valve_clean_water, OPEN);
        digitalWrite(valve_drain_water, OPEN);

        mode = 0;
        digitalWrite(pin_led_r, mode % 2);

        delay(2000);
        state = IDLE;

    default:
      Serial.print(IDLE);
      Serial.println(" IDLE ");
      digitalWrite(pin_led_r, LOW);

      for (i =0;i<max_sample_cnt;i++){
        data_arr[i] = 0.0;
      }
      mode =0;


      break;
  }// end of switch

  //Serial.print(" state executed  ");
  analogWrite(pin_led_r, mode % 2);   // turn the LED on (HIGH is the voltage level)

}// end of valve_sequence

// manual open/close test
void sampling_valve_test(){

  mode = mode +1;
  Serial.print(mode);

  if(mode % 2 ==0){
    digitalWrite(valve_algae_water, CLOSE);//LOW, 0V
    digitalWrite(valve_clean_water, CLOSE);
    digitalWrite(valve_drain_water, CLOSE);

    digitalWrite(pin_led_r, LOW);   // turn the LED on (HIGH is the voltage level)
    Serial.println(" closed");
  }
  else{
    digitalWrite(valve_algae_water, OPEN);//high, 3.3V
    digitalWrite(valve_clean_water, OPEN);
    digitalWrite(valve_drain_water, OPEN);

    digitalWrite(pin_led_r, HIGH);   // turn the LED on (HIGH is the voltage level)

      Serial.println(" open");
  }

}

// control led and read sensor data
void led_sampling(){
    // Serial.println(" led_sampling");
    start_time = millis();

    while (data_cnt <= max_sample_cnt) {

      inChar = Serial.read();

      if (inChar == 'c') {
        color_cnt = 0;
        data_cnt = 0;
        break;
      }

      RGBscan(10000);

      hold_start = millis();
      hold_led(mode, color_cnt);

      if (data_cnt >= 1 && data_cnt <= max_sample_cnt)
        display(scan_time / max_sample_cnt);

        end_time = millis();
        scan_period = end_time - start_time;

        if (color_cnt >= end_color_cnt) { //violet
            Serial.print(" ");
            color_cnt = 0 ;
            // mode = 0;
            data_cnt = 0;
            break;
        }

        if ( data_cnt >= max_sample_cnt){
          data_cnt = 0;
          color_cnt = 0;
          data_sum = 0;
          break;
        }

        data_cnt = data_cnt + 1;
    }// end of while in 's'



}

// change led light continuously
// https://www.electronicshub.org/rgb-led-with-arduino/#How_to_Generate_Colors_using_RGB_LED
void RGBscan(int scan_time) {

  int redBrightness;
  int greenBrightness;
  int blueBrightness;

  // cnt =1; //for red led mode

  if (color_cnt <= 255) {
    // redBrightness = 255 - color_cnt;
    redBrightness = 255;
    greenBrightness = color_cnt;
    blueBrightness = 0;
  }
  else if (color_cnt <= 511) {
    redBrightness = 0;
    greenBrightness = 255 - (color_cnt - 256);
    blueBrightness = (color_cnt - 256);
  }
  else {
    redBrightness = (color_cnt - 512);
    greenBrightness = 0;
    blueBrightness = 255 - (color_cnt - 512);
  }

  analogWrite(pin_led_r, redBrightness);
  analogWrite(pin_led_b, blueBrightness);
  analogWrite(pin_led_g, greenBrightness);

}

// standby before led sampling
void hold_led(int mode, int color_cnt) {


  // hold led for 2 sec to start sampling
  if (data_cnt == 1 || color_cnt == end_color_cnt) { // stay longer when start scanning and finishing
    Serial.println("stanby 2 sec for algae sampling");
    while (hold_end - hold_start < 2000) {
      hold_end = millis();
      delay(100);
      Serial.println("..");
      digitalWrite(pin_led_test, HIGH);
    }

    digitalWrite(pin_led_test, LOW);

    if (color_cnt == end_color_cnt) {
      //Serial.println(" end_color_cnt");
      digitalWrite(pin_led_test, HIGH);
    }
  }
}

// print data to serial port
void display(int update_time) {

   if(data_cnt ==1){
    // Serial.print("State ");
     Serial.print("Color ");
     Serial.print("Idx ");
     Serial.print("Data ");
     Serial.println("Avg ");
   }
      cell_val = analogRead(pin_cell_read2);
      voltage= cell_val * (3.3 / 1023.0);
      data_arr[data_cnt] = voltage;

      int avg_data_num = 5;

      // average 5 data
      if(data_cnt <=5){
        data_sum = data_sum + data_arr[data_cnt];
        data_avg= data_sum/data_cnt;
      }
      else{
          for(i == data_cnt-(avg_data_num-1);i==data_cnt;i++){
            data_sum = data_sum + data_arr[i]- data_arr[i-(avg_data_num)];
            data_avg= data_sum/avg_data_num;
          }
      }

      Serial.print(color_cnt);
      Serial.print("  ");
      Serial.print(data_cnt);
      Serial.print("  ");

      Serial.print(data_arr[data_cnt]);
      Serial.print("  ");

      Serial.println(data_avg);

      delay(update_time);
}

#define SCB_AIRCR (*(volatile uint32_t *)0xE000ED0C) // Application Interrupt and Reset Control location

// soft reset
void _softRestart()
{
  Serial.println("Reset");
  Serial.end();  //clears the serial monitor  if used
  SCB_AIRCR = 0x05FA0004;  //write value for restart
}
