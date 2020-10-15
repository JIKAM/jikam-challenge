#include <SoftwareSerial.h>
#include "stuff.h"

String inputMQTT = "";
String outputMQTT = "";

int instrucoes[100];

Motores rodasEsquerdas = {8,7, 6};
Motores rodasDireitas = {9,10, 5};

Motores elevadores = {2,3, 0};

SensorLinha sensoresFrente = {32,31,30,29,28};

void setup() {
  //sensores frontais
  pinMode(sensoresFrente.direita, INPUT);
  pinMode(sensoresFrente.meioDireita, INPUT);
  pinMode(sensoresFrente.meio, INPUT);
  pinMode(sensoresFrente.meioEsquerda, INPUT);
  pinMode(sensoresFrente.esquerda, INPUT);
  
  // initialize serial for debugging
  Serial.begin(9600);
  
}

void loop() {
    Serial.print(digitalRead(sensoresFrente.direita));
    Serial.print(" ");
    Serial.print(digitalRead(sensoresFrente.meioDireita));
    Serial.print(" ");
    Serial.print(digitalRead(sensoresFrente.meio));
    Serial.print(" ");
    Serial.print(digitalRead(sensoresFrente.meioEsquerda));
    Serial.print(" ");
    Serial.print(digitalRead(sensoresFrente.esquerda));
    Serial.println();
   delay(500);
}
