//------------------------------------------------------------------------
// Minimum Feature SMS based
// SahajPC_N11_SMS_V1_21    SMS based Pump controller 
// SMS 'Master' from desired mobile number during fast blue LED blinking at startup.
// --------------- SMS commands ( Using Master- only )---------------------------
// 1 - ON  or 2 - OFF or 3 - Pump Status
// 1*30  - Pump ON for 30 min   // max 65000 min or 45 days
// 1*5*20*10     - PUMP ON for 5 min,OFF for 20 min , 10 times
// * get sms command list 
// SMS to send from other than "Master" to the Pump SIM no.
  // Add Master no before SMS command if Sender is other than Master.
  // For Example
  // 81972392061(master mobile)1      to PUMP ON
  // 819723920612                     to PUMP OFF
  // 819723920611*30                  to Pump ON for 30 min 
  // 819723920611*5*20*10             to PUMP ON for 5 min,OFF for 20 min , 10 times

// Command 1 or 2 will stop Timer/ irrigation cycle 
// In case of Timer or Irrigation the Pump Status will Resume when Power comes back. otherwise Pump will be OFF on Power ON/Resume.

// SMS Response ---
// System Started !!! Pump  OFF
// System Started !!! Pump  was ON for 2 min before power cut

//---- If timer is ON --------
// System Started !!! Pump ON for 7 min. 7 min more to go.
// Pump ON for 10 min. 4 min more to go.
// Pump OFF. 14 min. over.
//---- If Irrigation Cycle is ON --------
// Irrigation Cycle Started!
// Pump ON!
// 2 min ON, 3 min OFF, For 4 times

// Irrig ( 2 min ON, 3 min OFF)- 4 Cycle; Balance Cycle 2
//-------------------------------------------------------------------------
//#define SASYA_MASTER_MOBILE_NO "+918197239206" in "MyHardware.h"   //Universal Master to send SMS Command
// Like Master ,Universal Master can send all command and can Make anyone Master by sending *100*(MobileNo)

//------------------------------------------------------ 
// ----------- for next version ?--------------------------       
// ** get Advanced sms commafor next versionnd list          SMS 'Master' from desired mobile number during fast blue LED blinking at startup.
// *** get config command list         //2*1'- Resume Pump status ,*3*1'- Start SMS every 5 min
// *2*1   - Resume Pump status ,Timer at Restart
// *2*0   - Pump ,Timer OFF at Restart
// *3*0   - Stop sending Pump Status SMS every 5 min from Devise    
// *3*1   - Start sending Pump Status SMS every 5 min from Devise   
// feedback of Pump ON/OFF..
// ----------- for next version -------------------------- 
//--------------------------------------------------

// Note: 
//Command 1 or 2 will stop Timer/ irrigation cycle  
// *100*8197239206(master mobile) 8197239206 will be registered as Master ,( Master can only assign another Master Remotly )- No need to convey to customer , can be used by engineer. 

// default:
// *2*1   - Resume Pump status ,Timer at Restart
// *3*0   - Stop sending Pump Status SMS every 5 min from Devise 

// Command is executed if the SMS is received from Master or from any mobile if 
// following is the message format( add Master mobile no. and then usual command)
//  * Start the Pump                        81972392061(master mobile)1      PUMP ON

// SMS Response ---
// System Started !!! Pump  OFF
//  System Started !!! Pump  was ON for 2 min before power cut

//---- If timer is ON --------
// System Started !!! Pump ON for 7 min. 7 min more to go.
// Pump ON for 10 min. 4 min more to go.
// Pump OFF. 14 min. over.


#include "MyHardware.h"

#define DUMP_DEBUG 

String stringFilename  =__FILE__;       // const char compile_date[] = __DATE__ " " __TIME__;

// Filename is same as where this Function is , so it will be in the main .ino file.
void Print_SystemInfo()
{        
   // String stringFilename  =__FILE__;       // const char compile_date[] = __DATE__ " " __TIME__;  
    int lastslash = stringFilename.lastIndexOf('\\');
    stringFilename =stringFilename.substring(lastslash+1);
  //  wdt_reset();     // Reset Watchdog Timer Regularly  
    Serial.print(F(" # File name: "));Serial.print(stringFilename);   
  //  wdt_reset();     // Reset Watchdog Timer Regularly       
    Serial.print(F(" #  Date: "));Serial.print(__DATE__);
  //  wdt_reset();     // Reset Watchdog Timer Regularly  
    Serial.print(F(" #  Time: "));Serial.println(__TIME__);
}

/* -------  SMS Received -----------

+CMGL: 1,"REC UNREAD","+918197239206",,"2023/05/21,15:13:37+06"
Hello
OK
// M2M SIM mobile no( +91(13Digits)) : +915754100153590

*/


void setup() 
{
  init_LED_and_Relay();
  GSM_Modem_Power_Down();
  LED_Self_Test();  // Start of System

  Serial.begin(115200);   
  SerialAT.begin(19200); // SerialAT.begin(9600); 
 // SerialAT.begin(9600); // SerialAT.begin(9600);  not working
 // SerialAT.begin(115200); 

  delay(10);
 
  Serial.println(F(" "));
  Serial.println(F("...  Starting  ..."));

  Print_SystemInfo();
 // Print_MasterNo();
 
  Load_EEPROM_or_DEFAULT();  // load Pump state , last counter   either Timer or Irrigation
  GSM_Modem_Power_UP();
  //check AT serial at modem Startup
   //MODEM:STARTUP
   //+PBREADY

  if (SerialAT.available()) 
  {
    String response = SerialAT.readString();
    response.trim();
    Serial.println(response);
    if (response.indexOf("MODEM:STARTUP") != -1) 
    {
      Serial.println(F("Modem OK"));  //--- Modem Started Successfully!!! ---
    }
  }
  /*
  else
  {
    //Serial.println(F("No Startup Message Received from GSM Modem...Sending AT command...")); delay(1000); 
    Serial.println(F("Err...Send AT ")); delay(1000); 
    ATCommandStatus=sendATCommand("AT", 2000, "OK");
    if (!ATCommandStatus){ATCommandStatus = sendATCommand("AT", 2000, "OK");}
    if (!ATCommandStatus){ATCommandStatus = sendATCommand("AT", 2000, "OK");}
   // if (!ATCommandStatus) {Serial.println(F("No Communication with Modem...Rebooting!!")); delay(1000); resetFunc(); }
    if (!ATCommandStatus) {Serial.println(F("No AT..Reboot!!")); delay(1000); resetFunc(); }
  }
  */
  CheckAndSetBaudRate();

  startup_flag=1;  // To send SMS "System Started"
  /*
  if(Pump_Status_on_Power_On)  //------------- Check if  Option of Resume last Pump status is ON.--------------
  {
    Pump_and_LED_Action(PumpState);  // Last pump state before power off.
  //  Serial.println(F("Restore Last Pump State at Start up is Enabled !"));
  }
  else
  {
    PumpState=0;
    Pump_and_LED_Action_and_Reset_Timer_and_IrrigationCycle( PumpState);  
   // Serial.println(F("Default Pump State, Timer,Irrig Cycle...OFF"));
  }
  
  */


  if(Pump_is_ON_withTimer)         //------------- Check if  Option of Timer is ON
  {
   // Serial.print(F("Pump Was ON for(minute) : "));Serial.print(PumpOnTimer_elapsed_min );Serial.print(F("  out of : "));Serial.println(PumpOnTimer_min );       
    currentMillis_timer = millis();
    previousMillis_timer = currentMillis_timer; 
  }
  else if (Pump_is_ON_withIrrigationCycle)           //------- Check if  Option of Irrigation cycle   ----------                 
  {
    //  Serial.print(F("Irrg Cycle completed : "));Serial.print(PumpONOFFCount_elapsed );Serial.print(F("  out of : "));Serial.println(PumpONOFFCount );       
    //  Serial.print(F("ON(Minute) : "));Serial.print(PumpONTime );Serial.print(F("  OFF(Minute) : "));Serial.println(PumpOFFTime );
    currentMillis_ONTimer = millis();
    previousMillis_ONtimer= currentMillis_ONTimer;            
  }
  else
  {
    PumpState=0;   // Pump OFF at Start
  }
  
  Pump_and_LED_Action(PumpState);  // Last pump state before power off


 // SerialAT.println("AT+CMGF=1");  //Set the SMS to text mode.
  ATCommandStatus= sendATCommand("AT+CMGF=1", 1000, "OK");       // Set the text mode for SMS
  if(!ATCommandStatus){ATCommandStatus= sendATCommand("AT+CMGF=1", 1000, "OK");}

  delay(1000);
  SerialAT.println("AT+CNMI=2,2,0,0,0"); //To set the mode how the module informs users of new SMS messages received from the network
                                         //new messages are displayed directly rather than stored on the SIM card).
  if (SerialAT.available()) {} // clear buffer before getting SMS
  /* uncheck for prod..*/
  Serial.println(F("Check 'Master' SMS for 10 Sec..."));  //GSM Modem Ready , Checking for 'Master' SMS for 10 Sec...
  int chk = check_Master_SMSatStartup(10000); //  (30000);  // within 30 Sec to assign Mater No.
  Serial.print(F("Master SMS rcved? : ")); Serial.println(chk);
  if (chk >=0){Serial.println(F("Reboot!!")); delay(5000); resetFunc(); } // delay required for SMS to be sent
  
  senderPhoneNumber= Master_phone;
  Send_PumpStatus();     // Check Pump and irrigation timer ststus and Send SMS


}


/**********************************************************************************************/
/*****************************************    Main Loop      **********************************/
/**********************************************************************************************/

void loop() 
{
  uint8_t StartIndex ,EndIndex; 
 // String senderPhoneNumber , SMSContent;

  BlinkGSMLED(2000);  // blink GSM LED every 2 Sec
  
  
  if(Pump_is_ON_withTimer ==1)                        //------- Timer ------------
  {      
    RunTimer();
  }
  else if (Pump_is_ON_withIrrigationCycle == 1)       //------- Irrigation cycle   ----------                 
  {
    RunIrrigationCycle();
  } 
  else if(PumpState)     //  Keep track of Pump ON time 
  {
    currentMillis_timer = millis();
    if (currentMillis_timer - previousMillis_timer >= one_min_interval) // every 60 sec
    {
      previousMillis_timer = currentMillis_timer;
      One_Min_Counter = One_Min_Counter + 1;
      EEPROMWriteInt(One_Min_Counter_EEADR, One_Min_Counter);
      if((One_Min_Counter % 5)==0)  // send data every 5 min
      {                  
        if(SendSMSin5Min)
        {
          Send_PumpStatus();
        }      
      }
    }
  }

  /*- SMS received
  +CMT: "+918197239206",,"2023/05/21,22:00:55+06"
  2
  */ 
  if (SerialAT.available()>0)   //------- SMS received by GSM Module and sent to controller through UART  ---------- 
  {    
    String message = SerialAT.readString();
    message.trim();
    Serial.print( "message :");Serial.println(message);
    if (message.indexOf("+CMT:") != -1)
    {   
      // Find the start and end index of the sender's phone number
      StartIndex = message.indexOf("\"")  +1;
      EndIndex = message.indexOf("\"", StartIndex);
      senderPhoneNumber = message.substring(StartIndex, EndIndex);
      Serial.print(F( "RcvdPhNum :"));Serial.println(senderPhoneNumber); // +918197239206

      StartIndex = message.indexOf("\n")  +1;
      EndIndex = message.indexOf("\n", StartIndex);
      SMSContent = message.substring(StartIndex, EndIndex);
      Serial.print(F( "SMSRcvd  :"));Serial.println(SMSContent); 
      deleteSMS(); 
      
      // If SMS is from Master or the SMS is MasterNo. or Sasya Master no. followed by Command then process or discard.      
      if((senderPhoneNumber == Master_phone)|| (senderPhoneNumber == SASYA_MASTER_MOBILE_NO))  // ]Master #: 8197239206
      {        
        //Respond_SMS(senderPhoneNumber,SMSContent);
        Respond_SMS_to_sender(SMSContent);
      }
      else
      { 
        //String  Master_phone1 = Master_phone.substring(3,13); //+918197239206 compare without +91
        //int length = str.length();
        String  Master_phone1 = Master_phone.substring(COUNTRY_CODE.length(),13); //+918197239206 compare without +91
       // Serial.println(Master_phone1);
       // Serial.println(SMSContent.substring(0,10));
        if(SMSContent.substring(0,10) == Master_phone1)   // Check if the 1st 10 digit is stored Master no,
        {
          SMSContent = SMSContent.substring(10);  // 88619960201 - 10 digit mobile no.
         // Serial.println(SMSContent);
          //Respond_SMS(senderPhoneNumber,SMSContent);
          Respond_SMS_to_sender(SMSContent);
        }
        else
        {
          Serial.print(F( "SMS Err,Master is "));Serial.println(Master_phone); 
        }
      }
      if (SerialAT.available()>0){ message = SerialAT.readString(); Serial.print(F( "Clear message :"));Serial.println(message);}// to clear the buffer }     
    }
  }
}
