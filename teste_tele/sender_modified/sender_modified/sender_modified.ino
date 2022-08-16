#include <SPI.h>
#include <LoRa.h>
#include "ds3231.h"
#include <WiFi.h>
#include <SD.h>
#include <dht.h>

dht DHT;
OLED_CLASS_OBJ display(OLED_ADDRESS, OLED_SDA, OLED_SCL);

#define WIFI_SSID       "skynet"
#define WIFI_PASSWORD   "f22raptor"
#define sensor 13
#define sensorChuva 34
#define sensorSolo 35

float temperatura=0;
int umidade=0;
int umidadeSolo=0;
bool isRaining;

void setup()
{
    Serial.begin(115200);
    pinMode(sensor, INPUT);
    pinMode(sensorChuva, INPUT);
    pinMode(sensorSolo, INPUT);
    
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

    SPI.begin(CONFIG_CLK, CONFIG_MISO, CONFIG_MOSI, CONFIG_NSS);
    LoRa.setPins(CONFIG_NSS, CONFIG_RST, CONFIG_DIO0);
    if (!LoRa.begin(BAND)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }
LoRa.print(",");
        
}

void loop()
{
  DHT.read22(sensor);
  temperatura = DHT.temperature; 
  umidade = DHT.humidity; 

  umidadeSolo = (analogRead(sensorSolo)/4096)*100;

  isRaining = !digitalRead(sensorChuva);
  
  
#if LORA_SENDER
     
        LoRa.beginPacket();
        
        LoRa.print(temperatura);
        LoRa.print(",");
        LoRa.print(umidade);
        LoRa.print(",");
        LoRa.print(isRaining);
        LoRa.print(",");
        LoRa.print(umidadeSolo);
        
//        LoRa.print(",");
//        LoRa.print(LoRa.rssi());
//      
  
        //"temp,umi
        LoRa.endPacket();

    delay(2500);

#endif
}
