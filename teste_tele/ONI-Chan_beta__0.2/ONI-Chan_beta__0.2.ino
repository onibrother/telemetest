//ooo000 ONI-Chan 000ooo
//Bibliotecas#############################
#include "SD.h"
#include "SPI.h"
#include <Wire.h>
#include <Rtc_Pcf8563.h>
#include <dht.h>

//DHT11###########
#define sensor_temperatura_humidade A0 
dht DHT;

//buzzer########## 
int buzzer = 4;           

//LDR#############
int sensor_luz = A1;  
int valor_de_luz;    

//sd-module#######
const int pino_SD = 10;
//String dataString = "";
File ONIdata;    

//RTC#############
Rtc_Pcf8563 relogio;  

//LEDs############
int ledAzul = 7; 
int ledVerm = 6; 

void setup() {
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, HIGH);
  delay(200);
  digitalWrite(buzzer, LOW);
  delay(500);
  digitalWrite(buzzer, HIGH);
  delay(200);
  digitalWrite(buzzer, LOW);
  
  Serial.begin(9600);      
  //setando configuracoes do arquivador
  pinMode(ledAzul, OUTPUT);  
  pinMode(ledVerm, OUTPUT); 
  pinMode(pino_SD, OUTPUT);  
  if(!SD.begin(pino_SD)){   
    Serial.println("Carta sd falhou, ou nao esta no modulo!");
    digitalWrite(ledVerm, HIGH); 
    return;
  }else{
    Serial.println("Cartao disponivel");
    digitalWrite(ledAzul, HIGH); 
  }
  //setando configuracoes do rtc
  relogio.initClock();         
  relogio.setTime(17, 00, 0); 
}

void loop() {
  valor_de_luz = analogRead(sensor_luz);   
  valor_de_luz = map(valor_de_luz, 0, 1023, 0, 100);
  DHT.read11(sensor_temperatura_humidade);       
 
  salvando_dados();     
  
 
  Serial.println(relogio.formatTime());  
  Serial.println(DHT.temperature);  
  Serial.println(DHT.humidity);  
  Serial.println(valor_de_luz);         
  delay(1000);         
}

void salvando_dados(){     
  ONIdata = SD.open("dados.csv", FILE_WRITE); 
  if(ONIdata){               
    
    ONIdata.print(relogio.formatTime());    
    ONIdata.print(",");                
    ONIdata.print(DHT.temperature);   
    ONIdata.print(",");               
    ONIdata.print(DHT.humidity);   
    ONIdata.print(",");
    ONIdata.println(valor_de_luz);      
    ONIdata.close();         
  }else{
    Serial.println("Erro ao escrever o arquivo!"); 
  }
}
