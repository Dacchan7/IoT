#include <ESP8266WiFi.h>
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>

#include "YL_69.h"
#include "DHT2.h"

String ssid = "CLARO1_D508E8"; // SSID
String password = "92941mJaRH"; // Password
String websockets_server = "wss://sistema-de-riego-iot.herokuapp.com/riego/"; //server
String id = "23ff0c37-013b-477b-a21f-c0adc4524da6";
String URL = websockets_server + id;

#define DHTPIN D2     // Aquí puedes cambiar el pin a otro si es necesario. En este ejemplo, usamos el pin D2.
#define DHTTYPE DHT11  

YL_69<6> sensorHumedadSuelo(A0, 940, 0);
DHT2 sensorTemperatura(DHTPIN, DHTTYPE);

int humedad_objetivo, temperatura_maxima;
int relePin = D0;
bool releEstado = false;

using namespace websockets;

WebsocketsClient cliente;

void onMessageCallback(WebsocketsMessage message) {
    Serial.print("Mensaje recibido:");
    Serial.println(message.data());
    
    StaticJsonDocument<500> json;
    DeserializationError error = deserializeJson(json, message.data());
    
    humedad_objetivo = int(json["humedad_objetivo"]);
    temperatura_maxima = int(json["temperatura_maxima"]);
    Serial.println("Planta cambiada a " + String(json["tipo"]) + ". Humedad objetivo: " + String(humedad_objetivo) + ", temperatura maxima: " + String(temperatura_maxima));
    
}

void onEventsCallback(WebsocketsEvent event, String data) {
    if(event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Conexion abierta");
    } else if(event == WebsocketsEvent::ConnectionClosed) {
        Serial.println("Conexion cerrada");
        cliente.connect(URL);
    }
}

void setup()
{
  
  Serial.begin(9600);
  Serial.println();

  pinMode(relePin, OUTPUT);

  sensorTemperatura.begin();
  WiFi.begin(ssid, password);

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
unsigned long tsend;

#define T 0
#define V 1
#define H 2
#define R 3

String notificaciones[4];

void loop() {
  sensorHumedadSuelo.medirH(500);
  sensorTemperatura.readTemperature(5000);
  cliente.poll();
    
  if (sensorHumedadSuelo.humedad < humedad_objetivo) // Si la humedad del suelo es baja o la temperatura alta y no esta bloqueado
  {
    releEstado = true; // Entonces activa el regado
  } else {
    releEstado = false; // Si esta bloqueado o las condiciones son optimas entonces se desactiva el regado
  }

    digitalWrite(relePin, !releEstado);

    if (millis()-tsend >= 1000)
  {
    tsend = millis();

    DynamicJsonDocument json(700);
    json["evento"] = "actualizar_monitor";
    json["humedad_medida"] = sensorHumedadSuelo.humedad;
    json["temperatura_medida"] = sensorTemperatura.temperatura > 10 ? sensorTemperatura.temperatura : 31;
    json["riego_activado"] = releEstado;

    String jsonString;
    serializeJson(json, jsonString);

    // Serial.println("Mensaje enviado: " + jsonString);
    cliente.send(jsonString);
  }
}



