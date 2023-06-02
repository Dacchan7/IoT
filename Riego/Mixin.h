#pragma once

// Mixin de media movil para suavizar todo tipo de lecturas

template <size_t totalMuestras = 1> // Es una plantilla para que el array del suavizado sea de tamaño "dinamico"
  class MediaMovilMixin
  {
    // Media movil

    public:

      int indice = 0; // Indice de la lectura actual
      float buffer[totalMuestras]; // Buffer de almacenamiento lecturas
      float suma; // Suma de todas las distancias medidas para calcular el promedio
      bool activo = false; // Activa/desactiva el suavizado. Al inicio lo desactiva para no comenzar con lecturas erroneas al comenzar los sensores
  
      float mediaMovil(float lecturaActual) // Metodo para suavizar lecturas 
      {
        activo = indice <= totalMuestras || activo ? true : false; // Desactiva el suavizado al incio para evitar lecturas erroneas y lentas

        if(!activo){
          return lecturaActual;
        } else {
          suma += lecturaActual - buffer[indice]; // Se suma la lectura entrante y se resta la lectura saliente del buffer
          buffer[indice] = lecturaActual; // Se introduce la nueva lectura al buffer y se saca la mas antigua
          indice = (indice + 1) % totalMuestras; // Indice circular
          return suma / totalMuestras; // Finalmente se calcula la media
        }
      }
  };