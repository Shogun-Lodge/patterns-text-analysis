/*
  Tweet Analyser
  
  JSON in via TCP
  
  Analyser data out via OSC/UDP
  
  Marcus Cook
  
  Shogun Lodge Services
  
  21/04/2015
  
  Sends shortened 'id' number
  
  !!! Work in progress - Much serial printing / debugging !!!
  
  Yes, needs tidy

  EtherMega hence 24AA025E48 MAC Chip code (I2C stuff) - Currently removed
  
  All spelling mistakes are intentional and quite witty

*/

#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>    
#include <OSCBundle.h>
#include <ArduinoJson.h>

//#define I2C_ADDRESS 0x50  // These are for the 24AA025E48 MAC Chip
//#include <Wire.h>

EthernetUDP Udp;

//the Arduino's IP
IPAddress ip(192, 168, 0, 15);
//destination IP
IPAddress outIp(192, 168, 0, 6);
//port to send OSC out on
const unsigned int outPort = 9000;

byte gateway[] = { 192, 168, 0, 1 };

byte mac[] = { 0x24, 0xAA, 0xBE, 0xEF, 0xFE, 0xED }; // you can find this written on the board of some Arduino Ethernets or shields

//variables of text to be sent via OSC
int txtLngth = 0;
int numbers = 0;
int letters = 0;
int spaces = 0;
int lower = 0;
int upper = 0;
int punct = 0;
int others = 0;
int ascii = 0;
int indiv1 = 0;
int indiv2 = 0;
int indiv3 = 0;
int indiv4 = 0;
int indiv5 = 0;
int indiv6 = 0;
int indiv7 = 0;
int indiv8 = 0;
int indiv9 = 0;
int indiv10 = 0;

//variables needed for text analysis 
char tcpIn = 'A';
String dataBuf = "";
const char* user = "";
const char* id = "";
char idShort[20] = " ";
char json[300] = "";
int textAly = 0;
int charAly = 0;
int feels = 0;

//flags need for tcp and OSC operation
int oscFlag = 0;
int tcpFlag = 0;

//blank OSC varaiables (to be removed later)
float para1 = 0;
float para2 = 0;
float para3 = 0;
float para4 = 0;
float para5 = 0;
float para6 = 0;
float para7 = 0;
float para8 = 0;
float para9 = 0;

//Start TCP Server (listen on port 8000)
EthernetServer server = EthernetServer(8000);

void setup(){
  delay(250);                              // In to try and get EtherTen to send OSC - didn't work
  //Extarct MAC address via I2C
  //Wire.begin();
  //mac[0] = readRegister(0xFA);
  //mac[1] = readRegister(0xFB);
  //mac[2] = readRegister(0xFC);
  //mac[3] = readRegister(0xFD);
  //mac[4] = readRegister(0xFE);
  //mac[5] = readRegister(0xFF);
  
  Ethernet.begin(mac,ip,gateway);                  //Set MAC and local IP
  Udp.begin(8888);                         //Start UPD listening on port 8888
  
  Serial.begin(57600);
  
  // start listening for clients
  server.begin();

}

void loop(){
  // if an incoming client connects, there will be bytes available to read:
  EthernetClient client = server.available();
  if (client == true){
    tcpFlag = 1;
    oscFlag = 0;
    tcpIn = client.read();
    json[feels] = tcpIn;
    feels++;
  } 
  
  while (client != true && tcpFlag == 1){
    feels = 0;
    Serial.println("--------");
    Serial.println("");
    Serial.println("TCP OUT!");
    Serial.println(dataBuf);
    StaticJsonBuffer<300> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(json);
    
    if (!root.success()) {
      Serial.println("parseObject() failed");
      return;
    }

    const char* text = root["text"];
    user = root["user"];
    id = root["id"];

    Serial.print("text = ");
    Serial.println(text);
    Serial.print("user = ");
    Serial.println(user);
    Serial.print("id = ");
    Serial.println(id);
    
    for(int i = 0; i < 12; i++){
     idShort[i] = id[(24+i)];
    }
   
   Serial.print("id Short = ");
   Serial.println(idShort); 
    
    dataBuf = text;                    // Move text into String buffer
    
    int len = dataBuf.length();
    txtLngth = len - 13;               // Text length minus '@videovector '
    
    //Cycyle though text String buffer analysing each char minus '@videovector '
    for(int i = 13; i < len; i++){
      textAly = dataBuf.charAt(i);
      ascii = ascii + textAly;        // Total ASCII value of text
      if(isDigit(textAly)) {        // # of numbers in text
        numbers++;                    
      }
      if(isAlpha(textAly)) {        // # of letters in text
        letters++;                 
      }
      if(isSpace(textAly)) {        // # of spaces in text
        spaces++;                  
      }
      if(isLowerCase(textAly)) {    // # of lower case letters in text
        lower++;
      }
      if(isUpperCase(textAly)) {    // # of upper case letters in text
        upper++;
      }
      if(isPunct(textAly)) {        // # of 'puncuation' in text
        punct++;
      }
      else{                        // # of 'others' in text
        others++;
      }
    }
    
    dataBuf = text;
    for(int i = 12; i < len; i++){
      charAly = dataBuf.charAt(i);
      if(charAly == 'H'){          
        indiv1++;
      }
      if(charAly == 'h'){          
        indiv2++;
      }
      if(charAly == '|'){          
        indiv3++;
      }
      if(charAly == 'z'){          
        indiv4++;
      }
      if(charAly == '9'){          
        indiv5++;
      }
      if(charAly == '{'){          
        indiv6++;
      }
      if(charAly == '?'){          
        indiv7++;
      }
      if(charAly == '%'){          
        indiv8++;
      }
      if(charAly == '~'){          
        indiv9++;
      }
      if(charAly == '/'){          
        indiv10++;
      }
    }
    
   tcpFlag = 0;                      // Clear TCP flag
   oscFlag = 1;                      // Set OSC flag
   textAly = 0;                      // Claera text analyse varaiable
   dataBuf = "";                     // Clear text String buffer
    
    Serial.println("");
    Serial.println("TEXT ANALYSED");
    Serial.println("");
    Serial.print("Text Length = ");
    Serial.println(txtLngth);
    Serial.print("Letters = ");
    Serial.println(letters);
    Serial.print("ASCII = ");
    Serial.println(ascii);
    Serial.print("Upper = ");
    Serial.println(upper);
    Serial.print("Lower = ");
    Serial.println(lower);
    Serial.print("Spaces = ");
    Serial.println(spaces);
    Serial.print("Numbers = ");
    Serial.println(numbers);
    Serial.print("Punctuation = ");
    Serial.println(punct);
    Serial.print("'Others' = ");
    Serial.println(others);
    Serial.print("Indiv1 = ");
    Serial.println(indiv1);
    Serial.print("Indiv2 = ");
    Serial.println(indiv2);
    Serial.print("Indiv3 = ");
    Serial.println(indiv3);
    Serial.print("Indiv4 = ");
    Serial.println(indiv4);
    Serial.print("Indiv5 = ");
    Serial.println(indiv5);
    Serial.print("Indiv6 = ");
    Serial.println(indiv6);
    Serial.print("Indiv7 = ");
    Serial.println(indiv7);
    Serial.print("Indiv8 = ");
    Serial.println(indiv8);
    Serial.print("Indiv9 = ");
    Serial.println(indiv9);
    Serial.print("Indiv10 = ");
    Serial.println(indiv10);
    Serial.println("");
  }
 
  if(oscFlag == 1){               // Are we ready for OSC!
     
    para1 = txtLngth;
    para2 = numbers;
    para3 = letters;
    para4 = spaces;
    para5 = lower;
    para6 = upper;
    para7 = punct;
    para8 = others;
   
    OSCBundle bndl;
   
    //Build OSC bundle
    bndl.add("/para1").add(para1);
    bndl.add("/para2").add(para2);
    bndl.add("/para3").add(para3);
    bndl.add("/para4").add(para4);
    bndl.add("/para5").add(para5);
    bndl.add("/para6").add(para6);
    bndl.add("/para7").add(para7);
    bndl.add("/para8").add(para8);
    bndl.add("/para9").add(ascii);
    bndl.add("/para10").add(indiv1);
    bndl.add("/para11").add(indiv2);
    bndl.add("/para12").add(indiv3); 
    bndl.add("/para13").add(indiv4); 
    bndl.add("/para14").add(indiv5); 
    bndl.add("/para15").add(indiv6); 
    bndl.add("/para16").add(indiv7); 
    bndl.add("/para17").add(indiv8); 
    bndl.add("/para18").add(indiv9);
    bndl.add("/para19").add(indiv10);     
    bndl.add("/user").add(user);
    bndl.add("/id").add(idShort);
      
    //Send OSC bundle out to the world
    Udp.beginPacket(outIp, outPort);    // IP of destiantion and port
        bndl.send(Udp);                 // send the bytes to the SLIP stream
    Udp.endPacket();                    // mark the end of the OSC Packet
    bndl.empty();                       // empty the bundle to free room for a new one
      
    //Clear all text avariables
    txtLngth = 0;
    numbers = 0;
    letters = 0;
    spaces = 0;
    lower = 0;
    upper = 0;
    punct = 0;
    others = 0;
    ascii = 0;
    indiv1 = 0;
    indiv2 = 0;
    indiv3 = 0;
    indiv4 = 0;
    indiv5 = 0;
    indiv6 = 0;
    indiv7 = 0;
    indiv8 = 0;
    indiv9 = 0;
    indiv10 = 0;
     
    oscFlag = 0;                      // Clear OSC flag
      
    Serial.println("OSC SENT FLAGS SET");
    Serial.println("");
    Serial.println("--------");
    Serial.println("");
  }    
}
/*
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
*/
