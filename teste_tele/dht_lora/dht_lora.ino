#include <dht.h>
#define sensor 12  
dht DHT;



float temperatura=0;
int umidade = 0;
unsigned long passado = 0;
const long intervalo = 1000;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(sensor, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
unsigned long atual = millis();

if((atual - passado)>=intervalo){
  passado = atual; 

  DHT.read22(sensor);
  
  temperatura = DHT.temperature;
  umidade = DHT.humidity; 
  Serial.print("temperatura: ");
  Serial.print(temperatura);
  Serial.print("umidade: ");
  Serial.println(umidade);
 
}

}
