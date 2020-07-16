
// control led and read sensor data
void led_sampling(){
    start_time = millis();

    while (data_cnt <= max_sample_cnt) {

      inChar = Serial.read();

      if (inChar == 'c') {
        color_cnt = 0;
        data_cnt = 0;
        data_sum = 0;

        break;
      }

      RGBscan(scan_time);

      hold_start = millis();
      hold_led(cnt, color_cnt);

      data_cnt = data_cnt + 1;
      delay(50);

      if (data_cnt >= 1 && data_cnt <= max_sample_cnt)

//      adc_convert1();
        adc_convert_mapping();
        Serial.print(data_cnt);
        Serial.print(" ");
        data_arr[data_cnt] = vol_convert_mapping;
        
        // Serial.println(slave_output);
    
        
        end_time = millis();
        scan_period = end_time - start_time;


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
           
         Serial.print(vol_convert_mapping);
          Serial.print(" ");
          Serial.println(data_avg);

        dtostrf(data_avg,5,f_precision,slave_output); // f_precision : digits under floating point

        if ( data_cnt >= max_sample_cnt){
          data_cnt = 0;
          color_cnt = 0;
          data_sum = 0;
          data_avg = 0;
          analogWrite(pin_led_r, 0);
          break;
        }
        
       
    }// end of while in 's'
   

    if (color_cnt >= end_color_cnt) { //violet
          Serial.print(" ");
          color_cnt = 0 ;
          data_cnt = 0;
    }

   

}

// change led light continuously
// https://www.electronicshub.org/rgb-led-with-arduino/#How_to_Generate_Colors_using_RGB_LED
void RGBscan(int scan_time) {
  // cnt =1; //for red led mode

  if (color_cnt <= 255) {
    greenBrightness = 0;
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
void hold_led(int cnt, int color_cnt) {


  // hold led for 2 sec to start sampling
  if (data_cnt == 1 || color_cnt == end_color_cnt) { // stay longer when start scanning and finishing
//    mySerial.println("stanby 2 sec for algae sampling");

    while (hold_end - hold_start < 2000) {
      hold_end = millis();
      delay(100);
      digitalWrite(pin_led_test, HIGH);
    }

    digitalWrite(pin_led_test, LOW);

    if (color_cnt == end_color_cnt) {
      digitalWrite(pin_led_test, HIGH);
    }
  }
}

// print data to mySerial port
// void display(int update_time) {
void adc_convert(float update_time) {  
      digi_cell_val = analogRead(pin_cell_read2);
      vol_cell_val= digi_cell_val * (3.3 / 1023.0);
      data_arr[data_cnt] = vol_cell_val;


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
       int f_precision = 4;
      dtostrf(data_avg,5,f_precision,slave_output); // f_precision : digits under floating point

}
