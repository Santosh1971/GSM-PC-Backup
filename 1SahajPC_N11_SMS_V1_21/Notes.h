* **********************   Functions   ****************************************
 /* * ********  SMS based ****************** 
 *  
 *  control PUMP using Master Mobile no.
 *  
 *     ACTION                             SMS TO SEND                         REPLY
 * Start the Pump(using user mobile)            1                         PUMP ON 
 * Start the Pump for 30 min                    1*30*                     PUMP ON for 30 min
 * Start the Pump for 5 min,OFF 20min, 10 times 1*5*20*10*                PUMP ON for 5 min,OFF for 20 min , 10 times
 * Stop the Pump(using user mobile)             2                         PUMP OFF 
 * Status of Pump                               3                         PUMP ON since last 30 min/OFF
 * get sms command list                         * 
 * control PUMP using Any Other Mobile no.
 *  
 * Start the Pump                        81972392061(master mobile)1      PUMP ON
 * Start the Pump for 30 min             81972392061(master mobile)1*30*  PUMP ON
 * Stop the Pump                         81972392062(master mobile)2      PUMP OFF 
 * Status of Pump                        81972392063(master mobile)3      PUMP ON since last 30 min/OFF  
 * Register the mobile                   *100*8197239206(master mobile) 8197239206 will be registered as Master ,no need to restart                     
 * 
 * Add Master Mobile no
 * 
 * 1. Switch Off the System
 * 2. Send SMS "Master" to system's Mobile no. from User mobile no.
 * 3. After sending SMS,within 1 min switch ON the system
 * 4. user will get registered as Master and will receive a Text message " You are now Master "
 * 5. For confirmation send SMS 1 or 2 or 3 to control PUMP
 * 6. call System Mobile no to ON/OFF the PUMP.

/*


/*
  // Send SMS from SIM to get the SIM no.
 sendATCommand("AT+CMGF=1", 1000, "OK");       // Set the text modeAT
 sendATCommand("AT+CSCS=\"UCS2\"", 1000, "OK");       // Configure the TE character set
 sendATCommand("AT+CMGS=\"+918197239206\"", 1000, ">");       // Configure the TE character set
// wait for ">"
//sendATCommand("\"Hello from Sahaj1\"", 1000, "OK");
SerialAT.println("Hello from Sahaj1");
SerialAT.write(26);
Serial.println("Message Sent! Check you mobile for SIM no. ");
// SIM mobile no : +915754100153590

  while(1){};  //Stay here

  ConnectGSMandMQTT();

  SendSysInfoACK() ;

  */







To DO for GSM -RPO
1. change rmac through mqtt
2. Client ID of AT+MQTTCONNPARAM - rmac


// Updated on 10th Nov2019
// checked Fuse settings
// using USBASP AND AvrDudess 2.11
// Use 10K/2K2 between CS and 3.3V of LoRa to deselect it during programing through USBASP
//http://www.engbedded.com/fusecalc?P=ATmega328P&O_HEX=Apply+values
// http://www.martyncurrey.com/arduino-atmega-328p-fuse-settings/
// good settings
// LOW - 0xFF
// HIGH - 0xDA
// Ext - 0xFD  -(FD = 101 - 2.7V ) -   BODLevel2,BODLevel1,BODLevel0 ( 111 - BOD Disable ) ,( 110 - 1.8V ) ,( 101 - 2.7V ),( 100 - 4.3V )

// PROBLEMATIC PROMINI
// LOW - 0xFF
// HIGH - 0xDA
// Ext - 0xFF   ( 111 - BOD Disable )

// Watch dog Timer
//  https://www.youtube.com/watch?v=BDsu8YhYn8g

// Default ATMega328P fuse setting
// 62,D9,FF
// changed to 
// E2,D9,FF

//62,DA,FD  // int 8Mz osc

// Rakesh internal Osc Hardware file setting
//E2,DA,05

//http://www.gammon.com.au/forum/?id=11637

//https://www.engbedded.com/fusecalc/



















[2023-02-11_08:55:47]
[2023-02-11_08:55:47] 
[2023-02-11_08:55:47]...  Starting  ...
[2023-02-11_08:55:47] # File name: MQTT_N11_Base.ino #  Date: Feb 10 2023 #  Time: 22:45:41
[2023-02-11_08:55:47]Config taken from EEPROM.
[2023-02-11_08:55:47]PumpState :0
[2023-02-11_08:55:47]Pump_Status_on_Power_On :0
[2023-02-11_08:55:47]PumpOnTimer_min :0
[2023-02-11_08:55:47]PumpOnTimer_elapsed_min :1
[2023-02-11_08:55:53]GSM_Modem Powering......Pl. Wait
[2023-02-11_08:56:07]GSM_Modem Powered UP!!!
[2023-02-11_08:56:08]MODEM:STARTUP

[2023-02-11_08:56:08]+PBREADY
[2023-02-11_08:56:08]--- Modem Started Successfully!!! ---
[2023-02-11_08:56:08]Sent..ATE0   Rcvd as expected..OK
[2023-02-11_08:56:09]Sent..AT+IPR?   Rcvd as expected..+IPR: 9600
[2023-02-11_08:56:10]OK
[2023-02-11_08:56:11]Baud Rate: 9600
[2023-02-11_08:56:11]Sent..AT   Rcvd as expected..OK
[2023-02-11_08:56:12]Sent..AT   Rcvd as expected..OK
[2023-02-11_08:56:14]Sent..AT+CGSN   Rcvd as expected..+CGSN: 868457051735879
[2023-02-11_08:56:15]OK
[2023-02-11_08:56:15]Sent..AT+CPIN?   Rcvd as expected..+CPIN: READY
[2023-02-11_08:56:16]OK
[2023-02-11_08:56:17]Sent..AT+CSQ?   Response not matching..ERROR
[2023-02-11_08:56:18]Sent..AT+CREG?   Rcvd as expected..+CREG: 0,5
[2023-02-11_08:56:19]OK
[2023-02-11_08:56:19]Sent..AT+CGATT?   Rcvd as expected..+CGATT: 1
[2023-02-11_08:56:20]OK
[2023-02-11_08:56:21]Sent..AT+XIIC=1   Rcvd as expected..OK
[2023-02-11_08:56:23]Sent..AT+XIIC?   Rcvd as expected..+XIIC:    1,10.245.3.118
[2023-02-11_08:56:24]OK
[2023-02-11_08:56:25]Sent..AT+MQTTCONNPARAM="imei1"," "," "   Rcvd as expected..OK
[2023-02-11_08:56:26]Sent..AT+MQTTWILLPARAM=0,1,"Willtopic","byby"   Rcvd as expected..OK
[2023-02-11_08:56:28]Sent..AT+MQTTCONN="prodbroker.sasyasystems.com:1883",1,60   Rcvd as expected..OK
[2023-02-11_08:56:32]Sent..AT+MQTTSUB="mqtt_rx/GSM-RPO/14",1   Rcvd as expected..OK
[2023-02-11_08:56:33]Pump 14 : OFF
[2023-02-11_08:56:34]Default Pump State, Timer,Irrig Cycle...OFF















Standard MQTT process:
AT //After the module starts, send “AT” to the module.
OK
AT+CPIN?
+CPIN: READY //The module can identify the SIM card.
OK
AT+CSQ
+CSQ: 31, 99 //The signal strength is good.
OK
AT+CREG?
+CEREG: 0,1 //The module is successfully registered with the network.
OK
AT+CGATT?
+CGATT: 1 //The module is successfully attached to the data service.
OK
AT+XIIC=1 //Activate the network connection.
OK
AT+XIIC?
+XIIC: 1,100.79.207.128 //Obtain the IP address.
OK
AT+MQTTCONNPARAM="owen","admin","public" //Set the user parameters.
AT+MQTTCONNPARAM="owen"," "," " //Set the user parameters. put space between quotes
OK
AT+MQTTWILLPARAM=0,1,"lixytopic","byby" //Set the will message according actual requirements.
OK
AT+MQTTCONN="121.43.166.63:1883",0,60 //Set up an MQTT connection.
//AT+MQTTCONN="prodbroker.sasyasystems.com:1883",1,60
OK
AT+MQTTSUB="lixytopic",1 //Subscribe to a topic. It is recommended to set QoS to 1.
OK
AT+MQTTPUB=0,1,"lixytopic","this is for testing!" //Publish a message.
OK
+MQTTSUB:4,"lixytopic",20,this is for testing! //Receive a message from lixytopic. 4 is the no of times the message have been sent under this Topic , 20 is the the message length



AT+MQTTSTATE? //Query the socket status.
+MQTTSTATE:1
OK
AT+MQTTDISCONN //Disconnect to the server.
OK



AT+UPDATETIME=1,128.138.141.172,10,"0",0
AT+CCLK?
*/




/*

PumpID = last 4 digits of imei ( by default) //65535   (int value  -32,768 to 32,767)
PumpId range:- from 1 to 30,000 except  9999 , this is for Broadcasting ID , all pump Node will respond.

//rmac = imei
//mac = 300000000100
New Logic , Mqtt based
1. Initialise mqtt
//2. Subscribe TOPIC: mqtt_rx/PC/300000000100    //by default  it is rmac ( at start up )
2. Subscribe TOPIC: mqtt_rx/PC/PumpID    //by default  it is last 4 digits of imei

3. Publish TOPIC: mqtt_tx/PC/300000000100      //by default  it is rmac
 Payload 
 1 : // Sent at Start  up  or in response of  {"payloadId" :"SystemInfo"} 
 {
  "payloadId" : "PNSTS",
  "Ver" : "GSM_PC_N11_Blackcat_V1_0",
  "Rly" : 2,  // 1 or 2 by default it is 2 Relay
  "Date" : "1970-01-01T00:00:05.000Z",
  "mac" : "300000000100",          // by default  it is rmac
  "rmac" : <"imei">
}
2: in reponse of  ( PC ON/OFF)
  { "payloadId" : "PNCMD", "PCmd" :[1,0,0]}   //"PCmd":[PumpCommand,Reset_PN,ResumePumpStatus]
  Reply from Devise
  {
  "payloadId":"PNSTS",
  "PSts":[1,0,0] ,  //"PSts":[PumpStatus,Reset_PN,ResumePumpStatus]
  "Date" : "1970-01-01T00:00:05.000Z",
  "mac" : "300000000100",          // by default  it is rmac
  "rmac" : <"imei">
  }
3: in reponse of  ( Timer)
  { "payloadId" : "PNCMD","Tmr":[20]}   //"Tmr":[Timer_Time]  in min
  Reply from Devise
  {
  "payloadId":"PNSTS",
  "PSts":[1,0,0] ,  //"PSts":[PumpStatus,Reset_PN,ResumePumpStatus]
  "Tmr":[20,0]     // "Tmr":[Timer_Time,Timer_TimeElapsed]  
  "Date" : "1970-01-01T00:00:05.000Z",
  "mac" : "300000000100",          // by default  it is rmac
  "rmac" : <"imei">
  }

4: in reponse of  ( Irrigation Cycle)
  { "payloadId" : "PNCMD","IrrgCyc":[3,4,5]}   //"IrrgCyc":[Irrig_ONTime,Irrig_OFFTime,Irrig_Cycle]  in min
  Reply from Devise
  {
  "payloadId":"PNSTS",
  "PSts":[1,0,0] ,  //"PSts":[PumpStatus,Reset_PN,ResumePumpStatus]
  "IrrgCyc":[3,4,5,0]  //"IrrgCyc":[Irrig_ONTime,Irrig_OFFTime,Irrig_Cycle,Irrig_CycleElapsed] 
  "Date" : "1970-01-01T00:00:05.000Z",
  "mac" : "300000000100",          // by default  it is rmac
  "rmac" : <"imei">
  }
5: in reponse of  ( Change mac ID)
  {
  "payloadId" : "PNCMD",
  "nmac" : 300000000100
  }   
  System restarts.
  Reply from Devise ( same as Systeninfo message)
  {
    "payloadId" : "PNSTS",
    "Ver" : "GSM_PC_N11_Blackcat_V1_0",
    "Rly" : 2,  // 1 or 2 by default it is 2 Relay
    "Date" : "1970-01-01T00:00:05.000Z",
    "mac" : "300000000100",          
    "rmac" : <"imei">
  }
6: in reponse of  ( Change no of Relay)
  {
  "payloadId" : "PNCMD",
  "Rly" : 1        // 1 or 2
  }   
  System restarts.
  Reply from Devise ( same as Systeninfo message)
  {
    "payloadId" : "PNSTS",
    "Ver" : "GSM_PC_N11_Blackcat_V1_0",
    "Rly" : 2,  // 1 or 2 by default it is 2 Relay
    "Date" : "1970-01-01T00:00:05.000Z",
    "mac" : "300000000100",          
    "rmac" : <"imei">
  }
Mobile app using App inventor , mqtt
*/






