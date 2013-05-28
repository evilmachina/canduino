#include <Servo.h>
#include <SPI.h>
#include <Ethernet.h>
#include "config.h"

//candymachine
Servo candyServo;
int candyServoPin = 3;

int ledPin = 9; 


EthernetClient client;

boolean isConnected = false;
char sid[22];
long lastHeartbeat = 0;

void setup() {
  
  Serial.begin(57600);
  Serial.print("Started...");
  
   pinMode(ledPin, OUTPUT);
  if (Ethernet.begin(mac) == 0) {
    //Serial.print("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    while(true);
  }
  
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print("."); 
  }
  ////Serial.print();
}


boolean Connect(){
  if(client.connected()) return true;
  
  if (client.connect(serverName, port)) {
    //Serial.print("connected");
    return true;   
  } 
  else { 
    //Serial.print("connection failed =(");
    return false;
  }
}

void sendHandshake(){
    client.println("GET /socket.io/1/ HTTP/1.1");
    client.println("Connection: keep-alive");
    client.println();
}

boolean readHandshake(){ 
  client.setTimeout(20);
  boolean okHedder = false;
  boolean gotKey = false;
  boolean handshaked = false;
  
   while(true){ 
     if (client.available()) { 
        if(gotKey){
          if(!client.find("0")) return false;
          //Serial.print("handshake authorized");
          handshaked = true;
          return true;
         }
        
         if(okHedder &&  !gotKey){
           if(!client.find("\r\n\r\n")) return false;
           if(!client.find("\r\n")) return false;
           
           int i = client.readBytesUntil(':', sid, 22);
           if(i < 15) return false;
           gotKey = true;
           Serial.print("sid:");
           //Serial.print(sid);
            //15:25
            //15 =heartbeat timout
            //25 connection timeout       
         }
      
        if(!okHedder){
          if(!client.find("HTTP/1.1 200 OK")) return false;
          okHedder = true;
         }
     }
   }
   
   return true;
}

void sendUpgradeToWebSoocketRequest(){
    client.print("GET ");
    client.print("/socket.io/1/websocket/");
    client.print(sid);
    client.println(" HTTP/1.1");
    
    client.println("Upgrade: websocket");
    
    client.println("Connection: Upgrade");
    
    client.print("Host: ");
    client.print(serverName);
    client.print(":");
    client.println(port);
    
    client.println("Sec-WebSocket-Key: jO302F6nZoZtd16ARhJXYw==");
    
    client.print("Sec-WebSocket-Origin: http://");
    client.print(serverName);
    client.print(":");
    client.println(port);
    
    client.println("Sec-WebSocket-Version: 8");
    client.println();
    //Serial.print("sent upgrade request");
   
}

boolean socketUpgraded(){
  delay(200);
  client.setTimeout(1000);
  while(true)
  {
     if (client.available()) { 
       if(client.find("HTTP/1.1 101 Switching Protocols")){
         //Serial.print("wee");
         return true;
       }
       else{
         //Serial.print("timed out");
         return false;
       }
       //if(client.find("1::")) return true;
       //if(client.find("HTTP/1.1 101")) return true;
      /*if(!client.find("Upgrade: websocket")) return false;
       if(!client.find("Connection: Upgrade")) return false;
       if(!client.find("Sec-WebSocket-Accept:")) return false;
       //use key UJhXubSOL7WqqaMSf1tbjwQ5FIw=")  ??????
      if(client.find("\r\n\r\n")) return true;
      */ 
       ////Serial.print("dodo");
      // return true;
     }
   }
   return true;
}


void readOrDie()
{
   while(true){
      if (client.available()) {
      char c = client.read();
      Serial.print(c);
      }
      disconnect();
    }
}

void connectToDeviceChannel(){
  //Serial.print("blob");
  //client.println();
  client.write((byte)129); //start
  client.write((byte)0x09); //length
  /*client.write((byte)49);
  client.write((byte)58);
  client.write((byte)58);
  */
  client.print("1::/thing"); //send connection string ?my=
  //client.write((byte)3);  
}

boolean connectToServer(){
   if(!Connect()) return false;
   sendHandshake();
   if(!readHandshake()) return false;
   sendUpgradeToWebSoocketRequest();
   delay(1000);
   //if(!socketUpgraded()) return false; 
   connectToDeviceChannel();
   sendSetup();
   digitalWrite(ledPin, HIGH);
   return true;
}

void disconnect(){
  if (!client.connected()) {
      //Serial.print();
      //Serial.print("disconnecting.");
      client.stop();
      //reconect
      while(true);
    }
}

boolean timeToSendHeartbeat(){
  return millis() - lastHeartbeat > 14000;
}

void sendHeartbeat(){
  //Serial.print("sending heartbeat");
  client.write((byte)129); //start
  client.write((byte)0x03); //length
  client.print("2::");
  
  lastHeartbeat = millis();
}

void sendSetup(){
  //Serial.print("sending setup");
  client.write((byte)0x81); //start
  client.write((byte)0x7e); //??
  client.write((byte)((settings.length() >> 8) & 0xff)); //??
  client.write((byte)(settings.length() & 0xff) );//183); //lengt
  client.print(settings);  
//client.print("5::/thing:{\"name\":\"setup\",\"args\":[{\"name\":\"Candy machine\",\"id\":\"85af8fc7-9c8c-4896-ae5a-24f1f1609a6b\",\"position\":{\"lat\":55.5972022,\"lon\":12.9793127},\"functions\":[{\"button\":\"Give\"}]}]}");
  }

void readIncomingData(){
     
    char messageLength = client.read();  //lengt?
    if(messageLength == (char)0x7e){ //start byte
       if(client.peek() == (char)0x00){
         client.read();
         messageLength = client.read();
       }      
    }
    Serial.print("Messge length: ");
    //Serial.print((byte)messageLength);
    char command[(byte)messageLength];
    client.readBytes(command, (byte)messageLength);
    Serial.print("resived command: ");
    //Serial.print(command);  
    //if(command[0] == 1);
    //else if(command[0] == 2);
    
    if(command[0] == '5')
    {
      releaseCandy();
    }
    
  
}

void releaseCandy(){
  candyServo.attach(candyServoPin);
  candyServo.write(150);
  delay(2000);
  candyServo.detach();
  //Serial.print("release");
}

void loop() {
  
  
  if(!isConnected)
    isConnected = connectToServer();
  if(isConnected && timeToSendHeartbeat())  
     sendHeartbeat(); //evry 15 sec 2::
  
  if(client.available()) {
      char c = client.read();  //lisen for 129,lengt,5::
      if(isConnected && c == (char)0x81){ //start byte
        //Serial.print("incoming comand!");
        readIncomingData();
       }
      
       Serial.print(c);
   }
      
   if(!isConnected || !client.connected())
   {
     digitalWrite(ledPin, LOW);
     client.stop();
     //Serial.print("trying to reconnect in 10");
     delay(10000);
     isConnected = false;
   }
}


