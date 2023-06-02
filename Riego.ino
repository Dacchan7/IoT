#include <ESP8266WiFi.h>
#include <ArduinoWebsockets.h>

String ssid = "RM"; // SSID
String password = "password"; // Password
String websockets_server = "wss://sistema-de-riego-iot.herokuapp.com/riego/"; //server
String id = "23ff0c37-013b-477b-a21f-c0adc4524da6";
String URL = websockets_server + id;

using namespace websockets;

void onMessageCallback(WebsocketsMessage message) {
    Serial.print("Mensaje recibido:");
    Serial.println(message.data());
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
   // Send a ping
  cliente.ping();
  cliente.poll();
}

