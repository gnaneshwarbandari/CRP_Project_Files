#include <Wire.h>
#include "DHT.h"
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define SSD1306_LCDHEIGHT 64 //128*64
#define OLED_ADDR   0x3C
Adafruit_SSD1306 display(-1);
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
#define led D8
#define buz D9
#define mot D7
#define e1 D3
#define t1 D4
#define  DHTPIN D7
#define DHTTYPE DHT11  
DHT dht(DHTPIN, DHTTYPE);
void callback(char* topic, byte* payload, unsigned int payloadLength);
const char* ssid = "Smartbridge-2Ghz";
const char* password = "iotgyan4u";
#define ORG "hj5fmy"
#define DEVICE_TYPE "NodeMCU"
#define DEVICE_ID "12345"
#define TOKEN "12345678"
String message;
const char publishTopic[] = "iot-2/evt/data/fmt/json";
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/cmd/home/fmt/String";// cmd  REPRESENT command type AND COMMAND IS TEST OF FORMAT STRING
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);
int publishInterval = 5000; // 30 seconds
long lastPublishMillis;
void publishData();
void setup() {
  wifiConnect();
  mqttConnect();
  Serial.begin(9600);
  Serial.println();
  dht.begin();
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
}
void loop() {
 if (millis() - lastPublishMillis > publishInterval)
  {
    publishData();
    lastPublishMillis = millis();
  }
 
  if (!client.loop()) {
    mqttConnect();
  }
}

void wifiConnect() {
  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("nWiFi connected, IP address: "); Serial.println(WiFi.localIP());
}

void mqttConnect() {
  if (!client.connected()) {
    Serial.print("Reconnecting MQTT client to "); Serial.println(server);
    while (!client.connect(clientId, authMethod, token)) {
      Serial.print(".");
      delay(500);
    }
    initManagedDevice();
    Serial.println();
  }
}

void initManagedDevice() {
  if (client.subscribe(topic)) {
   // Serial.println(client.subscribe(topic));
    Serial.println("subscribe to cmd OK");
  } else {
    Serial.println("subscribe to cmd FAILED");
  }
}

void callback(char* topic, byte* payload, unsigned int payloadLength) {
 
  Serial.print("callback invoked for topic: ");
  Serial.println(topic);
  for (int i = 0; i < payloadLength; i++) {
    //Serial.print((char)payload[i]);
    message += (char)payload[i];
  }
  Serial.println(message);
  if(message == "lighton"){
    digitalWrite(led,HIGH);
  }
  else if(message == "lightoff"){
    digitalWrite(led,LOW);
  }
  else if(message == "motoron"){
    digitalWrite(led,HIGH);
  }
  else if(message == "motoroff"){
    digitalWrite(led,LOW);
  }
}

void publishData()
{
  float s1=tank(t1,e1);
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  int a=analogRead(A0);
  //Serial.print("distance: ");
  //Serial.println(s1);
  display1(t,h,s1,a);
  String payload = "{""\"water\":";
  payload +=s1;
  payload += ",""\"ammonia\":";
  payload +=a;
  payload += ",""\"temperature\":";
  payload +=t;
  payload +=",""\"humidity\":";
  payload +=h;
  payload += "}";
 
  Serial.print("\n");
  Serial.print("Sending payload: "); Serial.println(payload);

  if (client.publish(publishTopic, (char*) payload.c_str())) {
    Serial.println("Publish OK");
  } else {
    Serial.println("Publish FAILED");
  }
}
float tank(unsigned char t,unsigned char e){
  //ultrasoinc start
  digitalWrite(t,LOW);
  delayMicroseconds(5);
  digitalWrite(t,HIGH);
  delayMicroseconds(10);
  digitalWrite(t,LOW);
  float duration = pulseIn(e,HIGH);
  float distance = (duration/2)*0.0343;
  return distance;
}

void display1(float t, float h, float s1, int a){
  String val;
  if(s1<5){
    val="FULL";
  }
  else if(s1>5 && s1<20){
    val="HALF FILLED";
  }
  else if(s1>20){
    val="EMPTY";
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("Temperature: ");
  display.println(t);
  display.print("Humidity: ");
  display.println(h);
  display.print("Water Level: ");
  display.println(val);
  display.print("Ammonia Gas: ");
  display.println(a);
  if(t>35){
    display.setCursor(0,40);
    display.println("Temperature is high");
  }
  else if(h>50){
    display.setCursor(0,40);
    display.println("Humidity is high");
  }
  else if(a>512){
    display.setCursor(0,40);
    display.println("Ammonia Gas is high");
  }
  else if(s1<0){
    display.setCursor(0,40);
    display.println("Fill water in Tank");
  }
  else{
    display.setCursor(0,40);
    display.println("Cool! Everything is Fine");
  }
  Serial.println("Temperature in Control ");
  display.display();
}
