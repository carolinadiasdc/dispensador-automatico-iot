#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>


//Conexao
const char* ssid = "Stefan123456";
const char* password = "nnlv6406";

//Conexao MQTT
const char* mqttServer = "postman.cloudmqtt.com";   //server
const char* mqttUser = "aeleozfk";              //user
const char* mqttPassword = "J0MSDy4RiaK8";      //password
const int mqttPort = 16157;                     //port
const char* mqttTopicSub = "alc/Gel";      //tópico que sera assinado
String strMSG = "0";
char mensagem[30];

//Pin
const int trigPin = D6;
const int echoPin = D5;
Servo servo;


long duration = 0;
double distance = 0;


WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial.println();
  Serial.print("Wifi connecting to ");
  Serial.println( ssid );
  WiFi.begin(ssid, password);
  Serial.println();
  Serial.print("Connecting");

  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wifi Connected Success!");
  Serial.print("NodeMCU IP Address : ");
  Serial.println(WiFi.localIP() );
  Serial.println("Conectado na rede WiFi com sucesso!");
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {

    Serial.println("Conectando ao Broker MQTT...");

    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
      Serial.println("Conectado");
    } else {
      Serial.print("falha estado  ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  //subscreve no tópico
  client.subscribe(mqttTopicSub);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  servo.attach(D2);

}

void loop() {


  getDistance();
  // Prints the distance value to the serial monitor
  Serial.print("Distance: ");
  Serial.println(distance);
  
  if(distance < 20 ){
    delay(500);
    servo.write(180);
    
    Serial.print("180");
    sprintf(mensagem, "1");
    Serial.print("Mensagem enviada: ");
    Serial.println(mensagem);  //Envia a mensagem ao broker
    client.publish("alc/Gel", mensagem);
    Serial.println("Mensagem enviada com sucesso...");
    
 
  }else{
    
    servo.write(0);
    
    Serial.print("0");
    sprintf(mensagem, "0");
    Serial.print("Mensagem enviada: ");
    Serial.println(mensagem);  //Envia a mensagem ao broker
    client.publish("alc/Gel", mensagem);
    Serial.println("Mensagem enviada com sucesso...");
   
    }

  delay(1000);

}

void getDistance()
{
  // Clear trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // trigPin HIGH por 10ms
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  //Reads echoPin, returns the travel time of the sound wave in ms
  duration = pulseIn(echoPin, HIGH);

  // Calculating the distance, in centimeters, using the formula described in the first section.
  distance = duration * 0.034 / 2;
  
}

void callback(char* topic, byte* payload, unsigned int length) {
  //armazena msg recebida em uma sring
  payload[length] = '\0';
  strMSG = String((char*)payload);
  Serial.print("Mensagem chegou do tópico: ");
  Serial.println(topic);
  Serial.print("Mensagem:");
  Serial.print(strMSG);
  Serial.println();
  Serial.println("-----------------------");
}

void reconect() {
  //Enquanto estiver desconectado
  while (!client.connected()) {
    Serial.print("Tentando conectar ao servidor MQTT");

    bool conectado = strlen(mqttUser) > 0 ?
                     client.connect("ESP8266Client", mqttUser, mqttPassword) :
                     client.connect("ESP8266Client");

    if (conectado) {
      Serial.println("Conectado!");
      //subscreve no tópico
      client.subscribe(mqttTopicSub, 1); //nivel de qualidade: QoS 1
    } else {
      Serial.println("Falha durante a conexão.Code: ");
      Serial.println( String(client.state()).c_str());
      Serial.println("Tentando novamente em 10 s");

      //Aguarda 10 segundos
      delay(5000);
    }
  }
}
