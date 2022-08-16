/*
* Telemetrilha 
* Vantacat
* 
* O inicio de uma rede de sensores ESPalhados pelo rio de janeiro conectados a entre si via protocolo de 
*comunicação lora cujos dados sao enviados para a internet 
*
*
* 16 de agosto de 2022
*/

// bibliotecas usadas 
#include <SPI.h>
#include <LoRa.h>
#include "ds3231.h"
#include <WiFi.h>
#include <SD.h>
#include "ThingSpeak.h"
#include "secrets.h"
// bibliotecas usadas 

//variaveis 
unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;
unsigned long thingTime = 0;
unsigned long displayTime = 0;
unsigned int thingInterval = 20e3;
unsigned int displayInterval = 20e2;
//variaveis 

OLED_CLASS_OBJ display(OLED_ADDRESS, OLED_SDA, OLED_SCL);


#define WIFI_SSID       "espnet"
#define WIFI_PASSWORD   "12345678"

WiFiClient  client;


void setup()
{

  Serial.begin(115200);
  //pinMode(sensor, INPUT);
  while (!Serial);//thingspeak chank
    //
    // Read in field 1 of the private channel which is a counter

    //

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
}

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

      //Serial.printf("recebido %s\n", buffer);

      char *temperatura = strtok(buffer, ",");
      char *umidade = strtok(NULL, ",");
      char *sinal = strtok(NULL, ",");
      char *chuva = strtok(NULL, ",");
      char *umidadeSolo = strtok(NULL, ",");
            
      // Serial.println(temperatura);
      // Serial.println(umidade);
      // Serial.println(sinal);

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
