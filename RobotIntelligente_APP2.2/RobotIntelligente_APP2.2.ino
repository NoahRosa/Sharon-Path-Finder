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
int powerS = 980;
int powerD = 1020;
#define TRIGGER D3 // GPIO0
#define ECHO D4 // GPIO2
NewPing sonar(TRIGGER, ECHO, 400);
int dist=0;
int giroD=0;
int giroS=0;
int modalita = 0;

boolean mod=false;
boolean wheels= false;

const String ssid="noah-HP-15-Notebook-PC";
const String pass="BJkd9jw4";

/*const String ssid="HUAWEI P8 lite 2017";
const String pass="ciaonoah123";*/

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
pinMode(3, INPUT);
attachInterrupt(digitalPinToInterrupt(D8), girS, RISING);
attachInterrupt(digitalPinToInterrupt(3), girD, RISING);

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

void girS(){
  //Serial.println("giroS");
  giroS++;
  }
  
void girD(){
  //Serial.println("giroD");
  giroD++;
  }




void loop() {
  receiveOSC();
  /*if(giroS==0 && digitalRead(D8)==1){
  Serial.println("giroS");
  }
  if(giroD==0 && digitalRead(3)==1){
  Serial.println("giroD");
  }*/
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
  switch(modalita){
    case 0: stopp(); break;
    case 1: avanti(); break;
    case 2: indietro(); break;
    case 3: destra(); break;
    case 4: sinistra(); break;
  }


  if (giroS>giroD){
    if (powerD<1020)
      powerD+=1;
    else
      powerS-=1;
    }
  else if (giroD>giroS){
    if (powerS<1020)
      powerS+=1;
    else
      powerD-=1;
    }
    if (powerS<970)
      powerS=970;
    if (powerD<970)
      powerD=970;
    Serial.print(giroS);
    Serial.print(",");
    Serial.print(giroD);
    Serial.print(",");
    Serial.print(powerS);
    Serial.print(",");
    Serial.println(powerD);
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
                  messageIN.route("/LEFT", avantiSinistra);
                  messageIN.route("/RIGHT", avantiDestra);
                }else{
                  messageIN.route("/LEFT", indietroSinistra);
                  messageIN.route("/RIGHT", indietroDestra);
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
  modalita=1;
}
void avanti() {
  analogWrite(speedpin,powerS);//input a value to set the speed
  digitalWrite(pinI2,LOW);
  digitalWrite(pinI1,HIGH);
  analogWrite(speedpin1,powerD);//input a value to set the speed
  digitalWrite(pinI4,LOW);
  digitalWrite(pinI3,HIGH);
}


void indietro(OSCMessage &messageIN, int addrOffset) {
  /*analogWrite(speedpin,1023);//input a value to set the speed
  digitalWrite(pinI2,HIGH);
  digitalWrite(pinI1,LOW);
  analogWrite(speedpin1,1023);//input a value to set the speed
  digitalWrite(pinI4,HIGH);
  digitalWrite(pinI3,LOW);*/
  modalita= 2;
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
  modalita=3;
}
void avantiDestra() {
  analogWrite(speedpin,1023);//input a value to set the speed
  digitalWrite(pinI2,LOW);
  digitalWrite(pinI1,HIGH);
  analogWrite(speedpin1,300);//input a value to set the speed
  digitalWrite(pinI4,LOW);
  digitalWrite(pinI3,HIGH);
}


void avantiSinistra(OSCMessage &messageIN, int addrOffset) {
  modalita=4;
}
void avantiSinistra(OSCMessage &messageIN, int addrOffset) {
  analogWrite(speedpin, 128);//input a value to set the speed
  digitalWrite(pinI2,LOW);
  digitalWrite(pinI1,HIGH);
  analogWrite(speedpin1,1023);//input a value to set the speed
  digitalWrite(pinI4,LOW);
  digitalWrite(pinI3,HIGH);
}


void stopp(OSCMessage &messageIN, int addrOffset) {
  /*analogWrite(speedpin,0);//input a value to set the speed
  digitalWrite(pinI2,LOW);
  digitalWrite(pinI1,HIGH);
  analogWrite(speedpin1,0);//input a value to set the speed
  digitalWrite(pinI4,LOW);
  digitalWrite(pinI3,HIGH);*/
  modalita=0;
}

void stopp() {
  analogWrite(speedpin,0);//input a value to set the speed
  digitalWrite(pinI2,LOW);
  digitalWrite(pinI1,HIGH);
  analogWrite(speedpin1,0);//input a value to set the speed
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
