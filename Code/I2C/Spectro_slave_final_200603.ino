// Wire Master Writer
// 

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// 
// Created June 3, 2020

// This example code is in the public domain.


#include <Wire.h>
byte code  =0;                   //used to hold the I2C response code from slave.
char in_data[20];                //incoming data from slave
byte i = 0;                      //counter 
char inChar;

void setup() {
  Wire.begin(); // join i2c bus (address optional for master)
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);  // start serial for output
}

// byte x=0,y;
byte mode = 0;

void loop() {
  if (Serial.available() ) {
    inChar = Serial.read();
  
    if(inChar != 0){
      Wire.beginTransmission(8); // transmit to device #8
      Wire.write(inChar);        // sends five bytes
      Serial.println(inChar);        // sends five bytes
      
      Wire.endTransmission();    // stop transmitting
     memset(in_data, 0, sizeof(in_data));  

    Wire.requestFrom(8, 48);    // request 6 bytes from slave device #2
    code = Wire.read();   

    if(code != NULL){
      mode = !mode; // led blinking
      digitalWrite(LED_BUILTIN, mode);
    }
    
    if(code ==1){
        Serial.println("Success");        //means the command was successful.
    }
    else if(code ==2){     
        Serial.println("not ready");        
     }
     else if(code ==254){     
         Serial.println("circuit not ready");        //means the command was successful.
     }
    else if(code==255)     
        Serial.println("No data");       
    
      while(Wire.available())    // slave may send less than requested
      { 
        char c = Wire.read(); // receive a byte as character
        in_data[i]=c;
        i+=1;

          if (c == 0 || c == NULL || c=='\n') {                  //if we see that we have been sent a null command.
              i = 0;                            //reset the counter i to 0.
              break;                            //exit the while loop.
        }
       }
       Serial.println(in_data);              //print the data.
    }
  } 
}
