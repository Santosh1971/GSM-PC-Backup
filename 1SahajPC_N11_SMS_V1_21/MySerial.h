#include <SoftwareSerial.h>
SoftwareSerial SerialAT(2, 3);  

//Library Definition
// The circuit:
// * RX is digital pin 10 (connect to TX of other device)
// * TX is digital pin 11 (connect to RX of other device)
// SoftwareSerial mySerial(10, 11); // RX, TX


//#ifndef _SS_MAX_RX_BUFF
//#define _SS_MAX_RX_BUFF 128 // RX buffer size by default 64 by default
//#endif




// See all AT commands, comment it for production
//#define DUMP_AT_COMMANDS 1

//const char* command = "AT"; // AT command to send
//const int timeout = 2000; // timeout in milliseconds
//const char* expectedResponse = "OK"; // expected response from modem

/*
ChatGPT
This code uses the SoftwareSerial library to create a serial communication channel on pins 2 and 3, 
and sends an AT command to the GSM modem. It then waits for a response for a specified timeout period 
(in this case, 1000 milliseconds) and compares the response to an expected response ("OK"). 
If the response is received within the timeout and matches the expected response, the function returns. 
If no response is received or the response does not match the expected response, 
the function can handle the timeout as per your requirement.
*/


String response((char *)0);
bool ATCommandStatus;

uint8_t sendATCommand(const char* command, unsigned int timeout, const char* expectedResponse) 
{
  //String response((char *)0);
  //response.reserve(100);// 150 required 90
 //String response;

  SerialAT.println(command); // send AT command with <CR>
  #ifdef DUMP_AT_COMMANDS 
 // Serial.print(F("Sent..")); Serial.println(command);
  #endif
  
  unsigned long startTime = millis();
  while ((millis() - startTime) < timeout) 
  {
    if (SerialAT.available()>0) 
    { 
      //String response = SerialAT.readString();  //Message Send Error!!!! 
       response = SerialAT.readString();
       response.trim();
      //String response = SerialAT.readStringUntil('\n');
      
      //if ((SerialAT.readString()).indexOf(expectedResponse) != -1) 
      if (response.indexOf(expectedResponse) != -1) 
      {
        // response matches expected response
       // #ifdef DUMP_AT_COMMANDS 
        //response.replace("\n", "  ");
        //  response.trim();
        Serial.print(F("   T->")); Serial.println(response);  // Result as expected
        //   digitalWrite(A3,HIGH); //For Testing only  #define LED2_GSM A3  //GSM LED  BLUE
        //   delay(500);//For Testing only
        //  digitalWrite(A3,LOW);//For Testing only
      //  #endif
        return 1;
      }
      else
      {
      //  #ifdef DUMP_AT_COMMANDS 
        //response.replace("\n", "  ");
      //  response.trim();
        Serial.print(F("   F?->"));Serial.println(response); //Response not matching..
      //  #endif
        return 0;
      }
    }
  }
//  #ifdef DUMP_AT_COMMANDS 
  Serial.println(F("   TO !!!")); //   Response not Rcvd  .... Timeout!!
//  #endif
  // response not received in time
  // handle timeout
  return 0;
}



/*
int sendATCommand(const char* command, int timeout, const char* expectedResponse) {
  char response[200];
  int responseIdx = 0;
  unsigned long startTime = millis();
  
  SerialAT.println(command);
  while (true) {
    if (SerialAT.available() > 0) {
      response[responseIdx] = SerialAT.read();
      responseIdx++;
      response[responseIdx] = '\0';
      if (strstr(response, expectedResponse) != NULL) {
        #ifdef DUMP_AT_COMMANDS 
        Serial.print(F("   T->")); Serial.println(response);  // Result as expected
        #endif
        return 1;
      }
    //  else{
    //    #ifdef DUMP_AT_COMMANDS 
     //   Serial.print(F("   F?->"));Serial.println(response); //Response not matching..
     //   #endif
     // }
    }
    if (millis() - startTime > timeout) {
      #ifdef DUMP_AT_COMMANDS 
      Serial.println(F("   TO !!!")); //   Response not Rcvd  .... Timeout!!
      #endif
      return -1;
    }
  }
}


*/



/*

int MqttConnected(void)
{
  if (sendATCommand("AT+MQTTSTATE?", 1000, "+MQTTSTATE:1"))       //Query the socket status.
  {
    return 1;
  }
  return 0;


}

*/
