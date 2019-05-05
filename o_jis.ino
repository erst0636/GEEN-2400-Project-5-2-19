//Header Files for Ethernet Board
#include <ThingSpeak.h>
#include <SPI.h>
#include <Ethernet.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>


// Header File for Arduino communication
#include <Wire.h>

// Set shield mac address
byte mac[] = { 0xA8, 0x61, 0x0A, 0xAE, 0x0D, 0x0C };

// Time elapsed since last program run tracker
unsigned long thingspeakPreviousTime;
unsigned long fiveSecCounter;

// Defining the cursor to change phone number
int screenCursor = 1;

// Global Temp Variable
int temp1 = 55;
int temp2 = 55;
int sendText = 0;
int sendTestText = 0;
String DESTINATION_NUMBER;
int LowTemp = 50;
int HighTemp = 100;
int num[10];
int id[16];
int smsInterval = 1;
int thingspeakInterval = 20;


// Defining Pins
char lowTempInput = A0;
char highTempInput = A1;
const int buttonPin = 24;
const int buttonPin2 = 26;
int buttonState = 0;
int buttonState2 = 0;
const int buttonPinDown = 25;
const int buttonPinUp = 27;
char APIKey;


// Setup for LCD
LiquidCrystal lcd(40, 39, 38, 37, 36, 35);






// Setting up the ethernet client
EthernetClient client;



//----------------  Fill in your credentails   ---------------------

unsigned long myChannelNumber = 0;  // Replace the 0 with your channel number
// const char* myWriteAPIKey = "B4SZAI5AZGZNM8SJ";    // Paste your ThingSpeak Write API Key between the quotes
//------------------------------------------------------------------



void setup() {


  id[0] = EEPROM.read(11);
  id[1] = EEPROM.read(12);
  id[2] = EEPROM.read(13);
  id[3] = EEPROM.read(14);
  id[4] = EEPROM.read(15);
  id[5] = EEPROM.read(16);
  id[6] = EEPROM.read(17);
  id[7] = EEPROM.read(18);
  id[8] = EEPROM.read(19);
  id[9] = EEPROM.read(20);
  id[10] = EEPROM.read(21);
  id[11] = EEPROM.read(22);
  id[12] = EEPROM.read(23);
  id[13] = EEPROM.read(24);
  id[14] = EEPROM.read(25);
  id[15] = EEPROM.read(26);
  thingspeakInterval = EEPROM.read(27);




  // Reading last phone number used from memory

  for (int i = 0; i < 10; i++) {
    num[i] = EEPROM.read(i);
  }

  // Reading sms interval from memory

  smsInterval = EEPROM.read(10);

  // Starting arduino communication function
  Wire.begin();   // join i2c bus

  // Setup LCD 16 Char, 2 Lines
  lcd.begin(16, 2);

  // Clearing LCD
  lcd.clear();

  //Setting pushbutton input
  pinMode(buttonPin, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(buttonPinDown, INPUT);
  pinMode(buttonPinUp, INPUT);

  // Setup SPI Master mode
  pinMode(53, OUTPUT);
  digitalWrite(53, HIGH);

  // Write to usb serial
  Serial.begin(9600);

  Serial.println("Hello World!");

  // Testing internet connectiong n stuff

  if (Ethernet.linkStatus() == LinkON) {


    Serial.println("Ethernet cable is connected");

    int x = Ethernet.begin(mac);

    if ( x == 1);
    {
      Serial.print("DHCP Connected IP is ");
    }

    Serial.println(Ethernet.localIP());

    Serial.print("The DNS server IP address is: ");
    Serial.println(Ethernet.dnsServerIP());

    Serial.print("The gateway IP address is: ");
    Serial.println(Ethernet.gatewayIP());


    if (client.connect("www.google.com", 80))
    {
      Serial.println("Internet Connection Test Passed");
    }
    else {
      Serial.println("Internet Connection Test Failed");
    }


    // Starting the thingspeak client function
    ThingSpeak.begin(client);

  }

  else {
    Serial.println("Ethernet cable is disconnected");
  }



}

void loop() {


  thingSpeakUpdate();
  receiveTemp();
  sendtoHub1();
  testMsgButton();
  integToStringConv();
  printMe();
  tempSetting();
  cursorButton();
  numberChanger();
  tempMonitor();




  // One Second loop runner

  if (millis() - fiveSecCounter >= 1000 ) {

    fiveSecCounter = millis();






  }







}


void thingSpeakUpdate()
{

  // Ethernet Renew
  Ethernet.maintain();

  // Set the fields with the values
  ThingSpeak.setField(1, temp1);
  ThingSpeak.setField(2, temp2);


  if (millis() - thingspeakPreviousTime >= thingspeakInterval * 1000 ) {

    thingspeakPreviousTime = millis();



    //Update thingspeak only if ethernet is connected
    if (Ethernet.linkStatus() == LinkON) {

      String key = String(String(char(id[0])) + String(char(id[1])) + String(char(id[2])) + String(char(id[3])) + String(char(id[4])) + String(char(id[5])) + String(char(id[6])) + String(char(id[7])) + String(char(id[8])) + String(char(id[9])) + String(char(id[10])) + String(char(id[11])) + String(char(id[12])) + String(char(id[13])) + String(char(id[14])) + String(char(id[15])));
      const char* myWriteAPIKey = key.c_str();

      // write to the ThingSpeak channel
      int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
      if (x == 200) {
        Serial.println("Channel update successful.");
      }
      else {
        Serial.println("Problem updating channel. HTTP error code " + String(x));
      }

    }


  }

}

void receiveTemp() {
  // Requesting Temperature from main hub 1

  Wire.requestFrom(8, 2);    // request 2 bytes from slave device #8 1 byte for every number

  while (Wire.available()) { // slave may send less than requested
    int c = Wire.read(); // receive a byte as integer

    if ( c < 50) {

      temp1 = c + 50;

    }

    if ( c >= 50 && c < 100) {

      temp2 = c ;

    }


  }
}

void sendtoHub1() {

  // Sending phone number

  Wire.beginTransmission(8); // transmit to device #8
  if (num[0] < 10) {
    Wire.write(num[0]);              // sends one byte
  }
  Wire.endTransmission();    // stop transmitting

  Wire.beginTransmission(8); // transmit to device #8
  if (num[1] < 10) {
    Wire.write(num[1] + 10);              // sends one byte
  }
  Wire.endTransmission();    // stop transmitting

  Wire.beginTransmission(8); // transmit to device #8
  if (num[2] < 10) {
    Wire.write(num[2] + 20);              // sends one byte
  }
  Wire.endTransmission();    // stop transmitting

  Wire.beginTransmission(8); // transmit to device #8
  if (num[3] < 10) {
    Wire.write(num[3] + 30);              // sends one byte
  }
  Wire.endTransmission();    // stop transmitting

  Wire.beginTransmission(8); // transmit to device #8
  if (num[4] < 10) {
    Wire.write(num[4] + 40);              // sends one byte
  }
  Wire.endTransmission();    // stop transmitting

  Wire.beginTransmission(8); // transmit to device #8
  if (num[5] < 10) {
    Wire.write(num[5] + 50);              // sends one byte
  }
  Wire.endTransmission();    // stop transmitting

  Wire.beginTransmission(8); // transmit to device #8
  if (num[6] < 10) {
    Wire.write(num[6] + 60);              // sends one byte
  }
  Wire.endTransmission();    // stop transmitting

  Wire.beginTransmission(8); // transmit to device #8
  if (num[7] < 10) {
    Wire.write(num[7] + 70);              // sends one byte
  }
  Wire.endTransmission();    // stop transmitting

  Wire.beginTransmission(8); // transmit to device #8
  if (num[8] < 10) {
    Wire.write(num[8] + 80);              // sends one byte
  }
  Wire.endTransmission();    // stop transmitting

  Wire.beginTransmission(8); // transmit to device #8
  if (num[9] < 10) {
    Wire.write(num[9] + 90);              // sends one byte
  }
  Wire.endTransmission();    // stop transmitting

  Wire.beginTransmission(8); // transmit to device #8
  if (sendText < 10) {
    Wire.write(sendText + 100);              // sends one byte
  }
  Wire.endTransmission();    // stop transmitting


  Wire.beginTransmission(8); // transmit to device #8
  if (smsInterval < 61) {
    Wire.write(smsInterval + 110);              // sends one byte
  }
  Wire.endTransmission();    // stop transmitting


  Wire.beginTransmission(8); // transmit to device #8
  if (sendTestText < 10) {
    Wire.write(sendTestText + 180);              // sends one byte
  }
  Wire.endTransmission();    // stop transmitting



}



// Function that monitors the pushbutton and send a text text message when the button is pushed
void testMsgButton()
{



  if (digitalRead(buttonPin) == HIGH)
  {

    Serial.println("Sending Text");
    sendTestText = 1;

  }

  else {

    sendTestText = 0;
  }

}


// Function to print to lcd screen
void printMe()
{


  if (screenCursor == 32) {

    lcd.setCursor(0, 0);
    lcd.print("TSpk Updt Intval");
    lcd.setCursor(0, 1);
    lcd.print(thingspeakInterval);

    if (thingspeakInterval < 100) {
      lcd.setCursor(2, 1);
      lcd.print("   ");

    }

    if (thingspeakInterval >= 100 && thingspeakInterval < 1000) {
      lcd.setCursor(3, 1);
      lcd.print("  ");

    }



    if (thingspeakInterval >= 1000 && thingspeakInterval < 10000) {
      lcd.setCursor(4, 1);
      lcd.print(" ");

    }


    lcd.setCursor(5, 1);
    lcd.print("Seconds    ");



  }


  if (screenCursor == 31) {

    lcd.setCursor(0, 1);
    lcd.print("Saved           ");
    lcd.setCursor(15, 0);
    lcd.print(" ");

  }




  if (screenCursor >= 15 && screenCursor < 31) {

    lcd.setCursor(0, 0);
    lcd.print("ThingSpeak ID  ");
    lcd.setCursor(0, 1);

    char myWriteAPIKey[] = {id[0] , id[1] , id[2] , id[3] , id[4] , id[5], id[6], id[7], id[8] , id[9], id[10], id[11], id[12], id[13], id[14] , id[15] };

    //    Serial.println(myWriteAPIKey);

    lcd.print(myWriteAPIKey);



  }


  if (screenCursor >= 14 && screenCursor < 15) {

    lcd.setCursor(0, 0);
    lcd.print("SMS Interval    ");
    lcd.setCursor(0, 1);
    lcd.print(smsInterval);

    if (smsInterval < 10) {
      lcd.setCursor(1, 1);
      lcd.print(" ");
    }

    lcd.setCursor(2, 1);
    lcd.print(" minutes      ");


  }





  if (screenCursor >= 2 && screenCursor <= 13)
  {

    // Print Stuff
    lcd.setCursor(0, 0);
    lcd.print("Phone");

    lcd.setCursor(5, 0);
    lcd.print(" ");

    lcd.setCursor(6, 0);
    lcd.print(DESTINATION_NUMBER);


    if (screenCursor == 2)
    {
      lcd.setCursor(0, 1);
      lcd.print("                ");

    }





  }

  // Print to LCD stuff

  if (screenCursor == 1)
  {

    lcd.setCursor(0, 0);
    lcd.print("T1:");

    lcd.setCursor(3, 0);
    lcd.print(" ");

    lcd.setCursor(4, 0);
    lcd.print(temp1);

    lcd.setCursor(6, 0);
    lcd.print(" ");

    lcd.setCursor(7, 0);
    lcd.print("T2:");

    lcd.setCursor(10, 0);
    lcd.print(" ");

    lcd.setCursor(11, 0);
    lcd.print(temp2);

    lcd.setCursor(13, 0);
    lcd.print("   ");

    lcd.setCursor(0, 1);
    lcd.print("Low");

    lcd.setCursor(3, 1);
    lcd.print(" ");

    lcd.setCursor(4, 1);
    lcd.print(LowTemp);

    lcd.setCursor(6, 1);
    lcd.print(" High ");

    lcd.setCursor(12, 1);
    lcd.print(HighTemp);

    if (HighTemp < 100) {
      lcd.setCursor(14, 1);
      lcd.print("  ");
    }

    else {
      lcd.setCursor(15, 1);
      lcd.print(" ");
    }



  }

}

void integToStringConv()
{

  // Integer to String Conversion
  DESTINATION_NUMBER = String(String(num[0]) + String(num[1]) + String(num[2]) + String(num[3]) + String(num[4]) + String(num[5]) + String(num[6]) + String(num[7]) + String(num[8]) + String(num[9]));

}






void tempSetting()
{

  LowTemp = 0.0390625 * analogRead(lowTempInput)  + 46;

  HighTemp = 0.0390625 * analogRead(highTempInput) + 62;

}



// Function monitoring second pushbutton
void cursorButton()
{

  buttonState2 = digitalRead(buttonPin2);

  if (buttonState2 == HIGH)
  {


    screenCursor = screenCursor + 01;
    delay(175);

  }


  if (screenCursor == 33)
  {

    screenCursor = 01;

  }

}


// This function changes the phone number based on the input value
void numberChanger()

{


  // Changing Numbers

  if (screenCursor == 3)
  {

    numberChangeButton(0);

    lcd.setCursor(0, 1);
    lcd.print("      ");
    lcd.setCursor(6, 1);
    lcd.print("^");
    lcd.setCursor(7, 1);
    lcd.print("         ");

  }

  if (screenCursor == 4)
  {

    numberChangeButton(1);

    lcd.setCursor(6, 1);
    lcd.print(" ");
    lcd.setCursor(7, 1);
    lcd.print("^");

  }

  if (screenCursor == 5)
  {

    numberChangeButton(2);



    lcd.setCursor(7, 1);
    lcd.print(" ");
    lcd.setCursor(8, 1);
    lcd.print("^");
    lcd.setCursor(8, 1);
    lcd.print("^");
  }

  if (screenCursor == 6)
  {


    numberChangeButton(3);


    lcd.setCursor(8, 1);
    lcd.print(" ");
    lcd.setCursor(9, 1);
    lcd.print("^");
    lcd.setCursor(9, 1);
    lcd.print("^");
  }

  if (screenCursor == 7)
  {

    numberChangeButton(4);



    lcd.setCursor(9, 1);
    lcd.print(" ");
    lcd.setCursor(10, 1);
    lcd.print("^");
    lcd.setCursor(10, 1);
    lcd.print("^");
  }

  if (screenCursor == 8)
  {


    numberChangeButton(5);


    lcd.setCursor(10, 1);
    lcd.print(" ");
    lcd.setCursor(11, 1);
    lcd.print("^");
    lcd.setCursor(11, 1);
    lcd.print("^");
  }


  if (screenCursor == 9)
  {
    numberChangeButton(6);

    lcd.setCursor(11, 1);
    lcd.print(" ");
    lcd.setCursor(12, 1);
    lcd.print("^");
    lcd.setCursor(12, 1);
    lcd.print("^");
  }

  if (screenCursor == 10)
  {


    numberChangeButton(7);

    lcd.setCursor(12, 1);
    lcd.print(" ");
    lcd.setCursor(13, 1);
    lcd.print("^");
    lcd.setCursor(13, 1);
    lcd.print("^");
  }

  if (screenCursor == 11)
  {

    numberChangeButton(8);



    lcd.setCursor(13, 1);
    lcd.print(" ");
    lcd.setCursor(14, 1);
    lcd.print("^");
    lcd.setCursor(14, 1);
    lcd.print("^");
  }


  if (screenCursor == 12)
  {

    numberChangeButton(9);


    lcd.setCursor(14, 1);
    lcd.print(" ");
    lcd.setCursor(15, 1);
    lcd.print("^");
    lcd.setCursor(15, 1);
    lcd.print("^");
  }


  if (screenCursor == 13)
  {

    lcd.setCursor(15, 1);
    lcd.print(" ");

    // Saving number entered into memory

    for (int i = 0; i < 10; i++)
    {
      EEPROM.update(i, num[i]);
    }


    lcd.setCursor(0, 1);
    lcd.print("Saved");


  }

  if (screenCursor == 14)
  {


    if (digitalRead(buttonPinDown) == HIGH)
    {

      if (smsInterval != 1)
      {
        smsInterval = smsInterval - 1;
        delay(175);
      }

    }

    if (digitalRead(buttonPinUp) == HIGH)
    {

      if (smsInterval != 60)
      {
        smsInterval = smsInterval + 1;
        delay(175);
      }

    }





    // Saving Interval to ROM
    EEPROM.update(10, smsInterval);


  }


  for (int i = 0; i < 16; i++) {

    if (screenCursor == i + 15)
    {

      lcd.setCursor(i, 0);
      lcd.print("_");

      if (digitalRead(buttonPinDown) == HIGH)
      {

        if (id[i] != 48)
        {



          id[i] = id[i] - 1;

          //          id[i] = 48;


          delay(175);
        }

      }

      if (digitalRead(buttonPinUp) == HIGH)
      {

        if (id[i] != 90)
        {
          id[i] = id[i] + 1;
          delay(175);

          //          id[i] = 90;
        }

      }




    }
  }


  if (screenCursor == 31)
  {


    EEPROM.update(11, id[0]);
    EEPROM.update(12, id[1]);
    EEPROM.update(13, id[2]);
    EEPROM.update(14, id[3]);
    EEPROM.update(15, id[4]);
    EEPROM.update(16, id[5]);
    EEPROM.update(17, id[6]);
    EEPROM.update(18, id[7]);
    EEPROM.update(19, id[8]);
    EEPROM.update(20, id[9]);
    EEPROM.update(21, id[10]);
    EEPROM.update(22, id[11]);
    EEPROM.update(23, id[12]);
    EEPROM.update(24, id[13]);
    EEPROM.update(25, id[14]);
    EEPROM.update(26, id[15]);


  }

  if (screenCursor == 32)
  {


    if (digitalRead(buttonPinDown) == HIGH)
    {

      if (thingspeakInterval != 15)
      {
        thingspeakInterval = thingspeakInterval - 1;
        delay(175);
      }

    }

    if (digitalRead(buttonPinUp) == HIGH)
    {

      if (thingspeakInterval != 9999)
      {
        thingspeakInterval = thingspeakInterval + 1;
        delay(175);
      }

    }





    // Saving Interval to ROM
    EEPROM.update(27, thingspeakInterval);



  }




}

void numberChangeButton(int i)
{

  if (digitalRead(buttonPinDown) == HIGH)
  {

    if (num[i] != 0)
    {
      num[i] = num[i] - 1;
      delay(175);
    }

  }

  if (digitalRead(buttonPinUp) == HIGH)
  {

    if (num[i] != 9)
    {
      num[i] = num[i] + 1;
      delay(175);
    }

  }


}


void idChangeButton(int i)
{

  if (digitalRead(buttonPinDown) == HIGH)
  {

    if (id[i] != 60)
    {
      id[i] = id[i] - 1;
      delay(175);
    }

  }

  if (digitalRead(buttonPinUp) == HIGH)
  {

    if (id[i] != 132)
    {
      id[i] = id[i] + 1;
      delay(175);
    }

  }


}


void tempMonitor() {

  if (temp1 <= LowTemp)
  {
    sendText = 1;
  }

  else if (temp1 >= HighTemp)
  {

    sendText = 1;
  }

  else if (temp2 <= LowTemp)
  {
    sendText = 1;
  }

  else if (temp2 >= HighTemp)
  {
    sendText = 1;
  }

  else {
    sendText = 0;
  }

}
