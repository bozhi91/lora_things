  //msg.temp0 = msg.payload[6];


parseFrame(msg);

function parseFrame(msg){

    //   0           1            2       3        4             5         6
    //GATEWAY_ID GATEWAY_STAT GROUP_ID NODE_ID NODE_STATUS  PAYLOAD_LEN  PAYLOAD 
    var GROUP_ID = msg.payload[2];
  
    switch(GROUP_ID){
        case 0:
            sensorType_0(msg);
        break;
        
        case 1:
            sensorType_1(msg);
        break;
        
        default:
            badRequest(msg);
    }
}

/** Sensor type 0: (DH11)Temp and Humidity sensor */
function sensorType_0(msg){
 
    var DEVICE_ID = msg.payload[3];
    var STAT      = msg.payload[4];
    
    msg.temp0 = msg.payload[6];
    
    /*
    switch(DEVICE_ID){
        case 0:
        case 1:
        case 2:
              msg.temp0 = msg.payload[6];
        break;
    }*/
  
    
    //Validate data...
    if(msg.temp0 >= 50){
        alarm(DEVICE_ID);
    }    

    //msg.sensor2 = msg.payload[6];
}

/** Send notification to our Telegram bot */
function alarm(DEVICE_ID){

    msg.chat_id = 227254225;
    msg.text    = "Warning! Temperature at node ["+DEVICE_ID+"] is exceeding the safe levels: "+msg.temp0+" ºC";
}

    
function badRequest(msg){
    //TODO: process the bad request
}

return {msg: msg, metadata: metadata, msgType: msgType};

