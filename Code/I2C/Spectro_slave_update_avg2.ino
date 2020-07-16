/*
Project: EOS spectrophotometer
Subroutine: led sensing and valve control
Version: <1.0 (development) >
Date: <2020/06/03>
Author/Editor:  Jinseong Lee
Notes:
 pin map is updated

//valve sequene

  1. valve open
  2. sensor read
  3. valve open
*/


// #include "sensor_data.h"
#include <Wire.h>
#include "Keyboard.h"


#define CLOSE HIGH //3.3V
#define OPEN LOW  //0 V

#define  end_color  0 //0: red2red, 1: violet

// RGB LED end color mode
#if end_color == 0
  #define end_color_cnt 768 // from red to red
#else
  #define end_color_cnt 640 //from red to red
#endif


# define max_sample_cnt 100// max data

// valve control
enum valvestate {STANDBY =0,DRAIN_BEFORE,FILL,SAMPLE,DRAIN_AFTER};
valvestate valst; //valve state
 bool valve_mode = LOW;


/*  pin assignment */
 int led_pin = 13;
 int pin_led_test = 0;

 int pin_led_r = A22;
 int pin_led_g = 20;
 int pin_led_b = 21;
 int valve_drain_water= 15;
 int valve_algae_water= 16;
 int valve_clean_water= 17;

 int pin_power_3_3V_out = A18; // power check
 int pin_cell_read2 = A0; // pin14, to read a light sensor


/*  counter and timiing */
 bool pump_vol = LOW, last_pump_vol= LOW;

// pump trigger for future use
 //int pin_pump_3_3V_out = 9; // test

 volatile char inChar_i2c;

 bool automanual; //0: manual, 1: auto,

 float update_time = 1000.0; // display update rate
 int scan_time = 10000; // sampling time
 int valve_hold_time = 5000; // valve hold time
 float scan_period = 0, end_time = 0 , start_time = 0;
 float hold_end , hold_start = 0.0; // to hold led for start sampling

 int i = 0,cnt, heatbeat_cnt; //index
 bool enable_sampling = false; // activated by main pump trigger
 byte turn;


// LED parameters
 volatile int redBrightness = 148;
 int greenBrightness;
 int blueBrightness;


/* sample data parameters */
 float vol_led_r, vol_cell_val, inv_vol_cell_val = 0.0;
 int digi_led_r,digi_cell_val = 0; // sensor reading
 int data_cnt = 1;
 float data_arr[max_sample_cnt]; // store sample data
 float data_avg = 0.0;
 float data_sum = 0.0;

 int color_cnt = 1; //color code
 int avg_group_num = 5; // moving element number
int f_precision = 3;

// sampling range
 float max_sensor_range = 3.2; // raw sensor data at darkest light
 float min_sensor_range = 0.0; // raw sensor data at brightest light
 float s2_min = 0.0; // spectro2 min result in the range`
 float vol_convert_mapping; // spectro1 sampling result after converting to spectro2
 
 bool mode = LOW;

//  float voltage = 0.0F;
 char slave_output[20]; // 

// health check
 bool prev_beat_mode= LOW, beat_mode = LOW;
 volatile bool led_mode = LOW;
 byte flag_alive = 0;
 volatile int flag_power_alive = 1;


 float power_mode = 0;


void initialize(){
  mode = 0;
  valst = STANDBY;
  color_cnt = 0;
  data_cnt = 0;
  enable_sampling = true;
  heatbeat_cnt = 0;
 
  analogWrite(pin_led_r, 0);   
  analogWrite(pin_led_g, 0);  
  analogWrite(pin_led_b, 0);   

  digitalWrite(valve_algae_water, CLOSE);
  digitalWrite(valve_clean_water, CLOSE);
  digitalWrite(valve_drain_water, CLOSE);
  
  
}

void setup()
{
  pinMode(valve_algae_water, OUTPUT);
  pinMode(valve_clean_water, OUTPUT);
  pinMode(valve_drain_water, OUTPUT);
  pinMode(led_pin, OUTPUT);
  pinMode(pin_led_g, OUTPUT);
  pinMode(pin_led_r, OUTPUT);
  pinMode(pin_led_b, OUTPUT);
  pinMode(pin_cell_read2, INPUT);

  digitalWrite(valve_algae_water, CLOSE);
  digitalWrite(valve_clean_water, CLOSE);
  digitalWrite(valve_drain_water, CLOSE);


  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent); // register event

  Serial.begin(9600);
  Serial.println("Hello, Serial");
  
  power_check();
  initialize();
}

void loop() {
  led_beat();
}


// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  while ( Wire.available()) { // loop through all but the last
    inChar_i2c= Wire.read(); // receive byte as a character

  }
}


void requestEvent(){

  mode = !mode;
  digitalWrite(led_pin, mode);

  switch(inChar_i2c){
    case 'i':// init
      
      initialize();
       sprintf (slave_output, "initialized ");
       Serial.println(" initialized");
       break;
    
    case 'z':// reset
      
      analogWrite(pin_led_r, LOW);
      analogWrite(pin_led_g, LOW);
      analogWrite(pin_led_b, LOW);
      
      sprintf (slave_output, "reset ");
      Serial.println(" reset");
      
       Wire.write(1);
       Wire.write(slave_output); 
       delay(500);
        
      _softRestart();
     
     break;
      

    case 'h':// health status
         power_check();
        sprintf (slave_output, "%3d", flag_power_alive);
      break;   

    case 'a':// health status
      sampling_valve_test();
      break; 
  
    case 'r'://  100 samples with valve control
      valve_sequence(true,  1);
      break; 

    case 't'://  100 samples
      digitalWrite(valve_algae_water, CLOSE);
      digitalWrite(valve_clean_water, CLOSE);
      digitalWrite(valve_drain_water, CLOSE);
      analogWrite(pin_led_r, redBrightness) ;
      delay(500);
      led_sampling();
      analogWrite(pin_led_r, 0);

      break;

    case 's'://  single readout under led light
      analogWrite(pin_led_r, redBrightness) ;
      delay(500);
      adc_convert_mapping();
      dtostrf(vol_convert_mapping,5,f_precision,slave_output); // f_precision : digits under floating point
      analogWrite(pin_led_r, 0);
      

    case 'o'://  single readout under ambient
      adc_convert_mapping();  
      dtostrf(vol_convert_mapping,5,f_precision,slave_output); // f_precision : digits under floating point
      break;

    case 'm': // data mapping
      adc_convert_mapping();  
      dtostrf(vol_convert_mapping,5,f_precision,slave_output); // f_precision : digits under floating point
      break;

    case 'u': // brighter led
      redBrightness  += 2;
        analogWrite(pin_led_r, redBrightness) ;
      sprintf (slave_output, "Up to %3d: ", redBrightness);
      break;

    case 'd': //darker led
      redBrightness  -= 2;
      analogWrite(pin_led_r, redBrightness) ;
      sprintf (slave_output, "Down to %3d", redBrightness);
   
      break;
   
    case 'l': // led on/off
      led_mode = !led_mode;
      if(led_mode == HIGH){
        analogWrite(pin_led_r, redBrightness) ;
        sprintf (slave_output, "led on ");
      }
      else{
        analogWrite(pin_led_r, 0) ;
        sprintf (slave_output, "led off ");
      }
      
      break;
    
    case 'v': // valve on/off
      valve_mode = !valve_mode;

      if(valve_mode == HIGH){
        digitalWrite(valve_drain_water, OPEN);
        sprintf (slave_output, "valve on");
      }
      else{
        digitalWrite(valve_drain_water, CLOSE);
        sprintf (slave_output, "valve close");
      }
      break;
  
   }
 
  if(flag_power_alive == 1)
      Wire.write(1); 
  else if(flag_power_alive == 254)
      Wire.write(254); 
  
  if(inChar_i2c !='z'){
    Wire.write(slave_output); 
    Wire.flush();
    inChar_i2c = 0;
  }

}


// Health status checking
void led_beat(){
  beat_mode = !beat_mode;
  delay(500);
  digitalWrite(led_pin,beat_mode);
  
  if(prev_beat_mode != beat_mode)
    flag_alive = 1; //Success
  else
    flag_alive = 2; // Fail

  Serial.print("led beat: ");
  Serial.println(flag_alive);
  prev_beat_mode = beat_mode;
}


void power_check(){
    power_mode = analogRead (pin_power_3_3V_out);
    Serial.println(power_mode);
  
    if( power_mode >1000 ){
      flag_power_alive = 1; //Success
    
    }
    else{
      flag_power_alive = 254; // Fail
    }
    Serial.println(flag_power_alive);
}


#define SCB_AIRCR (*(volatile uint32_t *)0xE000ED0C) // Application Interrupt and Reset Control location

// soft reset
void _softRestart()
{
  Serial.println("Reset");
  Serial.end();  //clears the mySerial monitor  if used
  SCB_AIRCR = 0x05FA0004;  //write value for restart
}

// map one spectrophotometer to the other (scale of 2.0)
void adc_convert_mapping() {
    digi_cell_val = analogRead(pin_cell_read2);
    vol_cell_val= digi_cell_val * (3.3 / 1023.0);
    
    vol_convert_mapping = (2 - s2_min)/3.2*vol_cell_val + s2_min; // mapping function

}
