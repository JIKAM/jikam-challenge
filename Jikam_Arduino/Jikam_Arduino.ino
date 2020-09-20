#include <SoftwareSerial.h>
#include "stuff.h"

String inputMQTT = "";
String outputMQTT = "";

int* instrucoes;

Motores rodasEsquerdas = {3,4, 2};
Motores rodasDireitas = {5,6, 7};

Motores elevadores = {8,9, 0};

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
  Serial1.begin(9600);
  
}

void loop() {
  instrucoes = readData();


  if (instrucoes != NULL && instrucoes[0] != 0) {

    executeActions(instrucoes);
    
  
  }else{
    desligaMotores(rodasEsquerdas);
    desligaMotores(rodasDireitas);    
  
  }
}

int* readData() {
  int i = 0;
  int tempInstrucoes[100];

  if (Serial1.available()) {
    String temp = "";
    while (Serial1.available()) {
      char c = (char)Serial1.read();
      if (c != '\n' && c != ',') {
          tempInstrucoes[i] = String(c).toInt();
      }
      Serial.println(c);
      delay(10);
    }
  }

  return tempInstrucoes;
}

void executeActions(int acts[]) {
  int i=0;
  while(i < (sizeof(acts) / sizeof(int)) ){
    if(acts[i] != 0){
      Serial.print("InputMQTT Action - Sentido: " + String(acts[i]));
      realizarTask(acts[i]);
      alinhar();
    }
    i++;
  }
}

void alinhar(){
    while( ! digitalRead(sensoresFrente.meio) ){
      if( digitalRead(sensoresFrente.meioEsquerda) ){
          girar(rodasDireitas,SENTIDO_ANTI_HORARIO);
          girar(rodasEsquerdas, SENTIDO_ANTI_HORARIO);
          delay(10);
      }else if (digitalRead(sensoresFrente.meioDireita)){
          girar(rodasDireitas, SENTIDO_HORARIO);
          girar(rodasEsquerdas, SENTIDO_HORARIO);
          delay(10);
      }
    }
}

void paraFrente(){
  Serial.print("Frente START"); 
  do {
    alinhar();
    girar(rodasDireitas, SENTIDO_ANTI_HORARIO); 
    girar(rodasEsquerdas, SENTIDO_HORARIO);
    delay(10);
    } while( digitalRead(sensoresFrente.meio) || digitalRead(sensoresFrente.esquerda) || digitalRead(sensoresFrente.direita) );
   Serial.print("Frente END");
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
