#include <SPI.h>
#include <LoRa.h>
#include "ds3231.h"
#include <WiFi.h>
#include <SD.h>
#include "ThingSpeak.h"
#include "secrets.h"
//thingSpeak chank
//
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;
unsigned long thingTime = 0;
unsigned long displayTime = 0;
unsigned int thingInterval = 20e3;
unsigned int displayInterval = 20e2;

//

OLED_CLASS_OBJ display(OLED_ADDRESS, OLED_SDA, OLED_SCL);

#define WIFI_SSID       "espnet"
#define WIFI_PASSWORD   "12345678"#include <SPI.h>
#include <LoRa.h>
#include "ds3231.h"
#include <WiFi.h>
#include <SD.h>
#include "ThingSpeak.h"
#include "secrets.h"
//thingSpeak chank
//
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;
unsigned long thingTime = 0;
unsigned long displayTime = 0;
unsigned int thingInterval = 20e3;
unsigned int displayInterval = 20e2;

//

OLED_CLASS_OBJ display(OLED_ADDRESS, OLED_SDA, OLED_SCL);

#define WIFI_SSID       "espnet"
#define WIFI_PASSWORD   "12345678"
#define sensor 12



void setup()
{

  Serial.begin(115200);
  //pinMode(sensor, INPUT);
  while (!Serial);

  if (OLED_RST > 0) {
    pinMode(OLED_RST, OUTPUT);
    digitalWrite(OLED_RST, HIGH);
    delay(100);
    digitalWrite(OLED_RST, LOW);
    delay(100);
    digitalWrite(OLED_RST, HIGH);
  }

  display.init();
  display.flipScreenVertically();
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(display.getWidth() / 2, display.getHeight() / 2, LORA_SENDER ? "LoRa Sender" : "LoRa Receiver");
  display.display();
  delay(2000);

  if (SDCARD_CS >  0) {
    display.clear();
    SPIClass sdSPI(VSPI);
    sdSPI.begin(SDCARD_SCLK, SDCARD_MISO, SDCARD_MOSI, SDCARD_CS);
    if (!SD.begin(SDCARD_CS, sdSPI)) {
      display.drawString(display.getWidth() / 2, display.getHeight() / 2, "SDCard  FAIL");
    } else {
      display.drawString(display.getWidth() / 2, display.getHeight() / 2 - 16, "SDCard  PASS");
      uint32_t cardSize = SD.cardSize() / (1024 * 1024);
      display.drawString(display.getWidth() / 2, display.getHeight() / 2, "Size: " + String(cardSize) + "MB");
    }
    display.display();
    delay(2000);
  }


  String info = ds3231_test();
  if (info != "") {
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(display.getWidth() / 2, display.getHeight() / 2, info);
    display.display();
    delay(2000);
  }

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    display.clear();
    Serial.println("WiFi Connect Fail");
    display.drawString(display.getWidth() / 2, display.getHeight() / 2, "WiFi Connect Fail");
    display.display();
    delay(2000);
    esp_restart();
  }
  Serial.print("Connected : ");
  Serial.println(WiFi.SSID());
  Serial.print("IP:");
  Serial.println(WiFi.localIP().toString());
  display.clear();
  display.drawString(display.getWidth() / 2, display.getHeight() / 2, "IP:" + WiFi.localIP().toString());
  display.display();
  delay(2000);

  SPI.begin(CONFIG_CLK, CONFIG_MISO, CONFIG_MOSI, CONFIG_NSS);
  LoRa.setPins(CONFIG_NSS, CONFIG_RST, CONFIG_DIO0);
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  if (!LORA_SENDER) {
    display.clear();
    display.drawString(display.getWidth() / 2, display.getHeight() / 2, "LoraRecv Ready");
    display.display();
  }
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
}#else

  int count = 0;
//*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--
void loop() {

#if  !LORA_SENDER

  if (millis() - displayTime > displayInterval) {
    if (LoRa.parsePacket()) {
      String recv = "";
      while (LoRa.available()) {
        recv += (char)LoRa.read();
      }
      count++;
      String recebidoCru = String(recv);//atribuindo o "texto" recebido a uma string
      char buffer[50];
      strcpy(buffer, recebidoCru.c_str());//copiando o texto recebido e transformando a copia numa string de C

      Serial.printf("recebido %s\n", buffer);

      char *temperatura = strtok(buffer, ",");
      char *umidade = strtok(NULL, ",");
      char *sinal = strtok(NULL, ",");
      char *chuva = strtok(NULL, ",");
      char *umidadeSolo = strtok(NULL, ",");
      Serial.println(temperatura);
      Serial.println(umidade);
      Serial.println(sinal);



      display.clear();
      display.drawString(display.getWidth() / 2, display.getHeight() / 2, recv);
      String info = "[" + String(count) + "]" + "RSSI " + String(LoRa.packetRssi());
      display.drawString(display.getWidth() / 2, display.getHeight() / 2 - 16, info);
      display.display();
      displayTime = millis();
    }

  }//displaymillis
  displayTime = millis();

  if (millis() - thingTime > thingInterval) {
    //thingspeak chank
    //
    // Read in field 1 of the private channel which is a counter

    //
    ThingSpeak.setField(1, LoRa.packetRssi());
    ThingSpeak.setField(2, umidade);
    ThingSpeak.setField(3, temperatura);
    ThingSpeak.setField(4, chuva);
    ThingSpeak.setField(5, random(0, 100));

    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if (x == 200) {
      Serial.println("Channel update successful.");
    }
    else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }

    // change the value



  }


#endif
}//loop
#define sensor 12



void setup()
{

  Serial.begin(115200);
  //pinMode(sensor, INPUT);
  while (!Serial);

  if (OLED_RST > 0) {
    pinMode(OLED_RST, OUTPUT);
    digitalWrite(OLED_RST, HIGH);
    delay(100);
    digitalWrite(OLED_RST, LOW);
    delay(100);
    digitalWrite(OLED_RST, HIGH);
  }

  display.init();
  display.flipScreenVertically();
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(display.getWidth() / 2, display.getHeight() / 2, LORA_SENDER ? "LoRa Sender" : "LoRa Receiver");
  display.display();
  delay(2000);

  if (SDCARD_CS >  0) {
    display.clear();
    SPIClass sdSPI(VSPI);
    sdSPI.begin(SDCARD_SCLK, SDCARD_MISO, SDCARD_MOSI, SDCARD_CS);
    if (!SD.begin(SDCARD_CS, sdSPI)) {
      display.drawString(display.getWidth() / 2, display.getHeight() / 2, "SDCard  FAIL");
    } else {
      display.drawString(display.getWidth() / 2, display.getHeight() / 2 - 16, "SDCard  PASS");
      uint32_t cardSize = SD.cardSize() / (1024 * 1024);
      display.drawString(display.getWidth() / 2, display.getHeight() / 2, "Size: " + String(cardSize) + "MB");
    }
    display.display();
    delay(2000);
  }


  String info = ds3231_test();
  if (info != "") {
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(display.getWidth() / 2, display.getHeight() / 2, info);
    display.display();
    delay(2000);
  }

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    display.clear();
    Serial.println("WiFi Connect Fail");
    display.drawString(display.getWidth() / 2, display.getHeight() / 2, "WiFi Connect Fail");
    display.display();
    delay(2000);
    esp_restart();
  }
  Serial.print("Connected : ");
  Serial.println(WiFi.SSID());
  Serial.print("IP:");
  Serial.println(WiFi.localIP().toString());
  display.clear();
  display.drawString(display.getWidth() / 2, display.getHeight() / 2, "IP:" + WiFi.localIP().toString());
  display.display();
  delay(2000);

  SPI.begin(CONFIG_CLK, CONFIG_MISO, CONFIG_MOSI, CONFIG_NSS);
  LoRa.setPins(CONFIG_NSS, CONFIG_RST, CONFIG_DIO0);
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  if (!LORA_SENDER) {
    display.clear();
    display.drawString(display.getWidth() / 2, display.getHeight() / 2, "LoraRecv Ready");
    display.display();
  }
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
}#else

  int count = 0;

void loop() {

#if  !LORA_SENDER

  if (millis() - displayTime > displayInterval) {
    if (LoRa.parsePacket()) {
      String recv = "";
      while (LoRa.available()) {
        recv += (char)LoRa.read();
      }
      count++;
      String recebidoCru = String(recv);//atribuindo o "texto" recebido a uma string
      char buffer[50];
      strcpy(buffer, recebidoCru.c_str());//copiando o texto recebido e transformando a copia numa string de C

      Serial.printf("recebido %s\n", buffer);

      char *temperatura = strtok(buffer, ",");
      char *umidade = strtok(NULL, ",");
      char *sinal = strtok(NULL, ",");
      char *chuva = strtok(NULL, ",");
      char *umidadeSolo = strtok(NULL, ",");
      Serial.println(temperatura);
      Serial.println(umidade);
      Serial.println(sinal);



      display.clear();
      display.drawString(display.getWidth() / 2, display.getHeight() / 2, recv);
      String info = "[" + String(count) + "]" + "RSSI " + String(LoRa.packetRssi());
      display.drawString(display.getWidth() / 2, display.getHeight() / 2 - 16, info);
      display.display();
      displayTime = millis();
    }

  }//displaymillis
  displayTime = millis();

  if (millis() - thingTime > thingInterval) {
    //thingspeak chank
    //
    // Read in field 1 of the private channel which is a counter

    //
    ThingSpeak.setField(1, LoRa.packetRssi());
    ThingSpeak.setField(2, umidade);
    ThingSpeak.setField(3, temperatura);
    ThingSpeak.setField(4, chuva);
    ThingSpeak.setField(5, random(0, 100));

    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if (x == 200) {
      Serial.println("Channel update successful.");
    }
    else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }

    // change the value



  }


#endif
}//loop
