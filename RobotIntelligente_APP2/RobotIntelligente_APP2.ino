// Copyright (C) 2018  Simone Scaravati, Noah Rosa, Stefano Radaelli


#include <NewPing.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCBundle.h>
#include <OSCBoards.h>

int pinI1=D0;//define I1 port  // GPIO16
int pinI2=D1;//define I2 port  // GPIO5
int speedpin=D2;//define EA(PWM speed regulation)port  // GPIO4
int pinI3=D5;//define I1 port  // GPIO14
int pinI4=D6;//define I2 port  // GPIO12
int speedpin1=D7;//define EA(PWM speed regulation)port // GPIO13

int Rx = 3; //GPIO3

#define TRIGGER D3 // GPIO0
#define ECHO D4 // GPIO2
NewPing sonar(TRIGGER, ECHO, 400);
int dist=0;
int giroD=0;
int giroS=0;

boolean mod=false;
boolean wheels= false;

/*const String ssid="noah-HP-15-Notebook-PC";
const String pass="BJkd9jw4";*/

/*const String ssid="HUAWEI P8 lite 2017";
const String pass="ciaonoah123";*/

const String ssid="FASTWEB-1-Scara";
const String pass="X7PfW3xvpW";

const byte MAX_MSG_SIZE PROGMEM=100;
byte packetBuffer[MAX_MSG_SIZE];  //buffer to hold incoming udp packet
WiFiUDP Udp;

void setup() {
Serial.begin(115200);

pinMode(pinI1,OUTPUT);//define this port as output
pinMode(pinI2,OUTPUT);
pinMode(speedpin,OUTPUT);
pinMode(pinI3,OUTPUT);//define this port as output
pinMode(pinI4,OUTPUT);
pinMode(speedpin1,OUTPUT);
pinMode(D8, INPUT);  // GPIO15
pinMode(Rx, INPUT);  

WiFi.mode(WIFI_STA);
WiFi.begin(ssid.c_str(), pass.c_str());

Serial.print("Connecting...");
while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
}
Serial.println();

Serial.print("Connected, IP address: ");
Serial.println(WiFi.localIP());
// ...fin qui

//setup ethernet part
Udp.begin(8000); // porta default OSC


}
void loop() {
  receiveOSC();
  if(giroS==0 && digitalRead(D8)==1){
  Serial.println("giroS");
  }
  if(giroD==0 && digitalRead(Rx)==1){
  Serial.println("giroD");
  }
  if(mod== true){
    dist = sonar.ping_cm();
      if(dist>=15 || dist==0) {
        avanti();
      } 
      if(dist<15 && dist!=0) {
        indietroDestra();
        delay(850);
        stopp();
        delay(5);
        dist=sonar.ping_cm();
        indietroSinistra();
        delay(1700);
        stopp();
        delay(5);
        if((dist>sonar.ping_cm() && sonar.ping_cm()!=0) || (dist==0 && sonar.ping_cm()!=0)) {
          indietroDestra();
          delay(1700);
        }
      }
  }
  giroS= digitalRead(D8);
  giroD=digitalRead(Rx);
}

void receiveOSC() {
    //Serial.println(btnState);

    //OSCBundle bundleIN; // NO
    OSCMessage messageIN;
    int size;
    if( (size = Udp.parsePacket())>0) {
        //pingLed();
        //oscLastPacketTime=millis();
        //Serial.println("received");
        //Serial.printf("Received %d bytes from %s, port %d\n", size, Udp.remoteIP().toString().c_str(), Udp.remotePort());
        
        Udp.read(packetBuffer,size);
        messageIN.fill(packetBuffer,size);
        
        if(!messageIN.hasError()) {
            Serial.println("----------->  no error");
            //Serial.println();
            messageIN.send(Serial);
            if(mod == false){
              messageIN.route("/UP", avanti);
              messageIN.route("/DOWN", indietro);
                if(wheels == false){
                  messageIN.route("/LEFT", indietroSinistra);
                  messageIN.route("/RIGHT", indietroDestra);
                }else{
                  messageIN.route("/LEFT", avantiSinistra);
                  messageIN.route("/RIGHT", avantiDestra);
                }
              messageIN.route("/STOP", stopp);
              messageIN.route("/ai", AI);
              messageIN.route("/4x4", wheelsON);
              messageIN.route("/2x4", wheelsOFF);
            }else{
              messageIN.route("/manual", manual);
            }       
        }
        Udp.flush();
    }
}

void AI(OSCMessage &messageIN, int addrOffset){
  mod= true;
}

void manual(OSCMessage &messageIN, int addrOffset){
  mod= false;
  stopp();
}

void wheelsOFF(OSCMessage &messageIN, int addrOffset){
  wheels=true;
}

void wheelsON(OSCMessage &messageIN, int addrOffset){
  wheels=false;
}

void avanti(OSCMessage &messageIN, int addrOffset) {
  analogWrite(speedpin,1023);//input a value to set the speed
  digitalWrite(pinI2,LOW);
  digitalWrite(pinI1,HIGH);
  analogWrite(speedpin1,1023);//input a value to set the speed
  digitalWrite(pinI4,LOW);
  digitalWrite(pinI3,HIGH);
}

void avanti() {
  analogWrite(speedpin,1023);//input a value to set the speed
  digitalWrite(pinI2,LOW);
  digitalWrite(pinI1,HIGH);
  analogWrite(speedpin1,1023);//input a value to set the speed
  digitalWrite(pinI4,LOW);
  digitalWrite(pinI3,HIGH);
}

void indietro(OSCMessage &messageIN, int addrOffset) {
  analogWrite(speedpin,1023);//input a value to set the speed
  digitalWrite(pinI2,HIGH);
  digitalWrite(pinI1,LOW);
  analogWrite(speedpin1,1023);//input a value to set the speed
  digitalWrite(pinI4,HIGH);
  digitalWrite(pinI3,LOW);
}

void indietro() {
  analogWrite(speedpin,1023);//input a value to set the speed
  digitalWrite(pinI2,HIGH);
  digitalWrite(pinI1,LOW);
  analogWrite(speedpin1,1023);//input a value to set the speed
  digitalWrite(pinI4,HIGH);
  digitalWrite(pinI3,LOW);
}

void avantiDestra(OSCMessage &messageIN, int addrOffset) {
  analogWrite(speedpin,1023);//input a value to set the speed
  digitalWrite(pinI2,LOW);
  digitalWrite(pinI1,HIGH);
  analogWrite(speedpin1,128);//input a value to set the speed
  digitalWrite(pinI4,LOW);
  digitalWrite(pinI3,HIGH);
}

void indietroDestra(OSCMessage &messageIN, int addrOffset) {
  analogWrite(speedpin,1023);//input a value to set the speed
  digitalWrite(pinI2,LOW);
  digitalWrite(pinI1,HIGH);
  analogWrite(speedpin1,1023);//input a value to set the speed
  digitalWrite(pinI4,HIGH);
  digitalWrite(pinI3,LOW);
}

void indietroDestra() {
  analogWrite(speedpin,1023);//input a value to set the speed
  digitalWrite(pinI2,LOW);
  digitalWrite(pinI1,HIGH);
  analogWrite(speedpin1,1023);//input a value to set the speed
  digitalWrite(pinI4,HIGH);
  digitalWrite(pinI3,LOW);
}

void avantiSinistra(OSCMessage &messageIN, int addrOffset) {
  analogWrite(speedpin, 128);//input a value to set the speed
  digitalWrite(pinI2,LOW);
  digitalWrite(pinI1,HIGH);
  analogWrite(speedpin1,1023);//input a value to set the speed
  digitalWrite(pinI4,LOW);
  digitalWrite(pinI3,HIGH);
}

void indietroSinistra(OSCMessage &messageIN, int addrOffset) {
  analogWrite(speedpin,1023);//input a value to set the speed
  digitalWrite(pinI2,HIGH);
  digitalWrite(pinI1,LOW);
  analogWrite(speedpin1,1023);//input a value to set the speed
  digitalWrite(pinI4,LOW);
  digitalWrite(pinI3,HIGH);
}

void indietroSinistra() {
  analogWrite(speedpin,1023);//input a value to set the speed
  digitalWrite(pinI2,HIGH);
  digitalWrite(pinI1,LOW);
  analogWrite(speedpin1,1023);//input a value to set the speed
  digitalWrite(pinI4,LOW);
  digitalWrite(pinI3,HIGH);
}

void stopp(OSCMessage &messageIN, int addrOffset) {
  analogWrite(speedpin,0);//input a value to set the speed
  digitalWrite(pinI2,LOW);
  digitalWrite(pinI1,HIGH);
  analogWrite(speedpin1,0);//input a value to set the speed
  digitalWrite(pinI4,LOW);
  digitalWrite(pinI3,HIGH);
}

void stopp() {
  analogWrite(speedpin,0);//input a value to set the speed
  digitalWrite(pinI2,LOW);
  digitalWrite(pinI1,HIGH);
  analogWrite(speedpin1,0);//input a value to set the speed
  digitalWrite(pinI4,LOW);
  digitalWrite(pinI3,HIGH);
}
