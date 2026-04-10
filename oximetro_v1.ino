#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h" // Algoritmo de Maxim

MAX30105 particleSensor;

// Buffers para los datos del sensor
uint32_t irBuffer[100];   // datos del LED infrarrojo
uint32_t redBuffer[100];  // datos del LED rojo

int32_t bufferLength;     // longitud de los buffers
int32_t spo2;             // valor de SpO2
int8_t validSPO2;         // indicador si SpO2 es válido (1 = sí, 0 = no)
int32_t heartRate;        // valor de frecuencia cardíaca
int8_t validHeartRate;    // indicador si la FC es válida

void setup() {
  Serial.begin(115200);
  Serial.println("Inicializando sensor...");

  // Inicializa el sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("ERROR: No se encontró el sensor MAX30102.");
    while (1);
  }

  // Configuración del sensor (valores estándar)
  byte ledBrightness = 0x1F;  // Brillo de los LEDs
  byte sampleAverage = 4;     // Promedio de muestras
  byte ledMode = 2;           // Modo: Red + IR
  int sampleRate = 100;       // Tasa de muestreo (100 samples por segundo)
  int pulseWidth = 411;       // Ancho de pulso
  int adcRange = 4096;        // Rango del ADC

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);

  Serial.println("Sensor listo. Coloca tu dedo sobre el sensor.");
  delay(1000);
}

void loop() {
  bufferLength = 100; // 100 muestras = ~4 segundos a 25sps

  // Lee las primeras 100 muestras
  for (byte i = 0; i < bufferLength; i++) {
    while (particleSensor.available() == false) {
      particleSensor.check(); // Espera nuevos datos
    }
    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample(); // Pasa a la siguiente muestra
  }

  // Calcula la frecuencia cardíaca y SpO2 después de las primeras 100 muestras
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

  // Muestra los resultados por el Monitor Serie
  if (validHeartRate == 1 && validSPO2 == 1) {
    Serial.print("Frecuencia Cardíaca: ");
    Serial.print(heartRate);
    Serial.print(" BPM, SpO2: ");
    Serial.print(spo2);
    Serial.println(" %");
  } else {
    Serial.println("Esperando señal estable...");
  }

  delay(1000);
}
