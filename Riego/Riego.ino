#include <ESP8266WiFi.h>
#include <ArduinoWebsockets.h>
#include "DHT2.h"
#include "YL_69.h"
#include <ArduinoJson.h>

String ssid = "Spg"; // SSID
String password = "abcd1234"; // Password
String websockets_server = "wss://sistema-de-riego-iot.herokuapp.com/riego/"; //server
String id = "23ff0c37-013b-477b-a21f-c0adc4524da6";
String URL = websockets_server + id;

DHT2 sensorTemperatura(11, DHT11);
YL_69 sensorHumedadSuelo(A0);
#define relePin 13
bool releEstado = false;

int humedad_objetivo = 60;

using namespace websockets;

void onMessageCallback(WebsocketsMessage message) {
    Serial.print("Mensaje recibido:");
    Serial.println(message.data());
    // Crear un objeto JSON
      StaticJsonDocument<200> doc;
    
    // Deserializa el JSON
    DeserializationError error = deserializeJson(doc, message.data());
    if(doc["evento"] == "planta_cambiada")
    {
      humedad_objetivo = doc["humedad_objetivo"];
      Serial.print("Nueva humedad: " + humedad_objetivo);
    }
}

void onEventsCallback(WebsocketsEvent event, String data) {
    if(event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Conexion abierta");
    } else if(event == WebsocketsEvent::ConnectionClosed) {
        Serial.println("Conexion cerrada");
    }
}

WebsocketsClient cliente;

void setup()
{
  Serial.begin(9600);
  Serial.println();

  // Rele

    pinMode(relePin, OUTPUT);
 
  // Sensor de temperatura y humedad ambiente

    sensorTemperatura.begin();

  WiFi.begin("Spg", "abcd1234");

  Serial.print("Conectando");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Conectado, direccion IP: ");
  Serial.println(WiFi.localIP());

   // Callbacks
    cliente.onMessage(onMessageCallback);
    cliente.onEvent(onEventsCallback);
    
    // Conectar
    cliente.connect(URL);
}

void loop() {
  // Primero se hacen las mediciones

    sensorTemperatura.medirTH(5000);
    sensorHumedadSuelo.medirH(500);

  // Send a ping
    cliente.connect(URL);
    cliente.poll();

  if (sensorHumedadSuelo.humedad < humedad_objetivo)
  {
    digitalWrite(relePin, HIGH);
    releEstado = true;
  } else {
    digitalWrite(relePin, LOW);
    releEstado = false;
  }

  
  // Crea un objeto de tipo StaticJsonDocument con una capacidad suficiente para almacenar tu JSON
  StaticJsonDocument<200> doc;

  // Agrega los pares clave-valor al documento
  doc["evento"] = "actualizar_monitor";
  doc["temperatura_medida"] = String(sensorTemperatura.temperatura);
  doc["humedad_medida"] = String(sensorHumedadSuelo.humedad);
  doc["riego_activado"] = String(releEstado);

  // Serializa el JSON
  serializeJson(doc, Serial);
  cliente.send(String(Serial));

}



