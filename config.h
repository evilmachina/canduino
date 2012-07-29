#ifndef __CONFIG_H__
#define __CONFIG_H__

//Djäkne
//String lat = "55.60599"; // "55.5972022"; 
//String lon = "13.00449"; // "12.9793127"; 

//skvadronsgatan
String lat = "55.5972022"; 
String lon = "12.9793127"; 
//String settings = "5::/thing:{\"name\":\"setup\",\"args\":[{\"name\":\"Candy machine\",\"id\":\"85af8fc7-9c8c-4896-ae5a-24f1f1609a6b\",\"position\":{\"lat\":[lat],\"lon\":[lon]},\"actionControles\":[{\"type\":\"button\",\"name\":\"Give\",\"id\":\"1\"}]}]}";

String settings = "5::/thing:{\"name\":\"setup\",\"args\":[{\"name\":\"Candy machine\",\"id\":\"85af8fc7-9c8c-4896-ae5a-24f1f1609a6b\",\"iconType\": \"candy\",\"quickAction\":{\"type\":\"button\", \"name\":\"Give\", \"id\":\"give\"},\"actionControles\":[{\"type\":\"button\",\"name\":\"Give\",\"id\":\"1\"}]}]}";

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


char serverName[] = "thing.everymote.com";
int port = 80; // 1337;

#endif
