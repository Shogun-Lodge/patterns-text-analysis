/*
  Tweet Analyser
  
  JSON in via TCP
  
  Analyser data out via OSC/UDP
  
  Marcus Cook
  
  13/04/2015

  EtherMega hence 24AA025E48 MAC Chip code (I2C stuff)

*/

#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>    
#include <OSCBundle.h>
#include <ArduinoJson.h>

#define I2C_ADDRESS 0x50  // THese are for the 24AA025E48 MAC Chip
#include <Wire.h>

EthernetUDP Udp;

//the Arduino's IP
IPAddress ip(192, 168, 10, 15);
//destination IP
IPAddress outIp(192, 168, 10, 11);
const unsigned int outPort = 9000;

byte mac[] = {  
  0x24, 0xAA, 0xBE, 0xEF, 0xFE, 0xED }; // you can find this written on the board of some Arduino Ethernets or shields

int txtLngth = 0;
int numbers = 0;
int letters = 0;
int spaces = 0;
int lower = 0;
int upper = 0;
int punct = 0;
int others = 0;

char tcpIn = 'A';
String dataBuf = "";
String userOSC = "";
String idOSC = "";
char json[300] = "";

//const char* user[] = {""};
//const char* id[] = {""};

int oscFlag = 0;
int serialFlag = 0;
int tcpFlag = 0;
int textAly = 0;
int feels = 0;

float para1 = 0;
float para2 = 0;
float para3 = 0;
float para4 = 0;
float para5 = 0;
float para6 = 0;
float para7 = 0;
float para8 = 0;
float para9 = 0;

EthernetServer server = EthernetServer(8000);

void setup() {
  Wire.begin();
  mac[0] = readRegister(0xFA);
  mac[1] = readRegister(0xFB);
  mac[2] = readRegister(0xFC);
  mac[3] = readRegister(0xFD);
  mac[4] = readRegister(0xFE);
  mac[5] = readRegister(0xFF);
  Ethernet.begin(mac,ip);
  Udp.begin(8888);
  
  Serial.begin(57600);
  
  // start listening for clients
  server.begin();

}

void loop(){
  // if an incoming client connects, there will be bytes available to read:
  //Serial.println("Waiting...");
  EthernetClient client = server.available();
  if (client == true) {
    tcpFlag = 1;
    oscFlag = 0;
    tcpIn = client.read();
    //dataBuf += tcpIn;
    //Serial.print("feels = ");
    //Serial.println(feels);
    json[feels] = tcpIn;
    feels++;
    //Serial.println(x);  
  } 
 // else if (client != true && flag == 1){
 //   Serial.println(data);
 //   data = "";  
 //   tcpFlag = 0;
 // }  
  
    while (client != true && tcpFlag == 1){
  //while (Serial.available() > 0) { // add any incoming characters to the String:
    //int textAly = Serial.read();
    //json = dataBuf;
    feels = 0;
    Serial.println("--------");
    Serial.println("");
    Serial.println("TCP OUT!");
    Serial.println("");
    Serial.println(dataBuf);
   // Serial.println(json);
    StaticJsonBuffer<300> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(json);
    
    if (!root.success()) {
      Serial.println("parseObject() failed");
      return;
    }

    const char* text = root["text"];
    const char* user = root["user"];
    const char* id = root["id"];

    Serial.print("text = ");
    Serial.println(text);
    Serial.print("user = ");
    Serial.println(user);
    Serial.print("id = ");
    Serial.println(id);
    
    dataBuf = text;
    userOSC = user;
    idOSC = id;
    
    int len = dataBuf.length();
    txtLngth = len;
      for(int i = 0; i < len; i++){
      textAly = dataBuf.charAt(i);
    
        //txtLngth++;  
        if(isDigit(textAly)) {
        numbers++;
        }
        if(isAlpha(textAly)) {
          letters++;
        }
        if(isSpace(textAly)) {
          spaces++;
        }
        if(isLowerCase(textAly)) {
          lower++;
        }
        if(isUpperCase(textAly)) {
          upper++;
        }
        if(isPunct(textAly)) {
          punct++;
        }
        else{
          others++;
        }
      }
   // serialFlag = 1;
   tcpFlag = 0;
   oscFlag = 1;
   textAly = 0;
   dataBuf = "";
  Serial.println("");
  Serial.println("TEXT ANALYSED");
  Serial.println("");
  Serial.print("Letters = ");
  Serial.println(letters);
  Serial.println("");
  }
 
   if(oscFlag == 1){
     
     para1 = txtLngth;
     para2 = numbers;
     para3 = letters;
     para4 = spaces;
     para5 = lower;
     para6 = upper;
     para7 = punct;
     para8 = others;
   
     OSCBundle bndl;
   
      bndl.add("/para1").add(para1);
      bndl.add("/para2").add(para2);
      bndl.add("/para3").add(para3);
      bndl.add("/para4").add(para4);
      bndl.add("/para5").add(para5);
      bndl.add("/para6").add(para6);
      bndl.add("/para7").add(para7);
      bndl.add("/para8").add(para8);
      bndl.add("/user").add(userOSC);
      bndl.add("/id").add(idOSC);
      
      Udp.beginPacket(outIp, outPort);
          bndl.send(Udp); // send the bytes to the SLIP stream
      Udp.endPacket(); // mark the end of the OSC Packet
      bndl.empty(); // empty the bundle to free room for a new one
      
      txtLngth = 0;
      numbers = 0;
      letters = 0;
      spaces = 0;
      lower = 0;
      upper = 0;
      punct = 0;
      others = 0;
     
      oscFlag = 0;
      Serial.println("OSC SENT FLAGS SET");
      Serial.println("");
      Serial.println("--------");
      Serial.println("");
    //  serialFlag = 0;
   }    
}

// MAC address from 24AA025E48 MAC Chip via I2C
byte readRegister(byte r){
  unsigned char v;
  Wire.beginTransmission(I2C_ADDRESS);
  Wire.write(r);
  Wire.endTransmission();
  
  Wire.requestFrom(I2C_ADDRESS,1);
  while(!Wire.available()){
    // Wait
  }
  v = Wire.read();
  return v;
}
