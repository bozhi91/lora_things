
#include "heltec.h"

void setup(){
  
  Serial2.begin(115200, SERIAL_8N1, 16, 17); //FOR GSM COMMUNICATION
  Serial.begin(115200); //FOR DEBUGGING
  
  delay(1000);
}

void loop(){
  
     if(Serial.available() > 0){
        Serial2.write(Serial.read()); 
     }
     if(Serial2.available() > 0){
        Serial.write(Serial2.read()); 
     }
}


void sendUARTExpect(String command, String expected){
  
  Serial2.println(command);
  delay(50);
  
  while(Serial2.available()){
     String value = Serial2.readString();

     if(value.indexOf(expected)!=-1){
        Serial.write("[received - OK]");
      }
      else{
         Serial.write("[BAD ANSWER -->");
        Serial.println(value);
        }
     
     //Serial.write(Serial2.read());
  }
}
