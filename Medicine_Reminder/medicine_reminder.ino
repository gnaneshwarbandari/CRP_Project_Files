#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#define SSD1306_LCDHEIGHT 64
void callback(char* topic, byte* payload, unsigned int payloadLength);
const char* ssid = "Smartbridge_Robotics-2.4GHz";
const char* password = "robotics";
#define ORG "hj5fmy"
#define DEVICE_TYPE "NodeMCU"
#define DEVICE_ID "12345"
#define TOKEN "12345678"
#define OLED_ADDR   0x3C
Adafruit_SSD1306 display(-1);
//-------- Customise the above values --------
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
const int buzzer =  D5;
String medicine;
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
  Serial.begin(115200);
  Serial.println();
  wifiConnect();
  mqttConnect();
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(40,25);
  display.println("MEDICINE");
  display.setCursor(40,35);
  display.println("REMINDER");
  display.display();
  pinMode(buzzer, OUTPUT);
  pinMode(D3,INPUT);
  pinMode(D4,INPUT);
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
    medicine += (char)payload[i];
  }
  
  Serial.print("medicine: "+medicine );
  if(medicine != "xx"){

  display.clearDisplay();
// display a line of text
  display.setTextSize(1); //defines th text size ( range is 1 to 8 )
  display.setTextColor(WHITE); //defines the text colour
  display.setCursor(0,10); //sets position of cursor - row,coloumn
  display.println("Time to Take the medicine: ");
  display.println(medicine);
  // update display with all of the above graphics
  display.display();
  digitalWrite(buzzer, HIGH);
  delay(2000);
  digitalWrite(buzzer, LOW);
  delay(200);  
  }
}

void publishData() 
{
   int b1=digitalRead(D3);
   int b2=digitalRead(D4);
   String payload = "{\"d\":{\"b1\":";
  payload +=b1;
  payload +=",""\"b2\":";
  payload +=b2;
  payload += "}}";
  Serial.print("\n");
  Serial.print("Sending payload: "); Serial.println(payload);
  if (client.publish(publishTopic, (char*) payload.c_str())) {
    Serial.println("Publish OK");
  } else {
    Serial.println("Publish FAILED");
  }
}
