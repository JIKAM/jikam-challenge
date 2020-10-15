#include <SoftwareSerial.h>
#include "stuff.h"

String inputMQTT = "";
String outputMQTT = "";

int* instrucoes;

Motores rodasEsquerdas = {7,8, 6};
Motores rodasDireitas = {9,10, 5};

Motores elevadores = {2,3, 0};

SensorLinha sensoresFrente = {6,5,4,3,2};

void setup() {
  //sensores frontais
  pinMode(sensoresFrente.direita, INPUT);
  pinMode(sensoresFrente.meioDireita, INPUT);
  pinMode(sensoresFrente.meio, INPUT);
  pinMode(sensoresFrente.meioEsquerda, INPUT);
  pinMode(sensoresFrente.esquerda, INPUT);
  
  // motores esquerdos
  pinMode(rodasEsquerdas.potencia, OUTPUT);
  pinMode(rodasEsquerdas.sentidoHorario, OUTPUT);
  pinMode(rodasEsquerdas.sentidoAntiHorario, OUTPUT);
  
  // motores direitos
  pinMode(rodasDireitas.potencia, OUTPUT);
  pinMode(rodasDireitas.sentidoHorario, OUTPUT);
  pinMode(rodasDireitas.sentidoAntiHorario, OUTPUT);

  pinMode(elevadores.sentidoHorario, OUTPUT);
  pinMode(elevadores.sentidoAntiHorario, OUTPUT);
  
  // initialize serial for debugging
  Serial.begin(9600);
  
}

void loop() {
  delay(100);

  girar(rodasEsquerdas, SENTIDO_HORARIO);
  girar(rodasDireitas, SENTIDO_HORARIO);

  delay(100);

  girar(rodasEsquerdas, SENTIDO_ANTI_HORARIO);
  girar(rodasDireitas, SENTIDO_HORARIO);

  delay(100);
  
  girar(rodasEsquerdas, SENTIDO_HORARIO);
  girar(rodasDireitas, SENTIDO_ANTI_HORARIO);
  
  delay(100);

  girar(rodasEsquerdas, SENTIDO_ANTI_HORARIO);
  girar(rodasDireitas, SENTIDO_HORARIO);
  
  delay(100);

  desligaMotores(rodasEsquerdas);
  desligaMotores(rodasDireitas);
  
  girar(elevadores, SENTIDO_HORARIO);
  
  delay(1200 * 2);

  girar(elevadores, SENTIDO_ANTI_HORARIO);
  
  delay(1200 * 2);
  
  desligaMotores(elevadores);
  
  girar(rodasEsquerdas, SENTIDO_ANTI_HORARIO);
  girar(rodasDireitas, SENTIDO_ANTI_HORARIO);

  delay(100);
}



void girar(struct Motores motores, String sentido){
  if(motores.potencia != 0)
    analogWrite(motores.potencia, POTENCIA_ROTACAO);
  //verifica para qual sentido é ara girar o motor
  if(sentido == SENTIDO_HORARIO){ 
    digitalWrite(motores.sentidoHorario, HIGH);
    digitalWrite(motores.sentidoAntiHorario, LOW);
  }else{
    digitalWrite(motores.sentidoHorario, LOW);
    digitalWrite(motores.sentidoAntiHorario, HIGH);
  }
}

void desligaMotores(struct Motores motores){
  digitalWrite(motores.sentidoHorario, LOW);
  digitalWrite(motores.sentidoAntiHorario, LOW);
}
