/*
Project: EOS spectrophotometer
Subroutine: led sensing and valve control
Version: <1.0 (development) >
Date: <2020/03/31>
Author/Editor:  Jinseong Lee
Notes:
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
//#include <TimerOne.h>

#include <SoftwareSerial.h>

SoftwareSerial mySerial(0, 1); // RX, TX

// origin
// #define CLOSE LOW
// #define OPEN HIGH

// updated 20200407
#define CLOSE HIGH //3.3V
#define OPEN LOW  //0 V
// valve state
enum valvestate {STANDBY =0,DRAIN_BEFORE,CLEAN_BEFORE,FILL,SAMPLE,DRAIN_AFTER,CLEAN_AFTER};
valvestate valst; //valve state

#define  end_color  0 //0: red2red, 1: violet

// RGB LED end color mode
#if end_color == 0
  #define end_color_cnt 768 // from red to red
#else
  #define end_color_cnt 640 //from red to red
#endif
//IntervalTimer myTimer;

// pin number assignment
int led = 13;
int pin_sig_high = 2; // test
int pin_led_test = 0;

//int pin_led_r = 20;
int pin_led_r = 37;
int pin_led_g = 21;
int pin_led_b = 22;

//sensor
int pin_3_3V = 38;
int pin_cell_read2 = 38; // pin to read a light sensor
int pin_GND = 35;//


// valve pins
//int valve_algae_water= 14;
//int valve_clean_water= 15;
//int valve_drain_water= 16;

int valve_algae_water= 15;
int valve_clean_water= 16;
int valve_drain_water= 14;

char inChar;
int mode= 0;

char state;// procedure state
bool automanual; //0: manual, 1: auto,

int valve1,valve2,valve3; //read valve on/off state

float data_arr[1000] = {0.0}; // store sample data
float data_avg = 0.0;
float data_sum = 0.0;

int update_time = 1000; // display update rate
// int scan_time = 10000; // 10 sec
// int scan_time = 10000; // 10 sec
int scan_time = 10000; // sampling time
int valve_hold_time = 5000; // valve hold time


float scan_period = 0, end_time = 0 , start_time = 0;

int i = 0; //index
byte turn;

int cell_val = 0; // sensor reading
float voltage = 0.0;

float hold_end , hold_start = 0.0; // to hold led for start sampling

int data_cnt = 1;
int max_sample_cnt = 100;// max data

int color_cnt = 1; //color code
int avg_group_num = 5;
int valve_num = 1; //each valve indicator

int redBrightness = 0;
int greenBrightness;
int blueBrightness;


void setup()
{
  pinMode(pin_sig_high, OUTPUT);

  pinMode(valve_algae_water, OUTPUT);
  pinMode(valve_clean_water, OUTPUT);
  pinMode(valve_drain_water, OUTPUT);

  pinMode(led, OUTPUT);
  pinMode(pin_led_r, OUTPUT);

  pinMode(pin_led_g, OUTPUT);
  pinMode(pin_led_r, OUTPUT);
  pinMode(pin_led_b, OUTPUT);

  pinMode(pin_3_3V, OUTPUT);
  pinMode(pin_cell_read2, INPUT);
  pinMode(pin_GND, OUTPUT);

  digitalWrite(valve_algae_water, CLOSE);
  digitalWrite(valve_clean_water, CLOSE);
  digitalWrite(valve_drain_water, CLOSE);

  digitalWrite(pin_3_3V, HIGH);
  digitalWrite(pin_GND, LOW);

  mySerial.begin(9600);
  mySerial.println("Hello, world?");

//    Timer1.initialize(10000000); //1 sec
  }


void loop() {
    // delay(500);

    digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)

    while (mySerial.available() > 0) {
      inChar = mySerial.read();

      switch(inChar){
        case 'i':
        // if (inChar == 'i') { //reset
          mode = 0;
          valst = STANDBY;
          color_cnt = 0;
          data_cnt = 0;
          analogWrite(pin_led_r, 0);   // turn the LED on (HIGH is the voltage level)
          analogWrite(pin_led_g, 0);   // turn the LED on (HIGH is the voltage level)
          analogWrite(pin_led_b, 0);   // turn the LED on (HIGH is the voltage level)
          mySerial.println(F(" initialized"));
          break;

        case 'k':
        // else if(inChar == '1'){
          mode = mode + 1;
          break;
        // }

        case 'a':
        // else if(inChar == '2'){
          // mode = mode + 1;
          sampling_valve_test();
          break;


        case 's':
        // else if(inChar == 's'){ // manual sampling by user command
          state = 's';
           valst = DRAIN_BEFORE;
//          mySerial.print(F("valvestate auto: "));
//          mySerial.println(valst);

          while(valst != STANDBY ){
            // valst = valst + 1; // start
            valve_sequence(1,  valst);
            if(inChar =='c'){
              valst = STANDBY; // start
              break;
            }
            break;
          }
          break;

        case 'm':
        // else if(inChar == 'm'){ // manual check for each state
          state = 'm';

          if(valst == 0)
            valst = valst +1;

          // valst = valst +1;      //valve_manual(mode);

//          mySerial.print(F("valvestate manual: "));
//          mySerial.println(valst);

          valst = valve_sequence(0, valst); //manual
//          valve_sequence(0, valst); //manual

//          mySerial.print(" after valvestate : ");
//          mySerial.println(valst);
          break;
        // }

        case 't':
          state = 't';

//            mySerial.print(SAMPLE);
//            mySerial.println(" SAMPLE for 10 sec");

            digitalWrite(valve_algae_water, CLOSE);
            digitalWrite(valve_clean_water, CLOSE);
            digitalWrite(valve_drain_water, CLOSE);

//            digitalWrite(pin_led_r, HIGH);   // turn the LED on (HIGH is the voltage level)
            led_sampling();
            break;


        case 'r':
        // else if (inChar == 'r') { //reset
           mySerial.println(F("Reset"));
           _softRestart();

           analogWrite(pin_led_r, LOW);
           analogWrite(pin_led_g, LOW);
           analogWrite(pin_led_b, LOW);
           delay(update_time);
           break;


         case 'f':

           if(valve_num % 3 ==1){
            digitalWrite(valve_drain_water, OPEN);
            delay(update_time);
            digitalWrite(valve_drain_water, CLOSE);
            mySerial.println(" drain valve");
           }

           else if(valve_num % 3 == 2) {
            digitalWrite(valve_algae_water, OPEN);
            delay(update_time);
            digitalWrite(valve_algae_water, CLOSE);
            mySerial.println(" algae valve");
           }

           else{
            digitalWrite(valve_clean_water, OPEN);
            delay(update_time);
            digitalWrite(valve_clean_water, CLOSE);
            mySerial.println(" clean valve");
           }

          delay(update_time);

          valve_num = valve_num +1;

          if(valve_num == 3)
            valve_num = 0;

          break;

        case '1':
          digitalWrite(valve_drain_water, OPEN);
          delay(update_time);
          digitalWrite(valve_drain_water, CLOSE);
          mySerial.println(" drain valve");
           break;

        case '2':

          digitalWrite(valve_algae_water, OPEN);
          delay(update_time);
          digitalWrite(valve_algae_water, CLOSE);
          mySerial.println(" algae valve");
           break;

        case '3':
          digitalWrite(valve_clean_water, OPEN);
          delay(update_time);
          digitalWrite(valve_clean_water, CLOSE);
          mySerial.println(" clean valve");

          break;

        default:
        // else
          state ='n';// normal
           // valst= STANDBY;// normal
          // break;
        }
    }// end of while
}

//void display_led(int mode){
void display_led(void){
  mode = mode +1;
  //mySerial.print(mode%2);
  mySerial.print(mode);

  if(mode % 2 ==0){
      digitalWrite(pin_led_r, HIGH);   // turn the LED on (HIGH is the voltage level)
      mySerial.println(" turned on");
  }
  else{
      digitalWrite(pin_led_r, LOW);   // turn the LED on (HIGH is the voltage level)

      mySerial.println(" turned off");
  }
}

// auto sampling
valvestate valve_sequence(bool automanual, int valst){// sampling
//  void valve_sequence(bool automanual, int valst){// sampling


  

  switch(valst){
    case DRAIN_BEFORE:

//      mySerial.print(DRAIN_BEFORE);
//      mySerial.println(": DRAIN_BEFORE for 2 sec");

      digitalWrite(valve_algae_water, CLOSE);
      digitalWrite(valve_clean_water, CLOSE);
      digitalWrite(valve_drain_water, OPEN);

      valst = valst +1;
      digitalWrite(pin_led_r, mode % 2);   // turn the LED on (HIGH is the voltage level)
      delay(valve_hold_time);

      if(automanual == 0)
        break;

      // valvestate = CLEAN_BEFORE;

    case CLEAN_BEFORE:
//      mySerial.print(CLEAN_BEFORE);
//      mySerial.println(": CLEAN_BEFORE for 2 sec");

      digitalWrite(valve_algae_water, CLOSE);
      digitalWrite(valve_clean_water, OPEN);
      digitalWrite(valve_drain_water, OPEN);

      valst = valst +1;
      digitalWrite(pin_led_r, mode % 2);   // turn the LED on (HIGH is the voltage level)

      delay(valve_hold_time);

      if(automanual == 0)
        break;

    case FILL:
//      mySerial.print(FILL);
//      mySerial.println(": FILL for 3 sec");

      digitalWrite(valve_algae_water, OPEN);
      digitalWrite(valve_clean_water, CLOSE);
      digitalWrite(valve_drain_water, CLOSE);

      valst = valst +1;
      digitalWrite(pin_led_r, mode % 2);   // turn the LED on (HIGH is the voltage level)

      delay(valve_hold_time);
      valst = SAMPLE;

      if(automanual == 0)
        break;

    case SAMPLE:
//      mySerial.print(SAMPLE);
//      mySerial.println(" SAMPLE for 10 sec");

      digitalWrite(valve_algae_water, CLOSE);
      digitalWrite(valve_clean_water, CLOSE);
      digitalWrite(valve_drain_water, CLOSE);

      digitalWrite(pin_led_r, HIGH);   // turn the LED on (HIGH is the voltage level)

      led_sampling();
      valst = valst +1;

      if(automanual == 0)
        break;
      // valvestate = DRAIN_AFTER;


    case DRAIN_AFTER:
//      mySerial.print(DRAIN_AFTER);
//      mySerial.println(" DRAIN_AFTER for 2 sec");

      digitalWrite(valve_algae_water, CLOSE);
      digitalWrite(valve_clean_water, CLOSE);
      digitalWrite(valve_drain_water, OPEN);

      digitalWrite(pin_led_r, mode % 2);   // turn the LED on (HIGH is the voltage level)

      delay(valve_hold_time);
      valst = valst +1;

      if(automanual == 0)
        break;
      // valvestate = CLEAN_AFTER;


    case CLEAN_AFTER:
//        mySerial.print(CLEAN_AFTER);
//        mySerial.println(" CLEAN_AFTER for 2 sec");

        digitalWrite(valve_algae_water, CLOSE);
        digitalWrite(valve_clean_water, OPEN);
        digitalWrite(valve_drain_water, OPEN);

        valst = 0;
        digitalWrite(pin_led_r, mode % 2);

        delay(valve_hold_time);
        valst = STANDBY;
        //break;
        // valvestate = STANDBY;

    case STANDBY:
//      mySerial.print("STANDBY called:");
//      mySerial.println(valst);

      digitalWrite(valve_algae_water, CLOSE);
      digitalWrite(valve_clean_water, CLOSE);
      digitalWrite(valve_drain_water, CLOSE);

//      mySerial.print(STANDBY);
//      mySerial.println(" STANDBY ");
      digitalWrite(pin_led_r, LOW);

      for (i =0;i<max_sample_cnt;i++){
        data_arr[i] = 0.0;
      }
       valst = STANDBY;

        break;

      default:
        break;

  }// end of switch

  //mySerial.print(" state executed  ");
  analogWrite(pin_led_r, mode % 2);   // turn the LED on (HIGH is the voltage level)
  return valst;

}// end of valve_sequence

// manual open/close test
void sampling_valve_test(){

  mode = mode +1;
  mySerial.print(mode);

  if(mode % 2 ==0){
    digitalWrite(valve_algae_water, CLOSE);//LOW, 0V
    digitalWrite(valve_clean_water, CLOSE);
    digitalWrite(valve_drain_water, CLOSE);

    digitalWrite(pin_led_r, LOW);   // turn the LED on (HIGH is the voltage level)
    mySerial.println(" closed");
  }
  else{
    digitalWrite(valve_algae_water, OPEN);//high, 3.3V
    digitalWrite(valve_clean_water, OPEN);
    digitalWrite(valve_drain_water, OPEN);

    digitalWrite(pin_led_r, HIGH);   // turn the LED on (HIGH is the voltage level)

      mySerial.println(" open");
  }

}

// control led and read sensor data
void led_sampling(){
    // mySerial.println(" led_sampling");
    start_time = millis();

    while (data_cnt <= max_sample_cnt) {

      inChar = mySerial.read();

      if (inChar == 'c') {
        color_cnt = 0;
        data_cnt = 0;
        // valst = STANDBY; // start
        break;
      }

      RGBscan(scan_time);

      hold_start = millis();
      hold_led(mode, color_cnt);

      if (data_cnt >= 1 && data_cnt <= max_sample_cnt)
        display(scan_time / max_sample_cnt);

        end_time = millis();
        scan_period = end_time - start_time;

        if (color_cnt >= end_color_cnt) { //violet
            mySerial.print(" ");
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

  

  // cnt =1; //for red led mode

  if (color_cnt <= 255) {
    // redBrightness = 255 - color_cnt;
//    redBrightness = 255;
    redBrightness = 64;
    //redBrightness = redBrightness + 255/max_sample_cnt*data_cnt;
    
    
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
//    mySerial.println("stanby 2 sec for algae sampling");

    while (hold_end - hold_start < 2000) {
      hold_end = millis();
      delay(100);
//      mySerial.println("..");
      digitalWrite(pin_led_test, HIGH);
    }

    digitalWrite(pin_led_test, LOW);

    if (color_cnt == end_color_cnt) {
      //mySerial.println(" end_color_cnt");
      digitalWrite(pin_led_test, HIGH);
    }
  }
}

// print data to mySerial port
void display(int update_time) {

   if(data_cnt ==1){
    
//     mySerial.print("Color ");
//     mySerial.print("Idx ");
//     mySerial.print("Data ");
//     mySerial.println("Avg ");
   }
      cell_val = analogRead(pin_cell_read2);
      voltage= cell_val * (3.3 / 1023.0);
      data_arr[data_cnt] = voltage;

       // average 5 data

      if(data_cnt <=avg_group_num){
        data_sum = data_sum + data_arr[data_cnt];
        data_avg= data_sum/data_cnt;
      }
      else{
          //for(i == data_cnt-(avg_data_num-1);i==data_cnt;i++){
            data_sum = data_sum + data_arr[data_cnt]- data_arr[data_cnt-(avg_group_num)];
            data_avg= (float) data_sum/avg_group_num;
          //}
      }

      mySerial.print(color_cnt);
      mySerial.print("  ");
      mySerial.print(data_cnt);
      mySerial.print("  ");

      mySerial.print(data_arr[data_cnt]);
      mySerial.print("  ");

      mySerial.println(data_avg);

      delay(update_time);
}

#define SCB_AIRCR (*(volatile uint32_t *)0xE000ED0C) // Application Interrupt and Reset Control location

// soft reset
void _softRestart()
{
  mySerial.println("Reset");
  mySerial.end();  //clears the mySerial monitor  if used
  SCB_AIRCR = 0x05FA0004;  //write value for restart
}
