
// Include this library to transmit with sx1272
#include <WaspSX1272.h>

// define the destination address to send packets
uint8_t rx_address = 0x12;

// status variable
int8_t e;

void setup(){
   
  //Init USB port
  USB.ON();
  // Init sx1272 module
  sx1272.ON();

  // Select frequency channel
  e = sx1272.setChannel(CH_17_868); //CH17 = 868.00 MHz
  USB.print(F("Setting Channel CH_16_868.\t state "));  //CH10_868 = 865.20 MHz
  USB.println(e);

  // Select implicit (off) or explicit (on) header mode
  e = sx1272.setHeaderON();
  USB.print(F("Setting Header ON.\t\t state "));  
  USB.println(e); 

  // Select mode: from 1 to 10
  e = sx1272.setMode(1);  
  USB.print(F("Setting Mode '1'.\t\t state "));
  USB.println(e);  

  // Select CRC on or off
  e = sx1272.setCRC_ON();
  USB.print(F("Setting CRC ON.\t\t\t state "));
  USB.println(e);  

  // Select output power (Max, High or Low)
  e = sx1272.setPower('H');
  USB.print(F("Setting Power to 'L'.\t\t state "));  
  USB.println(e); 

  // Select the node address value: from 2 to 255
  e = sx1272.setNodeAddress(0x12);
  
  sx1272.setSF(SF_12);
  sx1272.setBW(BW_125); //125KHz
  sx1272.setCR(CR_5);// CR= 4/5
  sx1272.setPreambleLength(8);

  //Read the sync word
   USB.println(sx1272.readRegister(0x39),HEX);


  USB.print(F("Setting Node Address to '2'.\t state "));
  USB.println(e);
  USB.println();
  
  delay(1000);  
  
  USB.println(F("----------------------------------------"));
  USB.println(F("Sending:")); 
  USB.println(F("----------------------------------------"));
}


void loop(){
  // Sending packet before ending a timeout
  e = sx1272.sendPacketTimeout(BROADCAST_0, "0123456789");

  // Check sending status
  if( e == 0 ) {
    USB.println(F("Packet sent OK"));     
  }
 
  else{
    USB.println(F("Error sending the packet"));  
    USB.print(F("state: "));
    USB.println(e, DEC);
  } 

  delay(2500); 
}


void setSyncWord(){
  
    byte st0 = sx1272.readRegister(REG_OP_MODE);

    sx1272.writeRegister(REG_OP_MODE, FSK_STANDBY_MODE);
   
    // set sync word
    sx1272.writeRegister(0x27, 1);
    sx1272.writeRegister(0x39, 0x12);
 
    // Getting back to previous status
    sx1272.writeRegister(REG_OP_MODE, st0);
}



