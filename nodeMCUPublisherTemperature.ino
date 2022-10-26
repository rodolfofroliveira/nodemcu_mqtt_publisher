#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

const char* SSID = "SSID";
const char* PASS = "senha";

const char* enderecoBroker = "test.mosquitto.org";
const char* topico = "10495285/temperatura";

WiFiClient espClient;
PubSubClient clienteMQTT(espClient);

#define DHTPIN 2  
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


void reconnect() {
  while (!clienteMQTT.connected()) {
    Serial.print("Tentando se conectar ao Broker MQTT - ");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    if (clienteMQTT.connect(clientId.c_str())) {
      
      Serial.println("Conectado!");

      clienteMQTT.subscribe(topico);
    } 
    else {     
      Serial.print("Falha, rc=");
      Serial.print(clienteMQTT.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void setup() {

  dht.begin();

  Serial.begin(9600);
  
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(SSID,PASS);
  while (WiFi.status() != WL_CONNECTED)
   {
    delay(300);
    Serial.print(".");
   }
  Serial.print("Endereço IP obtido: ");
  Serial.println(WiFi.localIP());

  //Define qual é o servidor broker, bem como sua porta
  clienteMQTT.setServer(enderecoBroker, 1883);
}


void loop() {

  delay(2000);
  
  if (!clienteMQTT.connected()) {
    reconnect();
  }

  //Realiza a leitura da temperatura
  float temperatura = dht.readTemperature();

  //Converte temperatura do tipo float para char (4 posições).
  // A variável terá duas casas decimais e 1 casa após a vírgula
  char temperaturaChar[4];
  dtostrf(temperatura, 2, 1, temperaturaChar);

  //Publica a temperatura no tópico especificado
  clienteMQTT.publish(topico,temperaturaChar);
}
