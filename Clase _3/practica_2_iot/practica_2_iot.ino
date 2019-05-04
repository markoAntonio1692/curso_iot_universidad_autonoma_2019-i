#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <SimpleDHT.h>

// for DHT22, 
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2
int pinDHT22 = 5;  //D1
SimpleDHT22 dht22(pinDHT22);

const char* ssid = "CLASSROOM";
const char* password = "1122334455";
const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;



  float temperature = 0;
  float humidity = 0;
  
void sensor_dht(){
   Serial.println("=================================");
  Serial.println("Sample DHT22...");
  
  
  pinMode(pinDHT22,INPUT);
  
  int err = SimpleDHTErrSuccess;
  if ((err = dht22.read2(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT22 failed, err="); Serial.println(err);delay(2000);
    return;
  }
  
  Serial.print("Sample OK: ");
  Serial.print((float)temperature); Serial.print(" *C, ");
  Serial.print((float)humidity); Serial.println(" RH%");

}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print("] ");
  
  /////////////////////////////
  String string_topic=String(topic); //almaceno el nombre del topico del cual recibo informacion
  String string_payload="";
  char c;
  /////////////////////////////
  
  for (int i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
    c=payload[i];
    string_payload+=c;
  }
   Serial.print(" Topico: ");  
   Serial.print(string_topic);  
   Serial.print("| Payload: ");  
   Serial.print(string_payload); 
   Serial.println();  
  
  

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
      client.subscribe("foco1-caballero");
      client.subscribe("foco2-caballero");
      client.subscribe("servo-caballero");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  
  client.loop();

  long now = millis();
  if (now - lastMsg >2000) {
    lastMsg = now;
   //lectura de sensor////////////////////////////////////////////////////
    
    sensor_dht();  //lectura de sensor dht
    float temp = temperature ; //simulamos que leemos el dato de un sensor
    float hum = humidity ;   
   /////////////////////////////////////////////////////////////////////////
    
    char sense[20]=""; //almacenar dato en char array
 
    String my_payload=String(temp); //convierto luego a string
    my_payload.toCharArray(sense, (my_payload.length() + 1)); //luego convierto a char array 
 
    client.publish("temperatura-caballero", sense); //siempre recibe un char array 
    Serial.println(sense);
    //////////////////////////////////////////////////////////////////////
    my_payload=String(hum); //convierto luego a string
    my_payload.toCharArray(sense, (my_payload.length() + 1)); //luego convierto a char array 
 
    client.publish("humedad-caballero", sense); //siempre recibe un char array 
    Serial.println(sense);
  }
}
