/*
  ========================================================
  =====  Copyright(c) 2020 - 2021. Bozhidar Ivaylov  =====
  ========================================================
   
*/

/** The FRAME we have to send to the Gateway has the following structure:
 *  
 *  [ node_group | node_id | node_stat | payload_len | payload ]
 *  
 *  - NodeGroup is the type of device or what kid of sensors we have attached to our device. 
 *    We use 1 byte so far to represent the group which are 256 combinations. 
 *    
 *  - NodeId: The node id within a certain group  
 *  
 *  - PAYLOAD: The payload may have any size, however when we program the terminal node, 
 *    we need to set the size manually.
 *    
 *      More info at: www.heltec.cn
        this project also realess in GitHub:
        https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series
**/


#include "heltec.h"
#include <dht.h>

#define dht_apin 13 //Analog pin 4. Physical pin: 13

#define BAND 868E6    

#define NODE_GROUP 0
#define NODE_ID    0  
#define PAYLOAD_SIZE 1
#define FRAME_SIZE (PAYLOAD_SIZE+4)    

byte FRAME_DATA[FRAME_SIZE];
byte PAYLOAD[PAYLOAD_SIZE];

dht DHT;

void setup(){
  
  //Initialize the LORA Heltec module
  Heltec.begin(false /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);

  //Configure the LORA receiver parameters
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setPreambleLength(8);
  LoRa.disableCrc();
  LoRa.setSyncWord(0x12);

  Serial.begin(9600);

  //Initialize temperature/humidity sensor
  DHT.read11(dht_apin);

  //initDisplay();
}


void loop(){
  
  setHeader();
  setPayload();
  sendPacket();

  delay(5000);
}

//Set the values for the header such as: NODE_ID, FRAME_ID. PAYLOAD, etc
void setHeader(){
  
  //Clear the frame before setting the new data
  for(int i=0; i < sizeof(FRAME_DATA); i++){
      FRAME_DATA[i] = 0x0;
  }
  
  //Set the frame HEADER. The header size is fixed
  FRAME_DATA[0] = (byte)NODE_GROUP;        //Device Group: 0 - 0xFF
  FRAME_DATA[1] = (byte)NODE_ID;           //Device ID:    0 - 0xFF   
  FRAME_DATA[2] = (byte)getDeviceStatus(); //Device status:0 - 0xFF; 
  FRAME_DATA[3] = PAYLOAD_SIZE;
}

//Put some data in our payload array
//The available bytes from the frame are [4 - 17]
void setPayload(){
  
  //Set the payload data. This is only a simulation.
  //We can read the temperature or the pressure for example 
  //and store it to our PAYLOAD buffer

  FRAME_DATA[4] = getTemp();

  Serial.println(FRAME_DATA[4]);
}

//Send the packet to the Gateway
void sendPacket(){
  
  //Send packet
  LoRa.beginPacket();

  //Send the FRAME header and the payload to the Gateway
  for(int i = 0; i < FRAME_SIZE; i++){
      LoRa.write(FRAME_DATA[i]);
  }
  LoRa.endPacket();
}


//Returns the device's status code.
//We can define a device status codes and notify the
//Gateway in case of some system malfunction
byte getDeviceStatus(){

    byte devStatus = 0;
  
    return devStatus;
}


/**Reads the temperature from a sensor **/
byte getTemp(){
  
  byte temp = (byte)DHT.temperature;
  
  return temp;
}

void initDisplay(){
  
  //Configure the display of our gateway. 
  //This is only for debugging, we'll remove this later
  /*Heltec.display->init();
   
  Heltec.display->flipScreenVertically();
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);

  Heltec.display->clear();
  Heltec.display->drawString(0,0,"firulays <(n.n)>");
  Heltec.display->display();*/
}
