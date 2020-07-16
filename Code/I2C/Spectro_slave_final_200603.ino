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
//#include "SPECTRO_VARIABLE.h"


#define CLOSE HIGH //3.3V
#define OPEN LOW  //0 V

enum valvestate {STANDBY =0,DRAIN_BEFORE,FILL,SAMPLE,DRAIN_AFTER};

valvestate valst; //valve state

#define  end_color  0 //0: red2red, 1: violet

// RGB LED end color mode
#if end_color == 0
  #define end_color_cnt 768 // from red to red
#else
  #define end_color_cnt 640 //from red to red
#endif

 int led_pin = 13;
//  int pin_sig_high = 2; // test
 int pin_led_test = 0;

// int pin_led_r = 14;
  int pin_led_r = A22;
  // int pin_led_r_copy = A21;


 int pin_led_g = 20;
 int pin_led_b = 21;

// pump trigger
 //int pin_pump_3_3V_out = 9; // test

// int pin_power_3_3V_out = 37; // test
 int pin_power_3_3V_out = A18; // test
//  int pin_pump_input = 11; // pin to read a light sensor

 int pin_cell_read2 = A0; // pin14, to read a light sensor
 bool pump_vol = LOW, last_pump_vol= LOW;

// after May 5,2020 afternoon
 int valve_drain_water= 15;
 int valve_algae_water= 16;
 int valve_clean_water= 17;

 char inChar;
 volatile char inChar_i2c;

 char state;// procedure state
 bool automanual; //0: manual, 1: auto,

 float update_time = 1000.0; // display update rate
 int scan_time = 10000; // sampling time
 int valve_hold_time = 5000; // valve hold time

 float scan_period = 0, end_time = 0 , start_time = 0;

 int i = 0,cnt, heatbeat_cnt; //index
 bool enable_sampling = false; // activated by main pump trigger
 byte turn;

 int digi_led_r,digi_cell_val = 0; // sensor reading

 float vol_led_r, vol_cell_val, inv_vol_cell_val = 0.0;

 float hold_end , hold_start = 0.0; // to hold led for start sampling

 int data_cnt = 1;

# define max_sample_cnt 100// max data

// float data_arr[max_sample_cnt] = {0.0}; // store sample data
 float data_arr[max_sample_cnt]; // store sample data
 float data_avg = 0.0;
 int data_avg_int;
 float data_sum = 0.0;

 int color_cnt = 1; //color code
 int avg_group_num = 5;
 int valve_num = 1; //each valve indicator

 volatile int redBrightness = 148;
 int greenBrightness;
 int blueBrightness;

 int inc = 2;
//  int inc_dir = 1;

 bool mode = LOW;

 float voltage = 0.0F;

//char slave_output[5]; // error
 char slave_output[20]; // error
 char* output_sum; // error

 float max_sensor_range = 3.2; // raw sensor data at darkest light
 float min_sensor_range = 0.0; // raw sensor data at brightest light
 float s2_min = 0.0; // spectro2 min result in the range`
 float vol_convert_mapping; // spectro1 sampling result after converting to spectro2
 

//volatile char slave_output[7];

 bool prev_beat_mode= LOW, beat_mode = LOW;
 volatile bool led_mode = LOW;
 bool valve_mode = LOW;
 byte flag_alive = 0;
 int f_precision = 4;

//  int rcv_cnt,req_cnt = 0;

// bool power_mode = LOW;
 float power_mode = 0;
//  int power_check_pin = 23;
 volatile int flag_power_alive;

void initialize(){
  mode = 0;
  valst = STANDBY;
  color_cnt = 0;
  data_cnt = 0;
  enable_sampling = true;
  heatbeat_cnt = 0;
  // rcv_cnt = 0;
  // req_cnt = 0;

  analogWrite(pin_led_r, 0);   // turn the LED on (HIGH is the voltage level)
  analogWrite(pin_led_g, 0);   // turn the LED on (HIGH is the voltage level)
  analogWrite(pin_led_b, 0);   // turn the LED on (HIGH is the voltage level)

  digitalWrite(valve_algae_water, CLOSE);
  digitalWrite(valve_clean_water, CLOSE);
  digitalWrite(valve_drain_water, CLOSE);
  
  
  
}

void setup()
{
  // pinMode(pin_sig_high, OUTPUT);

  pinMode(valve_algae_water, OUTPUT);
  pinMode(valve_clean_water, OUTPUT);
  pinMode(valve_drain_water, OUTPUT);

  pinMode(led_pin, OUTPUT);
  pinMode(pin_led_g, OUTPUT);
  pinMode(pin_led_r, OUTPUT);
  pinMode(pin_led_b, OUTPUT);

  // pinMode(pin_led_r_copy, INPUT);
  pinMode(pin_cell_read2, INPUT);
  // pinMode(pin_pump_input, INPUT);
  // pinMode(pin_pump_3_3V_out, OUTPUT);

  //
  digitalWrite(valve_algae_water, HIGH);

  //before 200428
  digitalWrite(valve_algae_water, CLOSE);
  digitalWrite(valve_clean_water, CLOSE);
  digitalWrite(valve_drain_water, CLOSE);


  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  //added
  Wire.onRequest(requestEvent); // register event

  // mySerial.begin(9600);

  // mySerial.println("Hello, mySerial");
  Serial.begin(9600);
  Serial.println("Hello, Serial");
  
  power_check();
  initialize();
  }

void loop() {
  
}



// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  
  // while (1 <= Wire.available()) { // loop through all but the last
  while ( Wire.available()) { // loop through all but the last
    inChar_i2c= Wire.read(); // receive byte as a character

    if(inChar_i2c ==0){  
      Serial.println(inChar_i2c);
      
       Serial.println("NUll is received");
    }
    else{
      Serial.println(inChar_i2c);
      
       Serial.println("Not NUll is received");

    }
  }
}

int req_cnt;

void requestEvent(){

//  if(inChar_i2c ==NULL || inChar_i2c ==''){  
   if(inChar_i2c == 0 ){  
  Serial.println(inChar_i2c);
  Wire.write(255);
  sprintf (slave_output, "NUll is sent");
 }
 else{
  req_cnt += 1;
    Serial.print("req_cnt:");
    Serial.println(req_cnt);

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
      delay(200);
       Wire.write(1);
       Wire.write(slave_output); 
        
      _softRestart();
     
     break;
      

    case 'h':// health status
         power_check();
        sprintf (slave_output, "%3d", flag_power_alive);
      break;   

    case 'a':// health status
      sampling_valve_test();
      break; 
  
    case 'r'://  chang eto 's'
      valve_sequence(true,  1);
      break; 

    case 't'://  chang eto 's'
      digitalWrite(valve_algae_water, CLOSE);
      digitalWrite(valve_clean_water, CLOSE);
      digitalWrite(valve_drain_water, CLOSE);
      analogWrite(pin_led_r, redBrightness) ;
      delay(100);
      led_sampling();
      analogWrite(pin_led_r, 0);

      // Wire.write(1);
      // Wire.write(slave_output,4);
      // Serial.println(slave_output);
      break;

    case 's'://  led sensing
      // Serial.print(" s req");
      analogWrite(pin_led_r, redBrightness) ;
      delay(500);
      adc_convert_mapping();
      analogWrite(pin_led_r, 0);
      

    case 'o'://  led sensing
      // adc_convert1();    
      adc_convert_mapping();
      break;

    case 'u':
      redBrightness  += 2;
        analogWrite(pin_led_r, redBrightness) ;
      sprintf (slave_output, "Up to %3d: ", redBrightness);
      break;

    case 'd':
      redBrightness  -= 2;
      analogWrite(pin_led_r, redBrightness) ;
      sprintf (slave_output, "Down to %3d", redBrightness);
   
      break;
   
    case 'l':
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
    
    case 'v':
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

    case 'm':
      adc_convert_mapping();  
      break;

  

    }
     Wire.write(1);
    Wire.write(slave_output); 
    Wire.flush();
    inChar_i2c = 0;
 }
 /*
    if(inChar_i2c ==NULL)
      Wire.write('255');
    else{
      if(inChar_i2c != 'z' || inChar_i2c != 'h'){
        Wire.write(1);
        Wire.write(slave_output); 
      }
    }
  Wire.flush();
  */

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


// void adc_convert(int update_time) {
void adc_convert1() {
    digi_cell_val = analogRead(pin_cell_read2);
      vol_cell_val= digi_cell_val * (3.3 / 1023.0);
      dtostrf(vol_cell_val,5,f_precision,slave_output); // f_precision : digits under floating point
}

// map one spectrophotometer to the other (scale of 2.0)
void adc_convert_mapping() {
  
    digi_cell_val = analogRead(pin_cell_read2);
    vol_cell_val= digi_cell_val * (3.3 / 1023.0);
    
    // inv_vol_cell_val = abs(max_sensor_range - vol_cell_val);
    inv_vol_cell_val = vol_cell_val;

    /* How to derive mapping function
       spectro 1, 
       s1_range = 3.2-0 = 3.2

       spectro 2, s2_min to 2
       s1_range = 2-s2_min 


      s2_vol_mapping = y2 = (2-a)x + a(offset)
                          = (2-a)(y1/3.2) + a
                          = (2-a)/3.2*y1 +a

        when y1 = 0, y2 = a                          
        when y1 = 3.2, y2 = 2
        Correct !
    */                     
       

    vol_convert_mapping = (2 - s2_min)/3.2*inv_vol_cell_val + s2_min;
    dtostrf(vol_convert_mapping,5,f_precision,slave_output); // f_precision : digits under floating point


/*
  digi_cell_val = analogRead(pin_cell_read2);

   digi_cell_val = analogRead(pin_cell_read2);
  vol_cell_val= digi_cell_val * (3.3 / 1023.0);
  inv_vol_cell_val = abs(max_sensor_range - vol_cell_val);
  data_arr[data_cnt] = inv_vol_cell_val;

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
      // display_cnt_sample_value();
      
      return data_avg;
      */
}


void run_adc(){
  do{
    data_cnt = data_cnt + 1;
    adc_convert1();
    Serial.print(slave_output);
    Serial.print(" ");
    Serial.println(data_cnt);
    // sprintf (slave_output, "done");
    Wire.write(1);
    Wire.write(slave_output); 
      
  
  }while(data_cnt<max_sample_cnt);

  if(data_cnt==max_sample_cnt)
    data_cnt =0;
}
