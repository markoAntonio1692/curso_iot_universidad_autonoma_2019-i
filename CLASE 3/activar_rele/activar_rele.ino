/*
 Basic ESP8266 MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "HUAWEI-73ED";
const char* password = "04592945";
const char* mqtt_server = "broker.shiftr.io";

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////AUTENTICACION BROKER MQTT///////////////////////////////////////////
const char clientID[]="NODEMCU-MARKO-001";  //identificador unico de para cada dispositivo iot
const char username[]="nodemcu-marko";       //usuario configurado en broker
const char passwords[]="123456789";      //contraseña usuario broker
const char willTopic[]= "";
int willQoS=0 ;                             //0-1-2
int  willRetain=0 ;                         //0-1  //si se activa o no la retencion de data
const char willMessage[]="";    //mensaje cuando device este desconectado de broker
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;



char c;  //variable para contener carcateres del payload 
String readString=""; 



void setup() {
  pinMode(D4, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(D6,OUTPUT);
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
  /////////////////////////////
  String string_topic;
  string_topic=String(topic); //almaceno el nombre del topico del cual recibo informacion
  readString="";
  /////////////////////////////
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    c=payload[i];
    readString+=c;
  }
  Serial.println();

if(string_topic=="rele")
{
   Serial.println(string_topic);
   if(readString=="1")
   {
    Serial.println(readString);
    digitalWrite(D6,HIGH);
   }
   if(readString=="0")
   {
    Serial.println(readString);
    digitalWrite(D6,LOW);
   }
}

if(string_topic=="led")
{
   Serial.println(string_topic);
   if(readString=="1")
   {
    Serial.println(readString);
    digitalWrite(D4,LOW);
   }
   if(readString=="0")
   {
    Serial.println(readString);
    digitalWrite(D4,HIGH);
   }
}
 
}

/*
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(D4, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(D4, HIGH);  // Turn the LED off by making the voltage HIGH
  }
*/


///////MODIFICAR PARA QUE FUNCIONE CON SHIFTR.IO
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
   // if (client.connect("ESP8266Client")) {
    if(client.connect(clientID,username,passwords,willTopic,willQoS,willRetain,willMessage)){ 
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

   client.subscribe("rele");
   client.subscribe("led");
  }
}



