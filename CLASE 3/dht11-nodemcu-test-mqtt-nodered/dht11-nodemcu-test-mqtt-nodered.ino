
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SimpleDHT.h>


// for DHT11, 
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2
int pinDHT11 = 14; //D5
SimpleDHT11 dht11;

// Update these with values suitable for your network.

const char* ssid = "HUAWEI-73ED";
const char* password = "04592945";
const char* mqtt_server = "192.168.8.101";


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;


int temperatura;  //variable para almacenar informacion de sensor dht
int humedad; //variable para almacenar informacion de sensor dht

void sensor_dht11()
{
   //Serial.println("=================================");
  Serial.println("Sample DHT11...");
  
  // read without samples.
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(pinDHT11, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err);delay(1000);
    return;
  }
  
  Serial.print("Sample OK: ");
  Serial.print((int)temperature); Serial.print(" *C, "); 
  Serial.print((int)humidity); Serial.println(" H");
  
  temperatura=(int)temperature;
  humedad=(int)humidity;
  // DHT11 sampling rate is 1HZ.
  //delay(1500);
}




void setup() {
  pinMode(D4, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
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

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(D4, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(D4, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

///////MODIFICAR PARA QUE FUNCIONE CON SHIFTR.IO
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
   if (client.connect("ESP8266Client")) {
  //  if(client.connect(clientID,username,passwords,willTopic,willQoS,willRetain,willMessage)){ 
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;

    sensor_dht11();
    
    char sense[]=""; //almacenar dato en char array
    
    String my_payload=String(temperatura); //convierto luego a string
    my_payload.toCharArray(sense, (my_payload.length() + 1)); //luego convierto a char array 
    client.publish("temperatura", sense); //siempre recibe un char array 
    Serial.println(sense);
    
    
    my_payload=String(humedad); //convierto luego a string
    my_payload.toCharArray(sense, (my_payload.length() + 1)); //luego convierto a char array 
    client.publish("humedad", sense); //siempre recibe un char array 
    Serial.println(sense);
 
    
  }
}



