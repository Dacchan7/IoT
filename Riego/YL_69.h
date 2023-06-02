#pragma once

 // Es una plantilla para que el array del suavizado sea de tamaño "dinamico"
  class YL_69 // Usa el mixin para suavizar las lecturas
  {
    public:
      
      //Atributos

      int pinAn; // Pin de lectura analogica
      int lectura; // Lectura en crudo
      float humedad; // Humedad relativa
      int calibracionAnalogicaMax, calibracionAnalogicaMin; // Valores de calibracion analogicos de humedad maxima y minima para calcular la humedad relativa  

      // Constructor del sensor de humedad del suelo

      YL_69(int pinAnalogico, int calibracionAnalogicaMax = 1020, int calibracionAnalogicaMin = 370)
      {
        pinAn = pinAnalogico;
        this->calibracionAnalogicaMax = calibracionAnalogicaMax; 
        this->calibracionAnalogicaMin = calibracionAnalogicaMin;
      }

      private:

        unsigned long t1; //  Variable para delay asincrono

      public:

        // Metodo para medir la humedad del suelo

          void medirH(int intervalo = 0) 
          {

              lectura = analogRead(pinAn); // Lectura en crudo

              // Procesamiento de la lectura en crudo

                int preHumedad = (float) floor(map(lectura, this->calibracionAnalogicaMax, this->calibracionAnalogicaMin, 0, 10000)) / 100; // A humedad relativa
                preHumedad = preHumedad < 0 ? 0 : preHumedad; // Trunca la lectura de humedad al 0.00%
                preHumedad = preHumedad > 100.00 ? 100.00 : preHumedad; // Trunca la lectura de humedad al 100.00% 

                preHumedad = preHumedad;

              // Actualizacion del valor de humedad con la lectura procesada

                if (millis() - this->t1 >= intervalo) {
                  t1 = millis();
                  humedad = preHumedad;
                }
        }

  };