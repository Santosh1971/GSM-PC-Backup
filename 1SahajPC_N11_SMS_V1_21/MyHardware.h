
/////////////// BOARD Blackcat Hardware //////////////////
//------------  To Set -------------------------


#define EEPROM_INIT 48   //( change the value if you want to restore the Default as below
#define PUMP_STATUS_ON_POWER_ON 1   // 0 or 1 //SMS  *2*1   - Resume Pump status ,Timer at Restart
#define PUMP_STATUS_SMS_EVERY_5MIN 1    // 0 or 1 // SMS *3*0   - Stop sending Pump Status SMS every 5 min from Devise 
#define SASYA_MASTER_MOBILE_NO "+918197239206"    //Universal Master


#define SEND_SMS 1 //0  // No SMS sent , used during developemnt , make it 1 for production
String COUNTRY_CODE = "+91";// India country code received phone no along with country code in SMS

//-----------------------------------------------

#define DEBUG 1 

//#include <avr/wdt.h>
#include "EEPROM_Functions.h"
//#include <ArduinoJson.h>      // https://github.com/bblanchon/ArduinoJson
#include "MySerial.h"


#define GSM_ON_OFF_Pin 11 //5
#define GSM_POWER_SHUT_DOWN_Pin 10 //6
#define RLY_NC_Pin A3  //A0 //8    //Relay1_Pin 9    //  NC  
#define RLY_NO_Pin A2  //A1 // 9    //Relay2_Pin 8    //  NO
#define LED1_Pump 9  //A2  //Pump LED YELLOW  - 13 for testing
#define LED2_GSM 8 //A3  //GSM LED  BLUE


///////////////////////////////////////////////////////////////////////

//-------  EEPROM Address for Variables-----------------

int init_EEADR = 0;  // 1 byte
int PumpState_EEADR = 1;
int Pump_Status_on_Power_On_EEADR = 3;
//int PumpID_EEADR = 5;
int PumpOnTimer_min_EEADR = 7;
int PumpOnTimer_elapsed_min_EEADR= 9;

int PumpONTime_EEADR= 11;
int PumpOFFTime_EEADR= 13;
int PumpONOFFCount_EEADR= 15;
int PumpONTime_elapsed_EEADR= 17;
int PumpOFFTime_elapsed_EEADR= 19;
int PumpONOFFCount_elapsed_EEADR= 21;

int Pump_is_ON_withTimer_EEADR= 23;
int Pump_is_ON_withIrrigationCycle_EEADR= 25;

int One_Min_Counter_EEADR= 27;
int Master_phone_EEADR= 29;  // up to 49  20 digits #define EEPROM_MASTER_MOBILE_NO 29;  // 20 digits
//int TBD_EEADR= 50;
int firstPumpCommandIndex_EEADR = 50;
int Timer_Mode_EEADR =52;
//int Resume_Pump_Timer_onStartup_EEADR;
int SendSMSin5Min_EEADR = 54;



// ----------   Global Variables ---------------------

bool PumpState;
bool Pump_Status_on_Power_On;

unsigned int PumpOnTimer_min;
unsigned int PumpOnTimer_elapsed_min;

unsigned int PumpONTime;
unsigned int PumpOFFTime;
unsigned int PumpONOFFCount;

unsigned int PumpONTime_elapsed;
unsigned int PumpOFFTime_elapsed;
unsigned int PumpONOFFCount_elapsed;


int Timer_Mode = 0;  // uint8_t 
bool Pump_is_ON_withTimer;
bool Pump_is_ON_withIrrigationCycle;

//int PumpONOFFCount_backup=0;
int firstPumpCommandIndex;

bool startup_flag;  // to send "System Stsrted SMS" at start up
//bool Resume_Pump_Timer_onStartup;
bool SendSMSin5Min;

//----------------- Counters and timers -----------------------
unsigned long startTime = millis(); // for GSM LED
unsigned long stopTime = millis();
bool GSM_LED_ON =0;  


unsigned long currentMillis_timer;
unsigned long previousMillis_timer;


unsigned long currentMillis_ONTimer;
unsigned long previousMillis_ONtimer;
//unsigned const long one_min_interval =   10000;  // 1 sec for testing   60000;   //  1 min   300000;   //5*60*1000; // 5 min
unsigned const long one_min_interval = 30000; //7500;  //30000; //60000;   //  1 min   300000;   //5*60*1000; // 5 min
unsigned int One_Min_Counter =0;

unsigned long currentMillis_wdt;
unsigned long previousMillis_wdt;
unsigned const long wdt_interval = 1500;

extern String stringFilename;   //=__FILE__;       // const char compile_date[] = __DATE__ " " __TIME__;



const int BUFSIZE = 20;
char buf[BUFSIZE];  // for mobile no.

String Master_phone = "+918197239206";

bool ToReset=0;

//char Master_phone[BUFSIZE];  // = MASTER_MOBILE_NO;

String senderPhoneNumber , SMSContent;


//String message = "+CMT: \"+918197239206\",,\"2023/05/21,22:00:55+06\" \n1*10*20*10"; //((char *)0); // Serial dtata received from GSM Modem
/*- SMS received
+CMT: "+918197239206",,"2023/05/21,22:00:55+06"
2
*/







///////////       -----------   Functions --------------------------

void(* resetFunc) (void) = 0;//declare reset function at address 0

void init_LED_and_Relay()  // Intialise ALL IO
{
  pinMode(GSM_ON_OFF_Pin, OUTPUT);   
  pinMode(GSM_POWER_SHUT_DOWN_Pin, OUTPUT);      
  pinMode(RLY_NC_Pin, OUTPUT);
  pinMode(RLY_NO_Pin, OUTPUT);
  pinMode(LED1_Pump, OUTPUT);
  pinMode(LED2_GSM, OUTPUT);
}

void Switch_ON_ALL_LED()    // Switch ON ALL LED 
{
  digitalWrite(LED1_Pump,HIGH);
  digitalWrite(LED2_GSM,HIGH);
}

void Switch_OFF_ALL_LED()    // Switch OFF ALL LED 
{
  digitalWrite(LED1_Pump,LOW);
  digitalWrite(LED2_GSM,LOW);
}

void  GSM_Modem_Power_Down()  // Power down of GSM Modem
{
  digitalWrite(GSM_POWER_SHUT_DOWN_Pin,HIGH);  // Power OFF through ULN
  digitalWrite(GSM_ON_OFF_Pin,LOW);  // Release Reset
} 

void  GSM_Modem_Power_UP()  // Power UP GSM Modem
{
  Serial.println(F("GSM_Modem Powering......Pl. Wait"));
  digitalWrite(GSM_POWER_SHUT_DOWN_Pin,LOW); // Power ON through ULN
  //***********
 // digitalWrite(13,HIGH); // for testing
 //digitalWrite(LED1_Pump,HIGH); // for testing 
  
  delay(6000); // delay to stabilize 4.2V
 // digitalWrite(13,HIGH); // for testing

 
  digitalWrite(GSM_ON_OFF_Pin,HIGH);  // Reset through  ULN
  //   Serial.println(F("GSM_ON_OFF   ...HIGH "));
  delay(2000);  // delay(6000);
  digitalWrite(GSM_ON_OFF_Pin,LOW);  // Release Reset
  //***********
  //digitalWrite(LED2_GSM,HIGH);  // for testing
 // delay(2000);  // mini 3 Sec
  Serial.println(F("GSM_Modem Powered UP!!!"));
   
 // digitalWrite(13,LOW); // for testing
 //*********** 
 // digitalWrite(13,HIGH); // for testing
  //digitalWrite(LED1_Pump,LOW); // for testing 
  //digitalWrite(LED2_GSM,LOW);  // for testing
}



void LED_Self_Test()
{
  Serial.println(F("LED_Self_Test!"));
  Switch_ON_ALL_LED();
 //  wdt_reset();     // Reset Watchdog Timer Regularly
  delay(1000);
 // wdt_reset();     // Reset Watchdog Timer Regularly
  delay(1000);
 //  wdt_reset();     // Reset Watchdog Timer Regularly
  Switch_OFF_ALL_LED();
}



void BlinkGSMLED(unsigned int interval)
{
  if ((millis() - startTime) >= interval)   // Every 1Sec check if mqtt is connected , switch ON the LED
  { 
    /*
    int IsMQTTConnected;
    if(MQTT_MESSAGE_RECEIVED){IsMQTTConnected=1;}
    else{ IsMQTTConnected = MqttConnected();}
    if(IsMQTTConnected)
    {
      digitalWrite(LED2_GSM,HIGH);  //digitalWrite(LED2_GSM,!digitalRead(LED2_GSM));
      GSM_LED_ON=1;  
    }*/

    digitalWrite(LED2_GSM,HIGH);  //digitalWrite(LED2_GSM,!digitalRead(LED2_GSM));
    GSM_LED_ON=1;  
    startTime = millis();  // RESET timer
    stopTime = millis();   // init Stop LED timer
    
  }
  if (GSM_LED_ON)
  {
    if ((millis() - stopTime) >= 200)   // Stop the LED glow
    {
      digitalWrite(LED2_GSM,LOW);  
      GSM_LED_ON=0;      
    }
  }
}

void Load_EEPROM_or_DEFAULT()
{
  if( EEPROM_INIT ==  EEPROM.read(init_EEADR))
  {
    #ifdef DEBUG
    Serial.println(F("Config taken from EEPROM.")); 
    #endif          
    PumpState = EEPROM.read(PumpState_EEADR);
    Pump_Status_on_Power_On = EEPROM.read(Pump_Status_on_Power_On_EEADR);  
    PumpOnTimer_min = EEPROMReadInt(PumpOnTimer_min_EEADR);
    PumpOnTimer_elapsed_min= EEPROMReadInt(PumpOnTimer_elapsed_min_EEADR);    
    PumpONTime = EEPROMReadInt(PumpONTime_EEADR);
    PumpOFFTime = EEPROMReadInt(PumpOFFTime_EEADR);
    PumpONOFFCount = EEPROMReadInt(PumpONOFFCount_EEADR);

    PumpONTime_elapsed = EEPROMReadInt(PumpONTime_elapsed_EEADR);
    PumpOFFTime_elapsed = EEPROMReadInt(PumpOFFTime_elapsed_EEADR);
    PumpONOFFCount_elapsed = EEPROMReadInt(PumpONOFFCount_elapsed_EEADR);
    One_Min_Counter=EEPROMReadInt(One_Min_Counter_EEADR);

    Pump_is_ON_withTimer = EEPROM.read(Pump_is_ON_withTimer_EEADR);
    Pump_is_ON_withIrrigationCycle = EEPROM.read(Pump_is_ON_withIrrigationCycle_EEADR);
    if(Pump_is_ON_withTimer){ Serial.print(F("Timer ON : PumpOnTimer_elapsed_min : ")); Serial.println(PumpOnTimer_elapsed_min);}
    if(Pump_is_ON_withIrrigationCycle){ Serial.print(F("Irrigation Cycle ON , PumpONOFFCount_elapsed :")); Serial.println(PumpONOFFCount_elapsed);}
   // Resume_Pump_Timer_onStartup =  EEPROM.read(Resume_Pump_Timer_onStartup_EEADR); 
    SendSMSin5Min =  EEPROM.read(SendSMSin5Min_EEADR); 

    eeprom_read_string(Master_phone_EEADR, buf, BUFSIZE);   // 
    Master_phone = buf;
  //  Master_phone = COUNTRY_CODE + Master_phone;
    Serial.print(F("Master : ")); Serial.println(Master_phone);
 
  }
  else
  {
    EEPROM.write(init_EEADR ,EEPROM_INIT); 
    #ifdef DEBUG
    Serial.println(F("Config taken by default..")); 
    #endif 
    Pump_Status_on_Power_On = PUMP_STATUS_ON_POWER_ON;  
    PumpOnTimer_min = 0; 
    PumpOnTimer_elapsed_min= 0;      
    PumpONTime = 0;
    PumpOFFTime = 0;
    PumpONOFFCount = 0; 
    PumpONTime_elapsed = 0;
    PumpOFFTime_elapsed = 0;
    PumpONOFFCount_elapsed = 0;                    
    Pump_is_ON_withTimer = 0;
    Pump_is_ON_withIrrigationCycle = 0; 
    One_Min_Counter=0;  
    
   // Resume_Pump_Timer_onStartup = PUMP_STATUS_ON_POWER_ON;
    SendSMSin5Min = PUMP_STATUS_SMS_EVERY_5MIN;
    Master_phone = SASYA_MASTER_MOBILE_NO;

    EEPROM.write(Pump_Status_on_Power_On_EEADR,Pump_Status_on_Power_On); 
      
    EEPROMWriteInt(PumpOnTimer_min_EEADR,PumpOnTimer_min);  
    EEPROMWriteInt(PumpOnTimer_elapsed_min_EEADR,PumpOnTimer_elapsed_min); 
      
    EEPROMWriteInt(PumpONTime_EEADR,PumpONTime); 
    EEPROMWriteInt(PumpOFFTime_EEADR,PumpOFFTime);  
    EEPROMWriteInt(PumpONOFFCount_EEADR,PumpONOFFCount);

    EEPROMWriteInt(PumpONTime_elapsed_EEADR,PumpONTime); 
    EEPROMWriteInt(PumpOFFTime_elapsed_EEADR,PumpOFFTime);  
    EEPROMWriteInt(PumpONOFFCount_elapsed_EEADR,PumpONOFFCount);  
    EEPROMWriteInt(One_Min_Counter_EEADR,One_Min_Counter);

    
    EEPROM.write(Pump_is_ON_withTimer_EEADR,Pump_is_ON_withTimer);  
    EEPROM.write(Pump_is_ON_withIrrigationCycle_EEADR,Pump_is_ON_withIrrigationCycle); 

   // EEPROM.write(Resume_Pump_Timer_onStartup_EEADR,Resume_Pump_Timer_onStartup); 
    EEPROM.write(SendSMSin5Min_EEADR,SendSMSin5Min); 
    
    
    //eeprom_write_string(Master_phone_EEADR,Master_phone);
    //boolean eeprom_write_string(int addr, const char* string)
    Serial.print(F("Master save : ")); Serial.println(Master_phone);     
    Master_phone.toCharArray(buf, BUFSIZE); //convert string to char array
    eeprom_write_string(Master_phone_EEADR, buf); 
          

  }

  #ifdef DEBUG
  Serial.print(F("PumpState :"));Serial.println(PumpState);
  Serial.print(F("Pump_Status_on_Power_On :"));Serial.println(Pump_Status_on_Power_On);
  Serial.print(F("PumpOnTimer_min :"));Serial.println(PumpOnTimer_min);
  Serial.print(F("PumpOnTimer_elapsed_min :"));Serial.println(PumpOnTimer_elapsed_min);
  #endif 

}


void Pump_and_LED_Action(bool PumpState)  // to Connect this START and STOP button of Starter
{
  digitalWrite(LED1_Pump, PumpState);
  EEPROM.write(PumpState_EEADR,PumpState);
  Serial.print(F("Pump "));Serial.print(F(" : "));
  Serial.println(PumpState ? "ON" : "OFF");
  //  wdt_reset();     // Reset Watchdog Timer Regularly 
  #ifndef NO_PUMP_WHEN_USB_USED
  if(PumpState)
  {  
    digitalWrite(RLY_NC_Pin, HIGH);
    digitalWrite(RLY_NO_Pin, LOW);
    delay(500);     
    digitalWrite(RLY_NC_Pin, LOW);
  }
  else
  {
    digitalWrite(RLY_NC_Pin, LOW);
    digitalWrite(RLY_NO_Pin, HIGH);
    delay(500); 
    digitalWrite(RLY_NO_Pin, LOW);
  }
  #endif 
  //  wdt_reset();     // Reset Watchdog Timer Regularly 

}


void Pump_and_LED_Action_and_Reset_Timer_and_IrrigationCycle(bool PumpState)  // to Connect this START and STOP button of Starter
{
  //  EEPROM.write(PumpState_EEADR,PumpState);
    //Pump_Status_on_Power_On=0;
    PumpOnTimer_min=0;
    PumpOnTimer_elapsed_min= 0; 
    PumpONTime=0;
    PumpOFFTime=0;
    PumpONOFFCount=0;
    Pump_is_ON_withTimer=0;
    Pump_is_ON_withIrrigationCycle=0;

    //EEPROM.write(Pump_Status_on_Power_On_EEADR,Pump_Status_on_Power_On);
    EEPROMWriteInt(PumpOnTimer_min_EEADR,PumpOnTimer_min); 
    EEPROMWriteInt(PumpONTime_EEADR,PumpONTime);
    EEPROMWriteInt(PumpOFFTime_EEADR,PumpOFFTime);
    EEPROMWriteInt(PumpONOFFCount_EEADR,PumpONOFFCount);

    EEPROM.write(Pump_is_ON_withTimer_EEADR,Pump_is_ON_withTimer);
    EEPROM.write(Pump_is_ON_withIrrigationCycle_EEADR,Pump_is_ON_withIrrigationCycle);

    PumpONTime_elapsed = 0;
    PumpOFFTime_elapsed = 0;
    PumpONOFFCount_elapsed = 1;    // Start with Cycle 1 
    PumpOnTimer_elapsed_min= 0;  
    One_Min_Counter=0;
    EEPROMWriteInt(One_Min_Counter_EEADR, One_Min_Counter);
    EEPROMWriteInt(PumpOnTimer_elapsed_min_EEADR,PumpOnTimer_elapsed_min);
    EEPROMWriteInt(PumpONTime_elapsed_EEADR,PumpONTime_elapsed);
    EEPROMWriteInt(PumpOFFTime_elapsed_EEADR,PumpOFFTime_elapsed);
    EEPROMWriteInt(PumpONOFFCount_elapsed_EEADR,PumpONOFFCount_elapsed);    
    
    Pump_and_LED_Action(PumpState);
/*
    digitalWrite(LED1_Pump, PumpState);
    
    Serial.print(F("Pump "));Serial.print(F(" : "));
    Serial.println(PumpState ? "ON" : "OFF");
    
  //  wdt_reset();     // Reset Watchdog Timer Regularly 
    #ifndef NO_PUMP_WHEN_USB_USED
    if(PumpState)
    {  
      digitalWrite(RLY_NC_Pin, HIGH);
      digitalWrite(RLY_NO_Pin, LOW);
      delay(500);     
      digitalWrite(RLY_NC_Pin, LOW);
    }
    else
    {
      digitalWrite(RLY_NC_Pin, LOW);
      digitalWrite(RLY_NO_Pin, HIGH);
      delay(500); 
      digitalWrite(RLY_NO_Pin, LOW);
    }
    #endif 
    */
   // wdt_reset();     // Reset Watchdog Timer Regularly 
    // initialise all millis
    currentMillis_timer = millis();
    previousMillis_timer = currentMillis_timer; 
    
    currentMillis_ONTimer = millis();
    previousMillis_ONtimer = currentMillis_ONTimer;

}


const char* command = "AT"; // AT command to send
const unsigned int timeout = 2000; // timeout in milliseconds
const char* expectedResponse = "OK"; // expected response from modem

void CheckAndSetBaudRate()
{
  bool ATCommandStatus;
  ATCommandStatus= sendATCommand(command, timeout, expectedResponse); //  sendATCommand("AT", 2000, "OK");
  if (!ATCommandStatus) sendATCommand(command, timeout, expectedResponse);
  if (!ATCommandStatus) sendATCommand(command, timeout, expectedResponse);
  if (!ATCommandStatus) {Serial.println(F("No Communication with Modem...Rebooting!!")); delay(1000); resetFunc(); }
  // if communication with GSM modem is OK...
  sendATCommand("ATE0", 1000, "OK");       // ECHO OFF
 if(sendATCommand("AT+IPR?", 1000, "+IPR: 9600"))
 // if(sendATCommand("AT+IPR?", 1000, "+IPR: 4800")) doesnt work
  {
    Serial.println(F("Baud Rate: 9600"));
  }
  else
  {
    Serial.println(F("Baud Rate is not 9600. Now Set...")); 
    sendATCommand("AT+IPR=9600", 1000, "OK");
   // sendATCommand("AT+IPR=4800", 1000, "OK");
  }
  


}
// Pump Status SMS to send 
// On Power uP
// System Started , if Pump State is ON & NO Timer &No Irrigation "Pump was ON for 34 min before power cut"
//                 , if Pump State is OFF & NO Timer &No Irrigation "Pump if OFF.""
//                 , if  Timer ON  & No Irrigation "Pump was ON for 34 min , 24 min more to go!  
//                 , if  No Timer ON  &  Irrigation ON  " Irrigation in progress!! 20 min ON , 30 min OFF ,20 times
                                                       // Remaining times = 10 
// Normal running
//if Pump State is ON & NO Timer & No Irrigation "Pump is ON "
//if Pump State is OFF & NO Timer & No Irrigation "Pump is OFF "
// if  Timer ON  & No Irrigation "Pump is ON since 34 min , 24 min more to go!  
//if  No Timer ON  &  Irrigation ON  " Irrigation in progress!! 20 min ON , 30 min OFF ,20 times
//                                    Remaining times = 10 ""


// SIM mobile no : +915754100153590       13 digit!!

//char Topic[80];  // char Topic[130];  130 makes SMS wrong..
//int sendSMS(const char* number, const char* text)
//bool sendSMS(String& number, String& text) 
//void sendSMS(const String& number, const char* text)
//void sendSMS(const String& number, const String& text)  
void sendSMS(const String& text) 
{
  char Topic[80];  // char Topic[130];  130 makes SMS wrong..
 // ATCommandStatus= sendATCommand("AT+CMGF=1", 1000, "OK");       // Set the text mode for SMS
 // if(!ATCommandStatus){ATCommandStatus= sendATCommand("AT+CMGF=1", 1000, "OK");}
  
 // ATCommandStatus= sendATCommand(command, timeout, expectedResponse);

 // snprintf(Topic, sizeof(Topic), "AT+CMGS=\"%s\"", number.c_str());  // sendATCommand("AT+CMGS=\"+918197239206\"", 1000, ">");       //
  snprintf(Topic, sizeof(Topic), "AT+CMGS=\"%s\"", senderPhoneNumber.c_str());
 
 
 // snprintf (Topic+strlen(Topic), sizeof(Topic)-strlen(Topic),"AT+CMGS=\"%s\"", number.c_str());
 // snprintf (strlen(Topic), sizeof(Topic)-strlen(Topic),"AT+CMGS=\"%s\"", number.c_str());
  Serial.print("CMGS-> "); Serial.println(Topic);
 // strcpy(Topic,"AT+CMGS=\"+918197239206\""); // hardcoding mobile no , for testing
  ATCommandStatus= sendATCommand(Topic, 5000, ">");   // wait for ">"
  if(!ATCommandStatus){ATCommandStatus= sendATCommand(Topic, 5000, ">");}
  else
  {
   // delay(10);
    Serial.println(text);   //SerialAT.println("Hello from Sahaj1");
    SerialAT.println(text);   //SerialAT.println("Hello from Sahaj1");
    SerialAT.write(26);       // stream.write((char)0x1A);	
   // SerialAT.println((char)26); // ASCII code of CTRL+Z
    Serial.println(F("SMS Sent!"));
  }       
  if (!ATCommandStatus)
  {
    Serial.println(F("SMS Send Error!!!"));
  }     
}







/*
void sendSMS(const String& number, const String& text) 
//int sendSMS(const char* number, const char* text)
//bool sendSMS(String& number, String& text) 
{
  char Topic[80];

  ATCommandStatus= sendATCommand("AT+CMGF=1", 1000, "OK");       // Set the text modeAT
  if(!ATCommandStatus){ATCommandStatus= sendATCommand("AT+CMGF=1", 1000, "OK");}
  else
  { 
    ATCommandStatus=  sendATCommand("AT+CSCS=\"UCS2\"", 1000, "OK");    // Configure the TE character set
    if(!ATCommandStatus){ATCommandStatus=sendATCommand("AT+CSCS=\"UCS2\"", 1000, "OK");}
    else
    { 
      snprintf(Topic, sizeof(Topic), "AT+CMGS=\"%s\"", number.c_str());  // sendATCommand("AT+CMGS=\"+918197239206\"", 1000, ">");       //
          // Serial.print("CMGS packet:   "); Serial.println(Topic);
      ATCommandStatus= sendATCommand(Topic, 1000, ">");   // wait for ">"
      if(!ATCommandStatus){ATCommandStatus= sendATCommand(Topic, 1000, ">");}
      else
      {
        delay(10);
        SerialAT.println(text);   //SerialAT.println("Hello from Sahaj1");
       // SerialAT.write(26);       // stream.write((char)0x1A);	
        SerialAT.println((char)26); // ASCII code of CTRL+Z
        Serial.println(F("Message Sent! "));
      }       
    }  
  
  }
  if (!ATCommandStatus)
  {
    Serial.println(F("Message Send Error!!!! "));
  }     
}
    
*/

void deleteSMS() {
  sendATCommand("AT+CMGD=1,4", 1000, "OK");  // Delete all SMS messages
}

//snprintf provides similar functionality to sprintf, but it allows you to specify 
//the maximum number of characters to write to the destination buffer, which helps prevent buffer overflows.

void Send_PumpStatus()
{
  char SMS_to_Send[130]="";  
 
  if (startup_flag)
  {
    snprintf (SMS_to_Send, sizeof(SMS_to_Send), "%s", "System Started!\n");
  }
  
  if (Pump_is_ON_withTimer)
  {
    if (PumpState)
    {
      snprintf (SMS_to_Send+strlen(SMS_to_Send), sizeof(SMS_to_Send)-strlen(SMS_to_Send), "Pump ON for %u min. %u min more to go.", PumpOnTimer_elapsed_min, PumpOnTimer_min - PumpOnTimer_elapsed_min);
    }
    else
    {
      sprintf(SMS_to_Send, "Pump OFF. %u min. over.", PumpOnTimer_min);
    }
  }
  else if (Pump_is_ON_withIrrigationCycle)
  {

    snprintf (SMS_to_Send+strlen(SMS_to_Send), sizeof(SMS_to_Send)-strlen(SMS_to_Send),"Irrig ( %u min ON, %u min OFF)- %u Cycle; Balance Cycle %u", PumpONTime, PumpOFFTime, PumpONOFFCount, PumpONOFFCount - PumpONOFFCount_elapsed);

    if(PumpONOFFCount_elapsed >= PumpONOFFCount)
    {  // Finshed irrigation 
     // strcat(SMS_to_Send, "\n Irrigation Cycle Finished! Pump OFF.");
      snprintf (SMS_to_Send+strlen(SMS_to_Send), sizeof(SMS_to_Send)-strlen(SMS_to_Send),"\n Irrigation Cycle Finished! Pump OFF.");
    }
  }
  else
  {
    if (PumpState)
    {      
      snprintf (SMS_to_Send+strlen(SMS_to_Send), sizeof(SMS_to_Send)-strlen(SMS_to_Send),"Pump ON for %u min.", One_Min_Counter);
    }
    else
    {
      snprintf (SMS_to_Send+strlen(SMS_to_Send), sizeof(SMS_to_Send)-strlen(SMS_to_Send),"Pump OFF");     
    }
  }
  /*
  strcat(SMS_to_Send, "\n SMS * for command list!");
  */
  Serial.print(F("Send SMS : ")); Serial.println(SMS_to_Send);
 
  if (SEND_SMS)
  {
    //sendSMS(senderPhoneNumber, SMS_to_Send);  // Master_phone
    sendSMS(SMS_to_Send);  // Master_phone
  }

  startup_flag = 0;
}





void RunTimer()
{
  currentMillis_timer = millis();
  if (currentMillis_timer - previousMillis_timer >= one_min_interval) // every 60 sec
  {  
    // wdt_reset();     // Reset Watchdog Timer Regularly  
    previousMillis_timer =   currentMillis_timer; 
    PumpOnTimer_elapsed_min = PumpOnTimer_elapsed_min+1;
    EEPROMWriteInt(PumpOnTimer_elapsed_min_EEADR,PumpOnTimer_elapsed_min);
    Serial.print(F(" , "));  Serial.print(PumpOnTimer_elapsed_min);    
    if((PumpOnTimer_elapsed_min % 5)==0)  // send data every 5 min
    {                  
      if(SendSMSin5Min)
      {
        Send_PumpStatus();
      }      
    }
    if(PumpOnTimer_elapsed_min > PumpOnTimer_min  )  //PumpOnTimer_elapsed_min >= PumpOnTimer_min
    {
      Serial.println(F(" "));
    //  Serial.println(F("Time up! Stop Pump!"));
      PumpState=0; 
      Send_PumpStatus(); 
      Pump_and_LED_Action_and_Reset_Timer_and_IrrigationCycle( PumpState); // Stop Timer and reset timinings        
    }
  }
}

void RunIrrigationCycle()
{
  if(PumpState)              //----------   PUMP IS ON --------------------------
  {
    currentMillis_ONTimer = millis();
    if (currentMillis_ONTimer - previousMillis_ONtimer >= one_min_interval) // 1 min has elapsed
    {
      previousMillis_ONtimer = currentMillis_ONTimer;
      One_Min_Counter = One_Min_Counter + 1;
      EEPROMWriteInt(One_Min_Counter_EEADR, One_Min_Counter);
     // Serial.print(F("Pump ON Time elapsed :")); Serial.print(One_Min_Counter);Serial.println(F(" min"));
      Serial.print(F(" , "));  Serial.print(One_Min_Counter);
      // wdt_reset();     // Reset Watchdog Timer Regularly  
      if( One_Min_Counter >= PumpONTime)    //One_Min_Counter >= PumpONTime
      {
       // Serial.println(F("PumpON time Finish, Start PumpOFF time."));
        PumpState=0;
        Pump_and_LED_Action( PumpState ); 
        EEPROM.write(PumpState_EEADR,PumpState);   
        One_Min_Counter = 0;
        EEPROMWriteInt(One_Min_Counter_EEADR, One_Min_Counter); 
        //  Send_PumpStatus();
        // wdt_reset();     // Reset Watchdog Timer Regularly
        /*
        if(PumpONOFFCount_elapsed >= PumpONOFFCount)
        {
          Serial.println(F("IrriGation Cycle Finsihed"));
          //Pump_is_ON_withIrrigationCycle = 0;
         // EEPROM.write(Pump_is_ON_withIrrigationCycle_EEADR,Pump_is_ON_withIrrigationCycle);                     
          Send_PumpStatus();
          Pump_and_LED_Action_and_Reset_Timer_and_IrrigationCycle( PumpState); // Stop Timer and reset timinings        
                  //  wdt_reset();     // Reset Watchdog Timer Regularly  
        }  
        */                                           
      }
    }
  }
  else  //if(!PumpState)  // Relay is OFF
  {
    currentMillis_ONTimer = millis();
    if (currentMillis_ONTimer - previousMillis_ONtimer >= one_min_interval) // 1 min has elapsed
    {
      previousMillis_ONtimer = currentMillis_ONTimer;
      One_Min_Counter = One_Min_Counter + 1;
      EEPROMWriteInt(One_Min_Counter_EEADR, (One_Min_Counter));
     // Serial.print(F("Pump OFF Time elapsed :")); Serial.print(One_Min_Counter);Serial.println(F(" min"));
      Serial.print(F(" , "));  Serial.print(One_Min_Counter);
      // wdt_reset();     // Reset Watchdog Timer Regularly  
      if(One_Min_Counter >= PumpOFFTime  )    //&& (firstPumpCommandIndex >0)) 
      {
        One_Min_Counter = 0;
        EEPROMWriteInt(One_Min_Counter_EEADR, One_Min_Counter);
        PumpONOFFCount_elapsed = PumpONOFFCount_elapsed+1;
        EEPROMWriteInt(PumpONOFFCount_elapsed_EEADR, PumpONOFFCount_elapsed);
      //  Serial.print(F("Irrig Cycle elapsed :")); Serial.println(PumpONOFFCount_elapsed);
        // SendCommandACK();
        if(PumpONOFFCount_elapsed >= PumpONOFFCount)
        {
         // Serial.println(F("IrriGation Cycle Finsihed"));
          //Pump_is_ON_withIrrigationCycle = 0;
         // EEPROM.write(Pump_is_ON_withIrrigationCycle_EEADR,Pump_is_ON_withIrrigationCycle);                     
          Send_PumpStatus();
          Pump_and_LED_Action_and_Reset_Timer_and_IrrigationCycle( PumpState); // Stop Timer and reset timinings        
                  //  wdt_reset();     // Reset Watchdog Timer Regularly  
        } 
        else
        {
         // Serial.println(F("PumpOFF time Finish, Start PumpON time."));
          PumpState=1;
          Pump_and_LED_Action( PumpState );
          EEPROM.write(PumpState_EEADR,PumpState);
          Send_PumpStatus();
        }
        
        //  wdt_reset();     // Reset Watchdog Timer Regularly 
      }
    }
  }
} 





void Print_MasterNo()
{
  Serial.print(F("Master #: "));
  eeprom_read_string(Master_phone_EEADR, buf, BUFSIZE);   // 
  Master_phone = buf;  // Uncomment for prod
 // Master_phone = "+918197239206";        // only for testing , comment for prod
  Serial.println(Master_phone);
}


int check_Master_SMSatStartup(unsigned int timeout)   // to assign Mater No.
{
  String message=""; 
 // char myStringChar[BUFSIZE];
  unsigned long startTime = millis();
  while ((millis() - startTime) < timeout) 
  {
    BlinkGSMLED(250);  // 
    if (SerialAT.available()) 
    {
      message = SerialAT.readString();
      message.trim();
      Serial.print(F( "message :"));Serial.println(message);
      if (message.indexOf("+CMT:") != -1)
      {   
        // Find the start and end index of the sender's phone number
        int senderStartIndex = message.indexOf("\"")  +1;
        int senderEndIndex = message.indexOf("\"", senderStartIndex);
        //String senderPhoneNumber = message.substring(senderStartIndex, senderEndIndex);
        senderPhoneNumber = message.substring(senderStartIndex, senderEndIndex);
        Serial.print(F( "SenderPhNum : "));Serial.println(senderPhoneNumber);

        int messageStartIndex = message.indexOf("\n")  +1;
        int messageEndIndex = message.indexOf("\n", messageStartIndex);
        //String SMSContent = message.substring(messageStartIndex, messageEndIndex);
        SMSContent = message.substring(messageStartIndex, messageEndIndex);
        Serial.print(F( "SMSTxt : "));Serial.println(SMSContent);

        deleteSMS();  // delete SMS 

        // Check if the received message is to set the master no.
        if(( SMSContent == "Master") || ( SMSContent == "MASTER") || ( SMSContent == "master"))
        {
         // Serial.println(F("Saving Master PhNum"));
          /*
          senderPhoneNumber.toCharArray(myStringChar, BUFSIZE); //convert string to char array
          strcpy(buf, myStringChar);
          eeprom_write_string(Master_phone_EEADR, buf); 
          Master_phone=senderPhoneNumber;
          Serial.println(F("Send SMS : You are now Master"));
          */
          Master_phone=senderPhoneNumber;
          Serial.print(F("Master save : ")); Serial.println(Master_phone);     
          Master_phone.toCharArray(buf, BUFSIZE); //convert string to char array
          eeprom_write_string(Master_phone_EEADR, buf); 
         // sprintf(buffer, "You are now Master"); 
          Serial.println(F("Send SMS : You are now Master"));
          
          
          if (SEND_SMS)
          {
            //sendSMS(Master_phone, "You are now Master" );//res = modem.sendSMS(Master_phone, "You are now Master" );
            sendSMS("You are now Master" ); //Master_phone=senderPhoneNumber;
          }
          return 1;
        }
        else
        {
          return 0;   // "Master" not receivd
        } 
      }
      else
      {
       // Reset Modem or Check AT..
      }      
    }    
  }
  return -1;    // Time out
}


// --------------- SMS commands ---------------------------
// 1 - ON  or 2 - OFF or 3 - Pump Status
// 1*30  - Pump ON for 30 min  
// 1*5*20*10     - PUMP ON for 5 min,OFF for 20 min , 10 times
// * get sms command list         
//------------------------------------------------
// in next model
// ** get Advanced sms command list          SMS 'Master' from desired mobile number during fast blue LED blinking at startup.
// *2*1   - Resume Pump status ,Timer at Restart
// *2*0   - Pump ,Timer OFF at Restart
// *3*0   - Stop sending Pump Status SMS every 5 min from Devise    
// *3*1   - Start sending Pump Status SMS every 5 min from Devise   

// Note: 
//Command 1 or 2 will stop Timer/ irrigation cycle   
// *100*8197239206(master mobile) 8197239206 will be registered as Master ,no need to restart   Not for customer

// default:
// *2*1   - Resume Pump status ,Timer at Restart
// *3*0   - Stop sending Pump Status SMS every 5 min from Devise   
//String SMS2Send = "2 min ON, 3 min OFF, 03 times   \n Send SMS * to  devise to get the List of SMS command!";

//void Respond_SMS(const String& smsfrom, const String& SMS_Read)
void Respond_SMS_to_sender(const String& SMS_Read)
{
  //String SMS2Send;  // = "2 min ON, 3 min OFF, 03 times   \n Send SMS * to  devise to get the List of SMS command!";

 char buffer[150]; // Adjust the buffer size as per your requirement

  String myString=""; 
  int firstPumpCommandIndex;
  int secondPumpCommandIndex;
  int thirdPumpCommandIndex;


 

  // ----- IDENTIFY WHICH KIND OF SMS COMMAND IS THIS ---------

  firstPumpCommandIndex = SMS_Read.indexOf('*');  // Either Timer or Irrigation
  EEPROM.write(firstPumpCommandIndex_EEADR,firstPumpCommandIndex);  
  if(firstPumpCommandIndex > 0)  // found first * after 1    1*
  {
    secondPumpCommandIndex = SMS_Read.indexOf('*',firstPumpCommandIndex+1);
    if (secondPumpCommandIndex >0) // found 2nd * , so command is 1*5*
    {
      thirdPumpCommandIndex = SMS_Read.indexOf('*',secondPumpCommandIndex+1);
      if(thirdPumpCommandIndex > 0)  // found 3rd *  so comand is 1*5*20*10 //----------- command is irrigation cycle---------
      {
        Pump_is_ON_withIrrigationCycle = 1;               
        Pump_is_ON_withTimer = 0; 
        PumpState=1;
        Pump_and_LED_Action(PumpState);       
        EEPROM.write(Pump_is_ON_withIrrigationCycle_EEADR,Pump_is_ON_withIrrigationCycle);
        // Serial.println(F("Pump ON and Start Irrig Cycle !"));

        myString = SMS_Read.substring(firstPumpCommandIndex+1, secondPumpCommandIndex);
        myString.toCharArray(buffer, 6);
        PumpONTime = atoi(buffer);
        
        EEPROMWriteInt(PumpONTime_EEADR,PumpONTime);  
      //  Serial.print(F("PumpONTime "));Serial.println(PumpONTime);

        myString = SMS_Read.substring(secondPumpCommandIndex+1, thirdPumpCommandIndex);
        myString.toCharArray(buffer, 6);
        PumpOFFTime = atoi(buffer);
        EEPROMWriteInt(PumpOFFTime_EEADR,PumpOFFTime);
       // Serial.print(F("PumpOFFTime "));Serial.println(PumpOFFTime);

        myString = SMS_Read.substring(thirdPumpCommandIndex +1);
        myString.toCharArray(buffer, 6);
        PumpONOFFCount = atoi(buffer);
        EEPROMWriteInt(PumpONOFFCount_EEADR,PumpONOFFCount);    
       // Serial.print(F("PumpONOFFCount "));Serial.println(PumpONOFFCount);

        currentMillis_ONTimer = millis();
        previousMillis_ONtimer = currentMillis_ONTimer;
        PumpONOFFCount_elapsed=0;
        One_Min_Counter = 0;
        sprintf(buffer, "Irrigation Cycle Started!\n Pump ON!\n %u min ON, %u min OFF, For %u times",
              PumpONTime, PumpOFFTime, PumpONOFFCount);  // , PumpONOFFCount - PumpONOFFCount_elapsed

      }
      else
      {
        // Wrong command   1*5*20  . 
        Serial.println(F("Wrong command !!, SMS * for command List"));
      }
    } 
    else                        //-------- command is 1*20 - go for timer--------------
    {
      Pump_is_ON_withTimer = 1; 
      Pump_is_ON_withIrrigationCycle = 0; 
      PumpState=1;    
      Pump_and_LED_Action(PumpState);                  
      Serial.println(F(" Pump ON and Start Timer!"));  
      Pump_is_ON_withTimer =1;
      EEPROM.write(Pump_is_ON_withTimer_EEADR,Pump_is_ON_withTimer);
      myString = SMS_Read.substring(firstPumpCommandIndex+1);  // 1*8
      myString.toCharArray(buffer, 6);
      PumpOnTimer_min = atoi(buffer);
      Serial.print(F("PumpOnTimer_min "));Serial.println(PumpOnTimer_min);
      EEPROMWriteInt(PumpOnTimer_min_EEADR,PumpOnTimer_min);    
      currentMillis_ONTimer = millis();
      previousMillis_ONtimer = currentMillis_ONTimer;
      PumpOnTimer_elapsed_min=0;
      /*
      SMS2Send = " Pump ON for  ";
      SMS2Send += PumpOnTimer_min;  //PumpONTime;
      SMS2Send += " min  ";
      */
      sprintf(buffer, " Pump ON for  %u min ", PumpOnTimer_min); // unsigned int

    }  
  }
  else  // command is * or ** or 1 or 2 or 3 or 2*1 or 2*0 or 3*1 or 3*0
  {
    if (SMS_Read=="*")   // for SMS Command list
    {
     // Serial.println(F("SMS Command List Request Received !"));
      sprintf(buffer, "1 - ON, 2 - OFF, 3 - Status\n1*30 - ON for 30 min\n1*5*20*10 - ON 5 min, OFF 20 min, 10 times");
    //  strcat(buffer, "\nSMS ** - Advanced Commands");   // total 120
    }
    /*   
    else if (SMS_Read=="**")   // for advanced SMS Command list
    {
     // Serial.println(F("SMS advanced Command List Request Received !"));      
     // sprintf(buffer, "To Register\nSMS 'Master' at startup fast blinking\nOR\nSMS '*100*MobileNo'");
      sprintf(buffer, "SMS 'Master' from desired mobile number during fast blue LED blinking at startup.");
    //  strcat(buffer, "\nSMS *** - Config Commands");   // total 120
    }  
    else if (SMS_Read=="***")   // Config commands
    {
     // Serial.println(F("SMS Config Command List Request Received !"));      
      // *2*1   - Resume Pump status ,Timer at Restart
      // *2*0   - Pump ,Timer OFF at Restart
      // *3*0   - Stop sending Pump Status SMS every 5 min from Devise    
      // *3*1   - Start sending Pump Status SMS every 5 min from Devise       
     
     // sprintf(buffer, "'*2*1'-Resume Pump status ,Timer at Restart\n'*2*0'- Pump ,Timer OFF at Restart\n");
     // strcat(buffer, "\n'*3*0'- Stop Pump Status SMS every 5 min\n'*3*1'- Start Pump Status SMS every 5 min");   // total 120
      sprintf(buffer, "'*2*1'- Resume Pump status \n'*2*0'- Pump OFF ");
      strcat(buffer, "\n'*3*0'- Stop SMS every 5 min\n'*3*1'- Start SMS every 5 min");   // total 120
    } 
    */
    else if (SMS_Read=="1") 
    {
     // Serial.println("PUMP ON");
      if (!PumpState)
      {
        PumpState =1;
        One_Min_Counter = 0;  // Reset counter to Start Counter for Pump ON time
        EEPROMWriteInt(One_Min_Counter_EEADR, One_Min_Counter);
        currentMillis_timer = millis();
        previousMillis_timer = currentMillis_timer;
        Pump_and_LED_Action_and_Reset_Timer_and_IrrigationCycle(PumpState); // 
        sprintf(buffer, "PUMP ON");
      }
      else
      {
        sprintf(buffer, "PUMP is already ON for %u min" , One_Min_Counter);
      }
    } 
    else if (SMS_Read=="2") 
    {
     // Serial.println(F("Pump OFF"));  
      PumpState =0;
      Pump_and_LED_Action_and_Reset_Timer_and_IrrigationCycle(PumpState); //     
      sprintf(buffer, "PUMP OFF");
    } 
    else if (SMS_Read=="3") 
    {
      //Serial.print(F("Pump Status: "));
      if (digitalRead(LED1_Pump)) 
      {
       // Serial.println(F("ON"));
        sprintf(buffer, "PUMP ON");
      } 
      else 
      {
        //Serial.println(F("OFF"));
        sprintf(buffer, "PUMP OFF");
      }
    }

    else if (SMS_Read.indexOf("*100*") >= 0)
    {
      // Serial.println(F("Saving Master PhNum"));
      Master_phone = SMS_Read.substring(5);  //.toCharArray(myStringChar, BUFSIZE); //convert string to char array
      Master_phone = COUNTRY_CODE + Master_phone ;// country code. +91
     // Serial.println(Master_phone);     // save with country code. +91
      Master_phone.toCharArray(buf, BUFSIZE); //convert string to char array
      eeprom_write_string(Master_phone_EEADR, buf); 
      sprintf(buffer, "You are now Master");
      ToReset=1;
    }  


    /*
    else  
    {
      
      //check for 
      // "*100*8197239206"; and set Master no. if true
      // *2*1   - Resume Pump status ,Timer at Restart
      // *2*0   - Pump ,Timer OFF at Restart
      // *3*0   - Stop sending Pump Status SMS every 5 min from Devise    
      // *3*1   - Start sending Pump Status SMS every 5 min from Devise 

      if (SMS_Read.indexOf("*100*") >= 0)
      {
        // Serial.println(F("Saving Master PhNum"));
        Master_phone = SMS_Read.substring(5);  //.toCharArray(myStringChar, BUFSIZE); //convert string to char array
        Master_phone = COUNTRY_CODE + Master_phone ;// country code. +91
        Serial.println(Master_phone);     // save with country code. +91
        Master_phone.toCharArray(buf, BUFSIZE); //convert string to char array
        eeprom_write_string(Master_phone_EEADR, buf); 
        sprintf(buffer, "You are now Master");
        ToReset=1;
      }       
      else if (SMS_Read.indexOf("*2*1")>=0)   // ("*2*1"); Resume Pump status ,Timer at Restart
      {
        Pump_Status_on_Power_On = 1;
        EEPROM.write(Pump_Status_on_Power_On_EEADR ,Pump_Status_on_Power_On); 
        sprintf(buffer,"RESUME Pump / Timer at Startup!!");        
      }
      else if (SMS_Read.indexOf("*2*0")>=0)   // ("*2*0");  Pump status ,Timer  = OFF at Restart
      {
        Pump_Status_on_Power_On = 0;
        EEPROM.write(Pump_Status_on_Power_On_EEADR ,Pump_Status_on_Power_On); 
        sprintf(buffer,  "STOP Pump / Timer at Startup!!");
      }
      else if (SMS_Read.indexOf("*3*1")>=0)   // ("*3*1"); Start sending Pump Status SMS every 5 min from Devise
      {
        SendSMSin5Min = 1;
        EEPROM.write(SendSMSin5Min_EEADR ,SendSMSin5Min);
        sprintf(buffer, "SEND SMS every 5 min if Pump /Timer ON"); 
      }
      else if (SMS_Read.indexOf("*3*0")>=0)   // ("*3*0"); Donot send Pump Status SMS every 5 min from Devise
      {
        SendSMSin5Min = 0;
        EEPROM.write(SendSMSin5Min_EEADR ,SendSMSin5Min); 
        sprintf(buffer, "DONOT SEND SMS every 5 min if Pump /Timer ON"); 
      }
      else 
      {    
       //Serial.println(F("Wrong Command!"));
        sprintf(buffer, "Wrong Command. SMS * for SMS Command");
      }
    }   
    */ 
    else 
    {    
      //Serial.println(F("Wrong Command!"));
      sprintf(buffer, "Wrong Command. SMS * for SMS Command");
    }  
  }  
  
 // sprintf(buffer,"%s%s", buffer,"\n SMS * for command list!");
  Serial.print(F("Send SMS : "));Serial.println(buffer);  
  if(SEND_SMS)
  {
    //sendSMS(smsfrom, buffer); 
    sendSMS(buffer); 
  }
  if (ToReset){Serial.println(F("Reboot!!")); delay(2000); resetFunc(); }  

}








