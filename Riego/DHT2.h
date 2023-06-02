#pragma once

// Archivos de cabecera
  #include <DHT.h>

// Clase sensor de temperatura y humedad ambiente

class DHT2 : public DHT // Esta clase extiende a la clase DHT de la libreria de Adafruit
{
  public:

    // Parametros de esta instancia del sensor DHT

    float temperatura;
    float humedad;
    
  public:

    // Constructor delegado del sensor temperatura y humedad ambiente

    DHT2(uint8_t pin, uint8_t type, uint8_t count = 6) : DHT(pin, type, count)
    {
    }

  // Metodo para actualizar la temperatura y humedad

  private:    
    
    unsigned long t1; // Variable para delay asincrono

   public:

    void medirTH(int intervalo = 0) { // Este metodo toma como argumento un intervalo para actualizar periodicamente los valores de humedad y temperatura
      if (millis() - this->t1 >= intervalo) { 
        t1 = millis();
        // Lecturas
          temperatura = readTemperature();
          humedad = readHumidity();
      }
    } 
};
