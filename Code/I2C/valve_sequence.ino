// auto sampling
int valve_sequence(bool automanual, int valst){// sampling
//  void valve_sequence(bool automanual, int valst){// sampling

  switch(valst){
    case DRAIN_BEFORE:

      digitalWrite(valve_algae_water, OPEN);
      digitalWrite(valve_clean_water, CLOSE);
      digitalWrite(valve_drain_water, OPEN);

      valst = valst +1;
      digitalWrite(pin_led_r, LOW);   
      delay(valve_hold_time*2);

      if(automanual == 0)
        break;

    case FILL:

      digitalWrite(valve_algae_water, OPEN);
      digitalWrite(valve_clean_water, CLOSE);
      digitalWrite(valve_drain_water, CLOSE);

      valst = valst +1;
      digitalWrite(pin_led_r, LOW);   

      delay(valve_hold_time); // to pull upto date algae stream

      if(automanual == 0)
        break;

    case SAMPLE:

      digitalWrite(valve_algae_water, CLOSE);
      digitalWrite(valve_clean_water, CLOSE);
      digitalWrite(valve_drain_water, CLOSE);

      digitalWrite(pin_led_r, HIGH);   // turn the LED on (HIGH is the voltage level)
     

      led_sampling();
      analogWrite(pin_led_r, 0);
      valst = valst +1;

      if(automanual == 0)
        break;

    case DRAIN_AFTER:

      digitalWrite(valve_algae_water, OPEN);
      digitalWrite(valve_clean_water, CLOSE);
      digitalWrite(valve_drain_water, OPEN);

      analogWrite(pin_led_r, 0);  

      delay(valve_hold_time);
      valst = 0;
   

      if(automanual == 0)
        break;
   
    case STANDBY:

      digitalWrite(valve_algae_water, CLOSE);
      digitalWrite(valve_clean_water, CLOSE);
      digitalWrite(valve_drain_water, CLOSE);

      analogWrite(pin_led_r, 0);

      for (i =0;i<max_sample_cnt;i++){
        data_arr[i] = 0.0;
      }
       
      break;

    default:
      break;

  }// end of switch

  return valst;

}// end of valve_sequence

// manual open/close test
void sampling_valve_test(){

  cnt = cnt +1;
  Serial.print(cnt);

  if(cnt % 2 ==1){
    digitalWrite(valve_algae_water, OPEN);//high, 3.3V
    digitalWrite(valve_clean_water, OPEN);
    digitalWrite(valve_drain_water, OPEN);

    analogWrite(pin_led_r, 255);   // turn the LED on (HIGH is the voltage level)
    
    sprintf (slave_output, "valve open/led on");
    
  }
  else{
    digitalWrite(valve_algae_water, CLOSE);//LOW, 0V
    digitalWrite(valve_clean_water, CLOSE);
    digitalWrite(valve_drain_water, CLOSE);

    analogWrite(pin_led_r, 0);   // turn the LED on (HIGH is the voltage level)
    sprintf (slave_output, "valve close/led off");
    
      cnt = 0;
  }

}
