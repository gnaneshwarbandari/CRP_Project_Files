#include <SPI.h>//include the SPI bus library
#include <MFRC522.h>//include the RFID reader library
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Servo.h>
Servo myservo;
#define SSD1306_LCDHEIGHT 64 //128*64
#define OLED_ADDR   0x3C
Adafruit_SSD1306 display(-1);
#define SS_PIN D3  //slave select pin
#define RST_PIN D4  //reset pin
MFRC522 mfrc522(SS_PIN, RST_PIN);        // instatiate a MFRC522 reader object.
MFRC522::MIFARE_Key key;//create a MIFARE_Key struct named 'key', which will hold the card information
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
void publishData();
String s,s1;
void setup() {
  Serial.begin(9600); 
  wifiConnect();
  mqttConnect();// Initialize serial communications with the PC
  SPI.begin();
  pinMode(D8,OUTPUT);  
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);// Init SPI bus
  mfrc522.PCD_Init();        // Init MFRC522 card (in case you wonder what PCD means: proximity coupling device)
  Serial.println("Scan a MIFARE Classic card");
  for (byte i = 0; i < 6; i++) {
          key.keyByte[i] = 0xFF;//keyByte is defined in the "MIFARE_Key" 'struct' definition in the .h file of the library
  }
  myservo.attach(D0);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(25,25);
  display.print("SMART SECURITY");
  display.setCursor(25,40);
  display.print("FOR INDUSTRIES");
  //display.println(t);
  display.display();
  delay(3000);
  myservo.write(0);
}
int block=2;//this is the block number we will write into and then read. Do not write into 'sector trailer' block, since this can make the block unusable.
int block1=4;
//byte blockcontent[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//all zeros. This can be used to delete a block.
byte readbackblock[18];//This array is used for reading out a block. The MIFARE_Read method requires a buffer that is at least 18 bytes to hold the 16 bytes of a block.
byte readbackblock1[18];
void loop(){
  read();
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
  if(message=="12345"){
    digitalWrite(D8,HIGH);
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(28,20);
    display.print(message);
    display.setTextSize(1);
    display.setCursor(15,40);
    display.print("Leave the room");
    //display.println(t);
    display.display();
    delay(4000);
    digitalWrite(D8,LOW);
  }
  else if(message=="67890"){
    digitalWrite(D8,HIGH);
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(28,20);
    display.print(message);
    display.setTextSize(1);
    display.setCursor(15,40);
    display.print("Leave the room");
    //display.println(t);
    display.display();
    delay(4000);
    digitalWrite(D8,LOW);
  }
  message="";
}

void read()
{
    int pos;
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(15,20);
    display.print("Scan the card");
    //display.println(t);
    display.display();
  if ( ! mfrc522.PICC_IsNewCardPresent()) {//if PICC_IsNewCardPresent returns 1, a new card has been found and we continue
    return;//if it did not find a new card is returns a '0' and we return to the start of the loop
  }

  if ( ! mfrc522.PICC_ReadCardSerial()) {//if PICC_ReadCardSerial returns 1, the "uid" struct (see MFRC522.h lines 238-45)) contains the ID of the read card.
    return;//if it returns a '0' something went wrong and we return to the start of the loop
  }    
   //Serial.println("card selected");    
   readBlock(block, readbackblock);//read the block back
   //Serial.print("read block: ");
   for (int j=0 ; j<16 ; j++)//print the block contents
   {
     Serial.write (readbackblock[j]);//Serial.write() transmits the ASCII numbers as human readable characters to serial monitor
   }
   //Serial.println("");

   //byte data to string conversion
   s=String((char *)readbackblock);
   //Serial.println("Read data from block1");
   //Serial.println(s);
   
   readBlock(block1, readbackblock1);//read the block back
   //Serial.print("read block: ");
   for (int j=0 ; j<16 ; j++)//print the block contents
   {
     Serial.write (readbackblock1[j]);//Serial.write() transmits the ASCII numbers as human readable characters to serial monitor
   }
   Serial.println("");
   s1=String((char *)readbackblock1);
   //Serial.println("Read data from block2");
   //Serial.println(s1);
   if(s=="Smartbridge"){
    if(s1=="12345" or s1=="67890"){
    Serial.print("Access! Welcome: ");
    Serial.println(s1);
    myservo.write(180);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(15,22);
    display.print("Access! Welcome");
    display.setTextSize(2);
    display.setCursor(35,40);
    display.print(s1);
    //display.println(t);
    display.display();
    delay(3000);
    String payload = "{""\"name\":\"";
    payload += s1;
    payload += "\"}";
    Serial.print("\n");
    Serial.print("Sending payload: "); Serial.println(payload);
    if (client.publish(publishTopic, (char*) payload.c_str())) {
      Serial.println("Publish OK");
      } 
      else {
        Serial.println("Publish FAILED");
        }
    myservo.write(0);
   }
   else{
    Serial.println("Unauthorized Access");
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(5,32);
    display.print("Unauthorized Access");
    //display.println(t);
    display.display();
    delay(3000);
   }
   }
   else{
    Serial.println("Detected wrong Organization ID");
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(15,30);
    display.print("Detected wrong");
    display.setCursor(15,45);
    display.print("Organization ID");
    //display.println(t);
    display.display();
    delay(3000);
   }
   Serial.println();
   mfrc522.PICC_HaltA();
    // Stop encryption on PCD
   mfrc522.PCD_StopCrypto1();        
}
      
int writeBlock(int blockNumber, byte arrayAddress[]) 
{
  //this makes sure that we only write into data blocks. Every 4th block is a trailer block for the access/security info.
  int largestModulo4Number=blockNumber/4*4;
  int trailerBlock=largestModulo4Number+3;//determine trailer block for the sector
  if (blockNumber > 2 && (blockNumber+1)%4 == 0){Serial.print(blockNumber);Serial.println(" is a trailer block:");return 2;}//block number is a trailer block (modulo 4); quit and send error code 2
  Serial.print(blockNumber);
  Serial.println(" is a data block:");
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
         Serial.print("PCD_Authenticate() failed: ");
         Serial.println(mfrc522.GetStatusCodeName((MFRC522::StatusCode)status));
         return 3;//return "3" as error message
  }
  status = mfrc522.MIFARE_Write(blockNumber, arrayAddress, 16);//valueBlockA is the block number, MIFARE_Write(block number (0-15), byte array containing 16 values, number of bytes in block (=16))
  if (status != MFRC522::STATUS_OK) {
           Serial.print("MIFARE_Write() failed: ");
           Serial.println(mfrc522.GetStatusCodeName((MFRC522::StatusCode)status));
           return 4;//return "4" as error message
  }
  Serial.println("block was written");
}


int readBlock(int blockNumber, byte arrayAddress[]) 
{
  int largestModulo4Number=blockNumber/4*4;
  int trailerBlock=largestModulo4Number+3;//determine trailer block for the sector
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
         Serial.print("PCD_Authenticate() failed (read): ");
         Serial.println(mfrc522.GetStatusCodeName((MFRC522::StatusCode)status));
         return 3;//return "3" as error message
  }
  byte buffersize = 18;//we need to define a variable with the read buffer size, since the MIFARE_Read method below needs a pointer to the variable that contains the size... 
  status = mfrc522.MIFARE_Read(blockNumber, arrayAddress, &buffersize);//&buffersize is a pointer to the buffersize variable; MIFARE_Read requires a pointer instead of just a number
  //Serial.println(" ");
  //Serial.println("Read Data");
  if (status != MFRC522::STATUS_OK) {
          Serial.print("MIFARE_read() failed: ");
          Serial.println(mfrc522.GetStatusCodeName((MFRC522::StatusCode)status));
          return 4;//return "4" as error message
  }
  //Serial.println("block was read");
}
