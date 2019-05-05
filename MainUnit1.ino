// Header Files for Radio Stuff
#include <RF24.h>
#include <RF24Network.h>
#include <printf.h>
#include <Wire.h>


// LTE Shield header files
#include <SoftwareSerial.h>
#include <SparkFun_LTE_Shield_Arduino_Library.h>

// Time elapsed counter
unsigned long oneSecCounter;
unsigned long textSecCounter;
unsigned long disconnect1Counter;
unsigned long disconnect2Counter;



// Create a SoftwareSerial object to pass to the LTE_Shield library
SoftwareSerial lteSerial(8, 9);

// Create a LTE_Shield object to use throughout the sketch
LTE_Shield lte;


//RF Stuff
RF24 radio(2, 3);                // nRF24L01 (CE,CSN)
RF24Network network(radio);      // Include the radio in the network
const uint16_t hub = 00;         // Parent
const uint16_t node01 = 01;      // Child
const uint16_t node02 = 02;


//Temperature Stuff
int temp1 = 50;
int temp2 = 50;
int sendText = 0;
int smsInterval = 1;
int testText = 0;

// Defining the integers for phone number input
int num1 = 0;
int num2 = 0;
int num3 = 0;
int num4 = 0;
int num5 = 0;
int num6 = 0;
int num7 = 0;
int num8 = 0;
int num9 = 0;
int num10 = 0;
String DESTINATION_NUMBER;


void setup() {

  //Begin writing to serial
  Serial.begin(9600);


  // Radio begin stuff
  SPI.begin();
  radio.begin();
  network.begin(90, hub);


  //Begin serial communication with Arduino and LTE Board
  lte.begin(lteSerial, 9600);

  // Arduino Board Communication
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // Sending Data register event
  Wire.onReceive(receiveEvent); // Receiving Data register event


  // Starting up the disconnect counters

  disconnect1Counter = millis();
  disconnect2Counter = millis();


}

void loop() {



  radioStuff();
  integToStringConv();
  sendHelp();
  testTextFun();
  disconnectNotification();



  // One Second loop runner

  if (millis() - oneSecCounter >= 1000 ) {

    oneSecCounter = millis();

//    Serial.println(millis() - disconnect1Counter);


//      lte.sendSMS("7406753099", "HELP! 2");
//
//Serial.println(num1);
//Serial.println(num2);

//      sendTxtMsg("Alert! Temperature out of range \nTemp Sensor 1 : " + String(temp1) + "\n"  + "Temp Sensor 2 : " + String(temp2) );

//      sendTxtMsg(String(temp2));




  }




}






void radioStuff()
{


  RF24NetworkHeader header;
  int fahrenheit1, fahrenheit2;
  network.update();
  //===== Receiving =====//
  if ( network.available() ) {

    // Is there any incoming data?
    network.peek(header);
    if (header.type == '1') {
      network.read(header, &fahrenheit1, sizeof(fahrenheit1));
      Serial.print("Node 1: ");
      Serial.println(fahrenheit1);
      temp1 = fahrenheit1;

      disconnect1Counter = millis();


    }
    network.peek(header);
    if (header.type == '2') {
      network.read(header, &fahrenheit2, sizeof(fahrenheit2));
      Serial.print("Node 2: ");
      Serial.println(fahrenheit2);
      temp2 = fahrenheit2;

      disconnect2Counter = millis();
    }

  }


}



// Sending data to main unit 2
void requestEvent() {
  if (temp1 < 100 && temp1 >= 50) {
    Wire.write(temp1 - 50); // respond with message of 1 byte
  }
  else {
    Wire.write(54); // respond with int 54 if temp is above 100 to avoid crashes
  }


  if (temp2 < 100 && temp2 >= 50 ) {
    Wire.write(temp2); // respond with message of 1 byte
  }
  else {
    Wire.write(99); // respond with int 99 if temp is above 100 to avoid crashes
  }


}



// Receiving data from main unit 2
void receiveEvent(int howMany) {


  // Read phone number from num1 to num 10

  int x = Wire.read();    // receive byte as an integer

  if (x < 10) {
    num1 = x;
  }

  if (x >= 10 && x < 20) {
    num2 = x - 10;
  }

  if (x >= 20 && x < 30) {
    num3 = x - 20;
  }

  if (x >= 30 && x < 40) {
    num4 = x - 30;
  }

  if (x >= 40 && x < 50) {
    num5 = x - 40;
  }

  if (x >= 50 && x < 60) {
    num6 = x - 50;
  }

  if (x >= 60 && x < 70) {
    num7 = x - 60;
  }

  if (x >= 70 && x < 80) {
    num8 = x - 70;
  }

  if (x >= 80 && x < 90) {
    num9 = x - 80;
  }

  if (x >= 90 && x < 100) {
    num10 = x - 90;
  }


  // Text message trigger value

  if (x >= 100 && x < 110) {
    sendText = x - 100;
  }

  // SMS Interval

  if (x >= 110 && x < 171) {
    smsInterval = x - 110;
  }

  // Test SMS

  if (x >= 180 && x < 190) {
    testText = x - 180;
  }



}


void integToStringConv()
{

  // Integer to String Conversion
  DESTINATION_NUMBER = String(String(num1) + String(num2) + String(num3) + String(num4) + String(num5) + String(num6) + String(num7) + String(num8) + String(num9) + String(num10));

}


// This function sends a text message
//void sendTxtMsg(String msg)
//{
//
//  // Call lte.sendSMS(String number, String message) to send an SMS
//  lte.sendSMS(DESTINATION_NUMBER, msg);
//
//}


void sendHelp() {

  if (sendText == 1) {

    Serial.println("Attempting to send sms");


    if (millis() - textSecCounter >= 60000 * smsInterval ) {

      Serial.println("Sending sms");

      textSecCounter = millis();


          lte.sendSMS(DESTINATION_NUMBER, "Alert! Temperature out of range \nTemp Sensor 1 : " + String(temp1) + "\n"  + "Temp Sensor 2 : " + String(temp2));


    }
  }

}

void testTextFun() {


  if (testText != 0) {
    Serial.println("Sending sms");

          lte.sendSMS(DESTINATION_NUMBER, "Alert! Temperature out of range \nTemp Sensor 1 : " + String(temp1) + "\n"  + "Temp Sensor 2 : " + String(temp2));

  }


}


void disconnectNotification() {

  if (millis() - disconnect1Counter >=     60000 * smsInterval ) {
         
    lte.sendSMS(DESTINATION_NUMBER, "Alert! Sensor 1 Disconnected");

//    sendTxtMsg("Alert! Sensor 1 Disconnected");
    disconnect1Counter = millis();

    temp1 = 50;
  }

  if (millis() - disconnect2Counter >=     60000 * smsInterval ) {
    
        lte.sendSMS(DESTINATION_NUMBER, "Alert! Sensor 2 Disconnected");
    
//    sendTxtMsg("Alert! Sensor 2 Disconnected");
    disconnect2Counter = millis();

    temp2 = 50;

  }

}
