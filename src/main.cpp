#include <WiFi.h>
#include <PubSubClient.h>
#include <stdlib.h>

//**************************************
//*********** MQTT CONFIG **************
//**************************************

const char* mqttServer = "192.168.1.40";
const int mqttPort = 1883;
const char* mqttUser = "dqyoxjgo";
const char* mqttPassword = "bDFPyIOx5Mln";
const char* root_topic_subscribe ="bazuca/input";
const char* root_topic_publish ="bazuca/output";


//**************************************
//*********** WIFICONFIG ***************
//**************************************

const char* ssid = "Martina";
const char* password =  "17156mscd";

//**************************************
//*********** GLOBALES   ***************
//**************************************

WiFiClient espClient;
PubSubClient client(espClient);
char msg[25];
long count=0;
const int SensorHumedad1 = 33; //mido la humedad al analogico GPIO0

//************************
//** F U N C I O N E S ***
//************************

//Prototipo de funciones
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void setup_wifi();
float HumiditySensor();


//SETUP
void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  pinMode(SensorHumedad1, INPUT);
}

//LOOP PRINCIPAL
void loop() {
  if(!client.connected()){
    reconnect();
  }

  //Envío de datos por MQTT
  if(client.connected()){
    String str = "La humedad es ->" + String(HumiditySensor()) + " %";
    str.toCharArray(msg,25);
    client.publish(root_topic_publish,msg);
	}


  client.loop();
}


//*****************************
//***    CONEXION WIFI      ***
//*****************************

void setup_wifi(){
	delay(10);
	// Nos conectamos a nuestra red Wifi
	Serial.println();
	Serial.print("Conectando a ssid: ");
	Serial.println(ssid);

	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

	Serial.println("");
	Serial.println("Conectado a red WiFi!");
	Serial.println("Dirección IP: ");
	Serial.println(WiFi.localIP());
}


//*****************************
//***    CONEXION MQTT      ***
//*****************************

void reconnect() {
	while (!client.connected()) {
		Serial.print("Intentando conexión Mqtt...");
		// Creamos un cliente ID
		String clientId = "BAZUCA_TEST";
		clientId += String(random(0xffff), HEX);
		// Intentamos conectar
		if (client.connect(clientId.c_str(),mqttUser,mqttPassword)) {
			Serial.println("Conectado!");
			// Nos suscribimos
			if(client.subscribe(root_topic_subscribe)){
        Serial.println("Suscripcion ok");
      }else{
        Serial.println("fallo Suscripciión");
      }
		} else {
			Serial.print("falló :( con error -> ");
			Serial.print(client.state());
			Serial.println(" Intentamos de nuevo en 5 segundos");
			delay(5000);
		}
	}
}

//*****************************
//***       CALLBACK        ***
//*****************************

void callback(char* topic, byte* payload, unsigned int length){
	String incoming = "";
	Serial.print("Mensaje recibido desde -> ");
	Serial.print(topic);
	Serial.println("");
	for (int i = 0; i < length; i++) {
		incoming += (char)payload[i];
	}
	incoming.trim();
	Serial.println("Mensaje -> " + incoming);
}


//*******************************
//***     SENSOR HUMEDAD      ***
//*******************************

float HumiditySensor() {
	float SensorValue = map(analogRead(SensorHumedad1), 4095, 1500, 0, 10000);
	SensorValue = SensorValue/100; 
	if(SensorValue>100) SensorValue = 100;
	Serial.println(SensorValue);
	delay(300);
	return SensorValue;
}