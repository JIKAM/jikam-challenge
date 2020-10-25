#include <SoftwareSerial.h>
#include "stuff.h"

String inputMQTT = "";
String outputMQTT = "";

int instrucoes[100];
int contadorActions = 0;
int contadorLeitura = 0;

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

  //inicio teste
    


  //fim teste
  
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
  contadorLeitura = 0;

  if (Serial1.available()) {
    String temp = "";
    while (Serial1.available()) {
      char c = (char)Serial1.read();
      if (c != '\n' && c != ',' && c != ' ' && String(c) != "") {
          instrucoes[contadorLeitura] = String(c).toInt();
          Serial.println(c);
          contadorLeitura++;
      }
      delay(10);
    }
  }
}
void executeActions() {
  contadorActions = 0;
  while(contadorActions < (sizeof(instrucoes) / sizeof(int)) ){
    if(instrucoes[contadorActions] != 0){
      Serial.println("InputMQTT Action - Sentido: " + String(instrucoes[contadorActions]));
      realizarTask(instrucoes[contadorActions]);
      Serial1.print(instrucoes[contadorActions]);
      instrucoes[contadorActions] = 0;
      alinhar();
    }else{
      desligaMotores(rodasDireitas);
      desligaMotores(rodasEsquerdas);
    }
    contadorActions++;
  }
}

void alinhar(){
    while( ! isLinhaSensor(sensoresFrente.meio) ){
      if( isLinhaSensor(sensoresFrente.meioEsquerda) ){
        while( ! isLinhaSensor(sensoresFrente.meio) ){
          desligaMotores(rodasDireitas);
          girar(rodasEsquerdas, SENTIDO_HORARIO);
          delay(5);
         }
      }else if ( isLinhaSensor(sensoresFrente.meioDireita)){
        while( ! isLinhaSensor(sensoresFrente.meio) ){
          girar(rodasDireitas, SENTIDO_HORARIO);
          desligaMotores(rodasEsquerdas);
          delay(5);
        }
      }
    }
}

void paraFrente(){
  Serial.println("Frente START"); 
  do {
    //alinhar();
    girar(rodasDireitas, SENTIDO_HORARIO); 
    girar(rodasEsquerdas, SENTIDO_HORARIO);
    delay(2);
    } while( ! isLinhaSensor(sensoresFrente.esquerda) && ! isLinhaSensor(sensoresFrente.direita) );
   Serial.println("Frente END");
}

void paraTras(){
  Serial.println("Tras START"); 
  girar(rodasDireitas, SENTIDO_HORARIO);
  girar(rodasEsquerdas, SENTIDO_ANTI_HORARIO);
  delay(10);
  Serial.println("Tras END"); 
}

void paraDireita(){
  Serial.println("Direita START");

  while( isLinhaSensor(sensoresFrente.direita) ){
    girar(rodasDireitas, SENTIDO_HORARIO);
    girar(rodasEsquerdas, SENTIDO_HORARIO);
    delay(1);  
  }

  while( isLinhaSensor(sensoresFrente.meio) ){
    girarForte(rodasDireitas, SENTIDO_ANTI_HORARIO); 
    girarForte(rodasEsquerdas, SENTIDO_HORARIO);
    delay(1);  
  }
  
  while( ! isLinhaSensor(sensoresFrente.meio) ){
    girarForte(rodasDireitas, SENTIDO_ANTI_HORARIO); 
    girarForte(rodasEsquerdas, SENTIDO_HORARIO);
    delay(1);
  }
  
  Serial.println("Direita END");
}

void paraEsquerda(){
  Serial.println("Esquerda START");
  
  while( isLinhaSensor(sensoresFrente.esquerda) ){
    girar(rodasDireitas, SENTIDO_HORARIO);
    girar(rodasEsquerdas, SENTIDO_HORARIO);
    delay(1);  
  }

  while( isLinhaSensor(sensoresFrente.meio) ){
    girarForte(rodasDireitas, SENTIDO_HORARIO); 
    girarForte(rodasEsquerdas, SENTIDO_ANTI_HORARIO);
    delay(1);  
  }

  while( ! isLinhaSensor(sensoresFrente.meio) ){
    girarForte(rodasDireitas, SENTIDO_HORARIO); 
    girarForte(rodasEsquerdas, SENTIDO_ANTI_HORARIO);
    delay(1);
  }
  Serial.println("Esquerda END");
 
}
bool elevado = false;
void elevaDeseleva(){
  Serial.println("elevaDeseleva START");
  if(elevado){
    girar(elevadores, SENTIDO_HORARIO);
    delay(800);
  }else{
    girar(elevadores, SENTIDO_ANTI_HORARIO);
    delay(800);
  }
  Serial.println("elevaDeseleva END");
}


void realizarTask(int task){
  Serial.println("realizarTask START");
  
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
     elevaDeseleva();
    
  }

  
   Serial.println("realizarTask END");
  
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

void girarForte(struct Motores motores, String sentido){
  if(motores.potencia != 0)
    analogWrite(motores.potencia, 150);
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
