#include <SoftwareSerial.h>
#include "stuff.h"

String inputMQTT = "";
String outputMQTT = "";

int instrucoes[100];

Motores rodasEsquerdas = {8,7, 6};
Motores rodasDireitas = {10,9, 5};

Motores elevadores = {2,3, 0};

SensorLinha sensoresFrente = {28,29,30,31,32};

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
  Serial1.begin(9600);
  
}

void loop() {
  readData();

  if (instrucoes != NULL && instrucoes[0] != 0) {

    executeActions();
    
  
  }else{
    desligaMotores(rodasEsquerdas);
    desligaMotores(rodasDireitas);    
  
  }
}

void readData() {
  int i = 0;

  if (Serial1.available()) {
    String temp = "";
    while (Serial1.available()) {
      char c = (char)Serial1.read();
      if (c != '\n' && c != ',' && c != ' ' && String(c) != "") {
          instrucoes[i] = String(c).toInt();
          i++;
      }
      Serial.println(c);
      delay(10);
    }
  }
}

void executeActions() {
  int i=0;
  while(i < (sizeof(instrucoes) / sizeof(int)) ){
    if(instrucoes[i] != 0){
      Serial.println("InputMQTT Action - Sentido: " + String(instrucoes[i]));
      realizarTask(instrucoes[i]);
      alinhar();
    }
    i++;
  }
}

void alinhar(){
    while( ! isLinhaSensor(sensoresFrente.meio) ){
      if( isLinhaSensor(sensoresFrente.meioEsquerda) ){
          girar(rodasDireitas, SENTIDO_ANTI_HORARIO);
          girar(rodasEsquerdas, SENTIDO_HORARIO);
          delay(100);
      }else if ( isLinhaSensor(sensoresFrente.meioDireita)){
          girar(rodasDireitas, SENTIDO_HORARIO);
          girar(rodasEsquerdas, SENTIDO_ANTI_HORARIO);
          delay(100);
      }
    }
}

void paraFrente(){
  Serial.println("Frente START"); 
  do {
    alinhar();
    girar(rodasDireitas, SENTIDO_HORARIO); 
    girar(rodasEsquerdas, SENTIDO_HORARIO);
    delay(100);
    } while( isLinhaSensor(sensoresFrente.meio) && ! isLinhaSensor(sensoresFrente.esquerda) && ! isLinhaSensor(sensoresFrente.direita) );
   Serial.println("Frente END");
}

void paraTras(){
  Serial.print("Tras START"); 
  girar(rodasDireitas, SENTIDO_HORARIO);
  girar(rodasEsquerdas, SENTIDO_ANTI_HORARIO);
  delay(10);
  Serial.print("Tras END"); 
}

void paraDireita(){
  Serial.print("Direita START");
  girar(rodasDireitas, SENTIDO_HORARIO);
  girar(rodasEsquerdas, SENTIDO_HORARIO);
  delay(100);
  girar(rodasDireitas, SENTIDO_ANTI_HORARIO); 
  girar(rodasEsquerdas, SENTIDO_HORARIO);
  delay(100);

  while( ! digitalRead(sensoresFrente.meio) ){
    girar(rodasDireitas, SENTIDO_ANTI_HORARIO); 
    girar(rodasEsquerdas, SENTIDO_HORARIO);
    delay(10);
  }
  Serial.print("Direita END");
}

void paraEsquerda(){
  Serial.print("Esquerda START");
  girar(rodasDireitas,SENTIDO_ANTI_HORARIO);
  girar(rodasEsquerdas, SENTIDO_ANTI_HORARIO);
  delay(100);
  girar(rodasDireitas, SENTIDO_ANTI_HORARIO); 
  girar(rodasEsquerdas, SENTIDO_HORARIO);
  delay(100);

  while( ! digitalRead(sensoresFrente.meio) ){
    girar(rodasDireitas, SENTIDO_ANTI_HORARIO); 
    girar(rodasEsquerdas, SENTIDO_HORARIO);
    delay(10);
  }
  Serial.print("Esquerda END");
 
}

void realizarTask(int task){
  Serial.print("realizarTask START");
  
  if(task == EM_FRENTE){ 
    //F-(Forward)=(direito=horario && esquerdo=anti-horario) 
    paraFrente();

  
  }else if(task == PARA_TRAS){
    //B-(Backward)=(direito=anti-horario && esquerdo=horario)
    paraTras();

    
  }else if(task == GIRAR_ESQUERDA){
    //L-(Left)=(direito=anti-horario && esquerdo=anti-horario)  
    paraEsquerda();
    
  }else if(task == GIRAR_DIREITA){ 
    //L-(Left)=(direito=anti-horario && esquerdo=anti-horario)  
    paraDireita();
    
  }else if(task == PEGAR_CARGA){ 
      
    
  }

  
   Serial.print("realizarTask END");
  
}

//quando nao refletir, esta na linha
bool isLinhaSensor(int pinoSensor){
  return digitalRead(pinoSensor) == 0;
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
