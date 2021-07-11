/* Heltec Automation send communication test example
 *
 * Function:
 * 1. Send data from a CubeCell device over hardware 
 * 
 * 
 * this project also realess in GitHub:
 * https://github.com/HelTecAutomation/ASR650x-Arduino
 * */

#include "LoRaWan_APP.h"
#include "Arduino.h"

//Define the values for the transmitter. All the modules(including gateway) must have the same configuration
//such as: frequency, bandwith, spreading factor, etc.
//Just calibrate the module once, and don't change anything from this section

/*********** LORA CONFIG PARAMETERS ***********/   
  #define RF_FREQUENCY                                868000000 // Hz
  #define LORA_BANDWIDTH                              0         // [0: 125 kHz 1: 250 kHz, 2: 500 kHz,3: Reserved]
  #define TX_OUTPUT_POWER                             14        // dBm: 0 - 20
  #define LORA_SPREADING_FACTOR                       7         // [SF7..SF12]
  #define LORA_CODINGRATE                             1         // [1: 4/5  2: 4/6, 3: 4/7, 4: 4/8]
  #define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
  #define LORA_SYMBOL_TIMEOUT                         0         // Symbols
  #define LORA_FIX_LENGTH_PAYLOAD_ON                  false
  #define LORA_IQ_INVERSION_ON                        false
/*********** LORA CONFIG PARAMETERS ***********/  

/*********** DEVICE CONFIG PARAMETERS ***********/  
  #define NODE_ID 2
  #define GROUP_ID 0
  #define FRAME_SIZE 5
  #define SYNC_WORD 0X12
/*********** DEVICE CONFIG PARAMETERS ***********/  

byte FRAME_DATA[FRAME_SIZE];
static int DEVICE_STATUS = 0X0;
byte PAYLOAD[6];

static RadioEvents_t RadioEvents;
char voltage;

void setup(){
  
    boardInitMcu();
    Serial.begin(115200);

    Radio.Init( &RadioEvents );
    Radio.SetChannel( RF_FREQUENCY );
    Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                   LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                   LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   true, 0, 0, LORA_IQ_INVERSION_ON, 3000 ); 
                                   
   Radio.SetSyncWord(SYNC_WORD);
}

void loop(){
  sendFrame();
  delay(1000);
}

void sendFrame(){
 
  //Set the frame HEADER. The header size is fixed
  FRAME_DATA[0] = (byte)GROUP_ID;      //Device ID:     0 - 0xFF
  FRAME_DATA[1] = (byte)NODE_ID;       //Frame ID :     0 - 0xFF
  FRAME_DATA[2] = (byte)DEVICE_STATUS; //Device status: 0 - 0xFF
  FRAME_DATA[3] = (byte)1; //Payload Length 0 - 0xFF

  //Set the payload
  FRAME_DATA[4] = (byte)60;

  //Send the frame over LORA 
  Radio.Send(FRAME_DATA, FRAME_SIZEGROUP_ID); //send the package out  

}

//Put some data in our payload array
void setPayload(){
  
  //Set the payload data 
  for(int i=4; i <= FRAME_SIZE-1; i++){
     FRAME_DATA[i] = i; 
  }

}

byte getPress(){
  byte pressure = 20;
  return pressure;
}

void  DoubleToString( char *str, double double_num,unsigned int len); 
void  DoubleToString( char *str, double double_num,unsigned int len) { 
  double fractpart, intpart;
  fractpart = modf(double_num, &intpart);
  fractpart = fractpart * (pow(10,len));
  sprintf(str + strlen(str),"%d", (int)(intpart)); //Integer part
  sprintf(str + strlen(str), ".%d", (int)(fractpart)); //Decimal part
}
/*  txNumber += 0.01;
  sprintf(txpacket,"%s","Hello world number");  //start a package
//  sprintf(txpacket+strlen(txpacket),"%d",txNumber); //add to the end of package
  DoubleToString(txpacket,txNumber,3);     //add to the end of package
  turnOnRGB(COLOR_SEND,0); //change rgb color
  Serial.printf("\r\nsending packet \"%s\" , length %d\r\n",txpacket, strlen(txpacket));
*/
