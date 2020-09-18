#include <ESP8266WiFi.h>
#include <PubSubClient.h>


#define ID_MQTT  "PLCWIFIKC1231"

#define SUBSCRIBE_RECEBE_TAREFA "subscribe/tarefa/nova"

#define PUBLISH_INICIOU_ACAO "publish/action/inicioAcao"
#define PUBLISH_FINAL_ACAO "publish/action/fimAcao"
#define PUBLISH_ENCONTROU_EXCECAO "publish/action/excecao"
#define PUBLISH_CONNECTION_VERIFICATION "publish/connection/verification"

//WiFi
// SSID -> nome da rede WiFi que deseja se conectar
// Senha da rede WiFi que deseja se conectar

WiFiClient wifiClient;
const char* SSID_REDE = "xodrom";
const char* SENHA_REDE = "20N40p455055";   

//MQTT Server
//URL do broker MQTT que se deseja utilizar, estamos usando um Broker publico
// Porta do Broker MQTT -> Usando TCP

const char* BROKER_MQTT = "broker.mqtt-dashboard.com"; 
int BROKER_PORT = 1883;                      

PubSubClient MQTT(wifiClient);        // Instancia o Cliente MQTT passando o objeto espClient

//Declaração das Funções
void mantemConexoes();  //Garante que as conexoes com WiFi e MQTT Broker se mantenham ativas
void conectaWiFi();     //Faz conexão com WiFi
void conectaMQTT();     //Faz conexão com Broker MQTT
void recebePacote(char* topic, byte* payload, unsigned int length);


String mensagemArduino;

void setup() {
  Serial.begin(9600);

  conectaWiFi();
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(recebePacote);
}

void loop() {
  mantemConexoes();
  MQTT.loop();
  mensagemArduino = readDataFromArduino();
  if(mensagemArduino != NULL || mensagemArduino != ""){
    if(mensagemArduino == "1" || mensagemArduino == "2" ||mensagemArduino == "3" || mensagemArduino == "4" || mensagemArduino == "5")
      finalizouUmaTask(mensagemArduino);
     else
      encontrouExcecao(mensagemArduino);
  }
}

void mantemConexoes() {
  if (!MQTT.connected()) {
    conectaMQTT();
  }
  //se não há conexão com o WiFI, a conexão é refeita
  conectaWiFi(); 
}

void conectaWiFi() {

  if (WiFi.status() == WL_CONNECTED) {
    return;
  }
  // Conecta na rede WI-FI
  WiFi.begin(SSID_REDE, SENHA_REDE); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
}

void conectaMQTT() {
  while (!MQTT.connected()) {
    
    if (MQTT.connect(ID_MQTT)) {
      
      MQTT.subscribe(SUBSCRIBE_RECEBE_TAREFA);
      while(publicaValor("1", PUBLISH_CONNECTION_VERIFICATION) == false)
        delay(100);
    
    } else {
      // Nova tentatica de conexao em 10s
      delay(10000);
    
    }
  }
}


bool publicaValor(String mensagem, char* nomePublicacao) {
  static unsigned long actionDoing;

  //Elimina efeito Bouncing
  if((millis() - actionDoing) > 100){
    char charBuff[mensagem.length()+1];
    mensagem.toCharArray(charBuff, mensagem.length()+1);
    MQTT.publish(nomePublicacao, charBuff);
  }
  
  actionDoing = millis();
  return true;
}


void recebePacote(char* topic, byte* payload, unsigned int length) {
  String msg;
  String topicString = String(topic);

  //obtem a string do payload recebido
  for (int i = 0; i < length; i++)
  {
    char c = (char)payload[i];
    msg += c;
  }
 
  if (topicString == SUBSCRIBE_RECEBE_TAREFA) {
    //printa para o arduino
    Serial.println(msg);
  }
  
}


void finalizouUmaTask(String task) {
  while (publicaValor(task, PUBLISH_FINAL_ACAO) == false) {
    delay(100);
  }
}

void encontrouExcecao(String task) {
  while (publicaValor(task, PUBLISH_ENCONTROU_EXCECAO) == false) {
    delay(100);
  }
}


String readDataFromArduino() {
  String payload;

  if (Serial.available()) {
    while (Serial.available()) {
      char c = Serial.read();
      if (c != NULL && c != '\n') {
        payload += c;
      }
      
      delay(10);
    }
  }

  return payload;
}
