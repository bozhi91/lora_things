/* 
 *   ===== LORA Things Project =====
 *  
 *   Copyright(c) 2020-2021. Bozhidar Ivaylov
 *  
 * More info at: www.heltec.cn
 * This project also realess in GitHub:
 * https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series
*/

#include "heltec.h"

#define BAND 868E6    
#define PAYLOAD_SIZE 20 //The maximum gateway 
#define GATEWAY_ID 0

//Define some global variables
int GATEWAY_FRAME[PAYLOAD_SIZE + 3]; //The frame we send to the server(over GSM)
bool PROCESSING_FRAME = false;
bool GSM_UART_BUSY    = false;
bool GSM_INITIALIZED  = false;

/************************************************************/
//GSM AT Commands
String INIT_GPRS[][2] = {
  
  {"AT", "OK"},
  {"AT+SAPBR=3,1, \"Contype\", \"GPRS\"","OK"},
  {"AT+SAPBR=3,1, \"APN\", \"ac.vodafone.es\"", "OK"},
  {"AT+SAPBR=1,1", "OK"},
  {"AT+SAPBR=2,1", "OK"}
};

String HTTP_REQUEST[][2] = {
  
  {"AT+HTTPINIT","OK"},
  {"AT+HTTPPARA=\"CID\",1","OK"},
  {"AT+HTTPPARA=\"URL\",\"http://demo.thingsboard.io/api/v1/T4heef9dzbcuejT5qegC/telemetry\"","OK"},
  {"AT+HTTPPARA=\"CONTENT\",\"application/json\"","OK"},
  {"AT+HTTPACTION=1","200"},
  {"AT+HTTPTERM","OK"}
};
/******************************************************/

void setup(){

  /**************************************************************/
    //Initialize the UART port for the GSM module
    
    /* BAUD, STOP BITS, RX, TX*/
    Serial2.begin(115200, SERIAL_8N1, 16, 17); //Configure the 2nd UART for GSM communications
    Serial.begin(115200); //FOR DEBUGGING. MUST BE REMOVED IN PRODUCTION!!!!
    
    while(!Serial); //REMOVE THIS IN PRODUCTION
    while(!Serial2);
  
    //Set the header of the
    GATEWAY_FRAME[0] = GATEWAY_ID;
    
  /**************************************************************/

  /**************************************************************/
    //Initialize the LORA module
    Heltec.begin(false /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
    
    //Configure the LORA receiver parameters
    LoRa.setSpreadingFactor(7);
    LoRa.setSignalBandwidth(125E3);
    LoRa.setCodingRate4(5);
    LoRa.setPreambleLength(8);
    LoRa.disableCrc();
    LoRa.setSyncWord(0x12);
    
    //Register the receiver callback
    LoRa.onReceive(onReceive);
  
    //Put the radio into receive mode
    LoRa.receive();
    
    //LoRa.setTxPower(20,RF_PACONFIG_PASELECT_PABOOST); //20dB output must via PABOOST
    //For more info refer to: https://heltec-automation-docs.readthedocs.io/en/latest/esp32/distance_test.html
  /**************************************************************/
  
   initGSM();
}

//Receive packages using interruption

int gateFrameSize;
int PACK_SIZE;
void onReceive(int packetSize){

   PACK_SIZE = packetSize;
   
   //Clear the Gateway frame before filling the new data
   gateFrameSize = (sizeof(GATEWAY_FRAME)/sizeof(int));

   //Copy the end node frame to the Gateway Frame 
   for(int i = 1; i < gateFrameSize-1; i++){
       GATEWAY_FRAME[i] = 0;
   }
   
   GATEWAY_FRAME[1] = gatewayStatus();
  
   //Store the LORA data to a int array. We'll send this array to the GSM later
   for(int i = 0; i < packetSize; i++){
       GATEWAY_FRAME[(i+2)] = (int)LoRa.read();
   }
   PROCESSING_FRAME = true;
}

void loop(){
  
  //When we receive a frame, do not process other frames until we send the data over GSM
  if(PROCESSING_FRAME && GSM_INITIALIZED){
     PROCESSING_FRAME = false;
     sendHttpsRequest(GATEWAY_FRAME);
  }
  delay(1000);
}

/**Initialize the GPRS module. This operation must be done only ONCE.
   Otherwise, the module may get blocked.*/
void initGSM(){
  
  Serial.write("### Initializing GPRS ###");

  //Send the AT commands to initialzie the GPRS
  for(int i=0; i < 5; i++){
    
    //Wait until the uart is free
    while(GSM_UART_BUSY){
      delay(50);
    }
    sendATCommand(INIT_GPRS[i][0],INIT_GPRS[i][1]);
  }
  
  GSM_INITIALIZED = true;
  Serial.println("### GPRS initialized ###");
}

//Send the frame to the IOT platform over GSM
void sendHttpsRequest(int payload[]){
  
  Serial.println("### Sendig HTTPS Request...");
  
  //Send the HTTP Request
  for(int i=0; i < 4; i++){
    
     while(GSM_UART_BUSY){
        delay(50);
     }
     sendATCommand(HTTP_REQUEST[i][0], HTTP_REQUEST[i][1]);
  }  
  
   /*** Set the PAYLOAD and send the Request ***/
   String myPayload = payloadBuilder();
   sendATCommand("AT+HTTPDATA="+String(myPayload.length())+",10000","DOWNLOAD"); //Set the payload length and timeout=10000 ms
   sendATCommand(myPayload,"OK"); //Set the JSON structure including the payload

   sendATCommand(HTTP_REQUEST[4][0], HTTP_REQUEST[4][1]); //Send the HTTPS Request
   sendATCommand(HTTP_REQUEST[5][0], HTTP_REQUEST[5][1]); //Terminate/Close the HTTPS Request

   //The Frame is sent to the IOT platform and the LORA module is ready to receive more papckages
   PROCESSING_FRAME = false;

   delay(5000);
}

/**
  Send an AT command to the UART adn wait for a specific answer.
  If the answer is not received in certain time, we quit the loop
*/
void sendATCommand(String ATcommand, String expected){
  
  String gsmAnswer = "";
  int counter = 0;    
  
  Serial.println("\n ### Sending command: "+ATcommand);
  
  //Send the command to the UART and block the UART input
  GSM_UART_BUSY = true;

  //Send our AT command to the UART
  if(Serial2.availableForWrite() > ATcommand.length()){
     Serial2.println(ATcommand);
  }
  
  //When we receive an answer from the GSM over the UART,
  //we check if we got the expected answer or not. Then we unblock the UART input
  while( (gsmAnswer.indexOf(expected) == -1) && (counter < 10)){

    gsmAnswer = "";
     
    //Read a string from the UART
    if(Serial2.available()){
      gsmAnswer = Serial2.readString();
    }
    if(gsmAnswer.length() > 0){
       Serial.println("## DATA: "+gsmAnswer);
    }
    
    counter++;
    delay(500);
  }//Wait until the expected answer is received. If not, quit the loop

  counter = 0;
  delay(200);
 
  //Release the UART so we can send the next command
  GSM_UART_BUSY = false;
  //Serial.println("### Answer: ["+gsmAnswer+"] ### \n\n");
  Serial2.flush();
}

/**
 * Converts a int array to String in order to create 
 * the payload data we want to send to the IOT platform
*/
String payloadBuilder(){

  String result;
  int frameSize = PACK_SIZE+2;
  for(int i=0; i < (frameSize - 1); i++){
    result.concat(GATEWAY_FRAME[i]);
    result.concat(",");
  }
  
  result.concat(GATEWAY_FRAME[frameSize - 1]);
  return "{\"payload\":["+result+"]}";
}


/** Returns the current status of the Gateway. */
byte gatewayStatus(){
   byte gateStatus = 0;
   return gateStatus;
}

boolean isAlphaNumeric(String str){
  
    int len = str.length();
    
    for(int i = 0; i<len; i++){
       if(not isalpha(str[i])){
          return false;
        } 
    }
    return true;
}

//https://github.com/ostaquet/Arduino-SIM800L-driver
//Not used for now
void initDisplay(){
  //Configure the display of our gateway. 
  //This is only for debugging, we'll remove this later
  /*Heltec.display->init();
  Heltec.display->flipScreenVertically();  
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->clear();*/
}
