#include <ESP8266WiFi.h>     //Include Esp library
#include <WiFiClient.h>      //Include Wifi library
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>        //include RFID library

#define SS_PIN D8 
#define RST_PIN D3
#define RedLed D1
#define BlueLed D2

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

/* Set these to your desired credentials. */
const char *ssid = "VLVG51";  //ENTER YOUR WIFI SETTINGS 
const char *password = "vlvgsince19048888306302";

//Web/Server address to read/write from 
const char *host = "192.168.0.113";   //IP address of server 

String getData ,Link;
String CardID="";

void setup() {
  delay(1000);
  Serial.begin(115200);
  SPI.begin();  // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card

  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  Serial.print("Connecting to ");
  Serial.print(ssid);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.println("Connected");
  Serial.print("NodeMCU IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP

  pinMode(RedLed,OUTPUT);
  pinMode(BlueLed,OUTPUT);
  
}

void loop() {
  if(WiFi.status() != WL_CONNECTED){
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    Serial.print("Reconnecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
    Serial.println("");
    Serial.println("Connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());  //IP address assigned to your ESP
  }
  
  //look for new card
   if ( ! mfrc522.PICC_IsNewCardPresent()) {
  return;//got to start of loop if there is no card present
 }
 // Select one of the cards
 if ( ! mfrc522.PICC_ReadCardSerial()) {
  return;//if read card serial(0) returns 1, the uid struct contians the ID of the read card.
 }

 for (byte i = 0; i < mfrc522.uid.size; i++) {
     CardID += mfrc522.uid.uidByte[i];
}
  
  HTTPClient http;    //Declare object of class HTTPClient
  
  //GET Data
  getData = "?CardID=" + CardID;  //Note "?" added at front
  Link = "http://192.168.0.113/loginsystem/postdemo.php" + getData; 
  
  http.begin(Link);
  
  int httpCode = http.GET();            //Send the request
  delay(10);
  String payload = http.getString();    //Get the response payload
  
  Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload
  Serial.println(CardID);     //Print Card ID
  
  if(payload == "login"){
    digitalWrite(RedLed,HIGH);
    Serial.println("red on");
    delay(500);  
  }
  else if(payload == "logout"){
    digitalWrite(BlueLed,HIGH);
    Serial.println("Blue on");
    delay(500);  
  }
  else if(payload == "succesful" || payload == "Cardavailable"){
    digitalWrite(BlueLed,HIGH);
    digitalWrite(RedLed,HIGH);
    delay(500);  
  }
  delay(500);
  
  CardID = "";
  getData = "";
  Link = "";
  http.end();  //Closing our connection
  
  digitalWrite(RedLed,LOW);
  digitalWrite(BlueLed,LOW);
}
//===========================THE-END================================
