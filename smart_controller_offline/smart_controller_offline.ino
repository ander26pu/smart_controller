// Script: Smart Controller Offline V1.0
// Description: Lee data de 2 BME680 y con esos parámetros toma acciones en base a la temperatura, humedad, calidad del aire
//              acciona los respectivos actuadores
// Autor: @ander26pu
// Fecha: 2024-09-22
// Board: ESP32R4 V2

#include <Wire.h>
#include <Adafruit_BME680.h>
#include <U8g2lib.h>

#define R1 25
#define R2 26
#define R3 33
#define R4 32

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// Crear objetos para los dos sensores BME680
Adafruit_BME680 bme1;  // Primer sensor (dirección 0x76)
Adafruit_BME680 bme2;  // Segundo sensor (dirección 0x77)

// Variables globales para almacenar las lecturas
float temp1, hum1, pres1, gas1;
float temp2, hum2, pres2, gas2;

// Límites para el control de los relés
float temp_min = 0.0, temp_max = 15.0;
float hum_min = 70.0, hum_max = 85.0;
float gas_threshold = 300.0; // Calidad de aire

// Configurar el tiempo de muestreo para los sensores (en milisegundos)
unsigned long delayTime = 1000;

void setup() {
  Serial.begin(115200);
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 20, "Wellcome!"); 
  u8g2.sendBuffer();
  delay(1000);
  u8g2.clearBuffer();

  // Iniciar el primer sensor (dirección I2C 0x76)
  if (!bme1.begin(0x76)) {
    Serial.println("No se encontró el sensor BME680 en la dirección 0x76");
    u8g2.drawStr(0, 20, "No se encontró el sensor BME680 en la dirección 0x76");
    u8g2.sendBuffer();
    while (1);
  }

  // Iniciar el segundo sensor (dirección I2C 0x77)
  if (!bme2.begin(0x77)) {
    Serial.println("No se encontró el sensor BME680 en la dirección 0x77");
    u8g2.drawStr(0, 20, "No se encontró el sensor BME680 en la dirección 0x76");
    u8g2.sendBuffer();
    while (1);
  }
  // Configurar los sensores BME680
  configurarSensor(bme1);
  configurarSensor(bme2);
  // Configurar pines de los relés
  pinMode(R1,OUTPUT);
  pinMode(R2,OUTPUT);
  pinMode(R3,OUTPUT);
  pinMode(R4,OUTPUT);
  // Inicialmente, apagar los relés
  digitalWrite(R1, LOW);
  digitalWrite(R2, LOW);
  digitalWrite(R3, LOW);
  digitalWrite(R4, LOW);
}

void loop() {
  // Leer los valores de ambos sensores
  leerBME(bme1);
  leerBME(bme2);
  printOled();
  
  relayController();
  // Imprimir los resultados
  printData();
  
  delay(delayTime); // Espera antes de la siguiente lectura
}

// Función para configurar el sensor
void configurarSensor(Adafruit_BME680 &bme) {
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320°C durante 150 ms
}

// Función para leer los valores del primer sensor
void leerBME(Adafruit_BME680 &bme) {
  if (!bme.performReading()) {
    Serial.println("Error al leer del sensor 1");
    return;
  }

  temp1 = bme.temperature;
  hum1 = bme.humidity;
  pres1 = bme.pressure / 100.0; // Convertir a hPa
  gas1 = bme.gas_resistance / 1000.0; // Convertir a ohms
}

void relayController(){
  // Control de temperatura
  float avgTemp = (temp1 + temp2) / 2;
  
  if (avgTemp > temp_max) {
    digitalWrite(R1, HIGH); // Encender R1 si la temperatura promedio es mayor que el máximo
  } else {
    digitalWrite(R1, LOW); // Apagar R1 si está entre los límites
  }

  // Control de humedad
  float avgHum = (hum1 + hum2) / 2;

  if (avgHum < hum_min) {
    digitalWrite(R2, HIGH); // Encender R2 si la humedad promedio es menor que el mínimo
    digitalWrite(R3, HIGH); // Encender R2 si la humedad promedio es menor que el mínimo
  } else {
    digitalWrite(R2, LOW); // Apagar R2 si la humedad está por encima del mínimo
    digitalWrite(R3, LOW); // Apagar R2 si la humedad está por encima del mínimo
  }


  // Control de calidad del aire
  if (gas1 > gas_threshold || gas2 > gas_threshold) {
    digitalWrite(R1, HIGH); // Encender R3 si la calidad del aire es mala
  } else {
    digitalWrite(R1, LOW);  // Apagar R3 si la calidad del aire es buena
  }
}
// Función para mostrar los datos en la pantalla OLED
void printOled() {
  u8g2.clearBuffer();
  
  u8g2.setCursor(0, 10);
  u8g2.printf("T1: %.1f C  H1: %.1f%%", temp1, hum1);
  u8g2.setCursor(0, 20);
  u8g2.printf("T2: %.1f C  H2: %.1f%%", temp2, hum2);
  u8g2.setCursor(0, 30);
  u8g2.printf("Gas1: %.0f ohm", gas1);
  u8g2.setCursor(0, 40);
  u8g2.printf("Gas2: %.0f ohm", gas2);

  u8g2.setCursor(0, 50);
  u8g2.printf("R1: %s R2: %s", digitalRead(R1) ? "ON" : "OFF", digitalRead(R2) ? "ON" : "OFF");
  u8g2.setCursor(0, 60);
  u8g2.printf("R3: %s R4: %s", digitalRead(R3) ? "ON" : "OFF", digitalRead(R4) ? "ON" : "OFF");
  
  u8g2.sendBuffer();
}

void printData(){
  Serial.print("Sensor 1 - Temp: "); Serial.print(temp1);
  Serial.print(" °C, Hum: "); Serial.print(hum1);
  Serial.print(" %, Pres: "); Serial.print(pres1);
  Serial.print(" hPa, Gas: "); Serial.print(gas1); Serial.println(" ohm");

  Serial.print("Sensor 2 - Temp: "); Serial.print(temp2);
  Serial.print(" °C, Hum: "); Serial.print(hum2);
  Serial.print(" %, Pres: "); Serial.print(pres2);
  Serial.print(" hPa, Gas: "); Serial.print(gas2); Serial.println(" ohm");
}


