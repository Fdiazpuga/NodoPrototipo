#include <WiFi.h>
#include <PubSubClient.h>
#include <stdlib.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//**************************************
//*********** MQTT CONFIG **************
//**************************************

const char* mqttServer = "192.168.1.40";
const int mqttPort = 1883;
const char* mqttUser = "dqyoxjgo";
const char* mqttPassword = "bDFPyIOx5Mln";
const char* root_topic_subscribe ="bazuca/input";
//const char* root_topic_publish ="bazuca/output";


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
char msg1[25];
const int SensorHumedad1 = 33; //mido la humedad al analogico GPIO33

// GPIO where the DS18B20 is connected to
const int oneWireBus = 32;    
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);


//************************
//** F U N C I O N E S ***
//************************

//Prototipo de funciones
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void setup_wifi();
float HumiditySensor();
float TemperatureSensor();


//SETUP
void setup() {
	// Start the Serial Monitor
	Serial.begin(115200);
	// Start the DS18B20 sensor
  	sensors.begin();
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
    //** Envío Humedad **
	String str = String(HumiditySensor()) + " %";
    str.toCharArray(msg,25);
    //client.publish(root_topic_publish,msg);
	client.publish("prototipo/humedad",msg);
	//Serial.println(TemperatureSensor());

	//** Envío Temperatura **
	sensors.requestTemperatures(); 
	float temperatureC = sensors.getTempCByIndex(0);
	
	String str1 = String(temperatureC) + " °C";
    str1.toCharArray(msg1,25);
	client.publish("prototipo/temperatura",msg1);
	//Serial.print(temperatureC);
  	//Serial.println("ºC");
	//Serial.println(msg1);

	//delay(300);

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
	float SensorValue = map(analogRead(SensorHumedad1), 4095, 1950, 0, 10000);
	SensorValue = SensorValue/100; 
	if(SensorValue>100) SensorValue = 100;
	//Serial.println(analogRead(SensorHumedad1));
	//Serial.println(SensorValue);
	//delay(300);
	return SensorValue;
}

//**********************************
//***     SENSOR TEMPERATURA     ***
//**********************************
