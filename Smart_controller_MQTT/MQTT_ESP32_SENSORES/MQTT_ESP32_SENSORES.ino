/*
 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/
#include <Wire.h>
#include <Adafruit_BME680.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define R1 25
#define R2 26
#define R3 33
#define R4 32

//--------------VARIABLES DE LECTURAS DE SENSORES-------------------------//

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

//-----------------CONFIGURACIÓN DE PROTOCOLO MQTT--------------------------//

// Datos de la red WiFi y del servidor MQTT

const char* ssid = "DANIEL 25";            // Nombre de la red WiFi
const char* password = "C1u2a3r8e7s6m5a";        // Contraseña de la red WiFi
const char* mqtt_server = "192.168.1.61";  // Dirección del servidor MQTT

WiFiClient espClient;       // Cliente WiFi
PubSubClient client(espClient);  // Cliente MQTT utilizando el cliente WiFi
unsigned long lastMsg = 0;   // Variable para almacenar el tiempo del último mensaje
#define MSG_BUFFER_SIZE (50) // Tamaño máximo del buffer de mensajes
char msg[MSG_BUFFER_SIZE];   // Buffer para almacenar el mensaje
int value = 0;               // Variable para el valor a enviar

// Configuración inicial del dispositivo
void setup() {
  pinMode(2, OUTPUT);     // Configura el pin del LED incorporado como salida
  Serial.begin(115200);   // Inicializa la comunicación serie a 115200 baudios
  setup_wifi();           // Llama a la función para conectar a la red WiFi
  client.setServer(mqtt_server, 1883);  // Configura el servidor MQTT y el puerto
  client.setCallback(callback);         // Asigna la función de callback para manejar los mensajes MQTT
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
    //while (1);
  }

  // Iniciar el segundo sensor (dirección I2C 0x77)
  if (!bme2.begin(0x77)) {
    Serial.println("No se encontró el sensor BME680 en la dirección 0x77");
    u8g2.drawStr(0, 20, "No se encontró el sensor BME680 en la dirección 0x76");
    u8g2.sendBuffer();
    //while (1);
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
// Bucle principal del programa
void loop() {
  temp1 = random(20,31);
  temp2 = random(15,21);
  hum1 = random(90,97);
  hum2 = random(80,85);
  gas1 = random(80,100);
  gas2 = random(80, 91);
  pres1 = random(900, 1013);
  pres2 = random(900, 1013);
  // Leer los valores de ambos sensores
  //leerBME(bme1); 
  //leerBME(bme2);
  //printOled();
  
  //relayController();
  // Imprimir los resultados
  //printData();
  
  //delay(delayTime); // Espera antes de la siguiente lectura

  // Verifica si está conectado al servidor MQTT
  if (!client.connected()) {
    reconnect();  // Llama a la función de reconexión si no está conectado
  }
  client.loop();  // Mantiene la conexión al servidor MQTT activa

  unsigned long now = millis();  // Obtiene el tiempo actual en milisegundos
  if (now - lastMsg > 500) {    // Verifica si han pasado 2 segundos desde el último mensaje
    lastMsg = now;               // Actualiza el tiempo del último mensaje
    ++value;                     // Incrementa el valor del mensaje
    snprintf(msg, MSG_BUFFER_SIZE, "%.f",temp1);  // Formatea el mensaje para enviar
    Serial.print("Publish message: ");  // Muestra el mensaje a enviar en el monitor serie
    Serial.println(msg);
    client.publish("T1", msg);    // Publica el mensaje en el tema MQTT "outTopic"
    snprintf(msg, MSG_BUFFER_SIZE, "%.f",temp2);  // Formatea el mensaje para enviar
    Serial.print("Publish message: ");  // Muestra el mensaje a enviar en el monitor serie
    Serial.println(msg);
    client.publish("T2", msg);    // Publica el mensaje en el tema MQTT "outTopic"
    snprintf(msg, MSG_BUFFER_SIZE, "%.f",hum1);  // Formatea el mensaje para enviar
    Serial.print("Publish message: ");  // Muestra el mensaje a enviar en el monitor serie
    Serial.println(msg);
    client.publish("H1", msg);    // Publica el mensaje en el tema MQTT "outTopic"
    snprintf(msg, MSG_BUFFER_SIZE, "%.f",hum2);  // Formatea el mensaje para enviar
    Serial.print("Publish message: ");  // Muestra el mensaje a enviar en el monitor serie
    Serial.println(msg);
    client.publish("H2", msg);    // Publica el mensaje en el tema MQTT "outTopic"
    snprintf(msg, MSG_BUFFER_SIZE, "%.f",gas1);  // Formatea el mensaje para enviar
    Serial.print("Publish message: ");  // Muestra el mensaje a enviar en el monitor serie
    Serial.println(msg);
    client.publish("C1", msg);    // Publica el mensaje en el tema MQTT "outTopic"
    snprintf(msg, MSG_BUFFER_SIZE, "%.f",gas2);  // Formatea el mensaje para enviar
    Serial.print("Publish message: ");  // Muestra el mensaje a enviar en el monitor serie
    Serial.println(msg);
    client.publish("C2", msg);    // Publica el mensaje en el tema MQTT "outTopic"
     snprintf(msg, MSG_BUFFER_SIZE, "%.f",pres1);  // Formatea el mensaje para enviar
    Serial.print("Publish message: ");  // Muestra el mensaje a enviar en el monitor serie
    Serial.println(msg);
    client.publish("P1", msg);    // Publica el mensaje en el tema MQTT "outTopic"
     snprintf(msg, MSG_BUFFER_SIZE, "%.f",pres2);  // Formatea el mensaje para enviar
    Serial.print("Publish message: ");  // Muestra el mensaje a enviar en el monitor serie
    Serial.println(msg);
    client.publish("P2", msg);    // Publica el mensaje en el tema MQTT "outTopic"
    
  }
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

// Función para conectar el dispositivo a la red WiFi
void setup_wifi() {

  delay(10);  // Pausa para iniciar la conexión
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);  // Muestra el nombre de la red a la que intenta conectar

  WiFi.mode(WIFI_STA);  // Configura el modo del WiFi en "cliente" (Station)
  WiFi.begin(ssid, password);  // Inicia la conexión a la red WiFi

  // Espera hasta que el dispositivo se conecte a la red
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");  // Muestra puntos para indicar que sigue intentando conectar
  }

  randomSeed(micros());  // Inicia el generador de números aleatorios con una semilla basada en el tiempo

  Serial.println("");
  Serial.println("WiFi connected");       // Indica que se ha conectado al WiFi
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());         // Muestra la dirección IP asignada al dispositivo
}

// Función de callback para manejar los mensajes entrantes del servidor MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);         // Muestra el tema del mensaje recibido
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);  // Muestra el contenido del mensaje
  }
  Serial.println();

  // Enciende o apaga el LED según el contenido del mensaje
  if ((char)payload[0] == '1') {
    digitalWrite(2, LOW);   // Enciende el LED (activo en nivel bajo)
  } else {
    digitalWrite(2, HIGH);  // Apaga el LED
  }
}

// Función para reconectar al servidor MQTT en caso de desconexión
void reconnect() {
  // Intenta reconectar hasta que la conexión sea exitosa
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");  // Mensaje indicando que intenta conectar
    // Crea un ID de cliente aleatorio para el dispositivo
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);  // Agrega un número aleatorio en hexadecimal al ID
    // Intenta conectar al servidor MQTT
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");  // Mensaje indicando que la conexión fue exitosa
      // Una vez conectado, publica un mensaje de anuncio...
      //client.publish("outTopic", "hello world");
      // ... y se suscribe a un tema
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");  // Mensaje indicando que la conexión falló
      Serial.print(client.state());  // Muestra el código de error de la conexión
      Serial.println(" try again in 5 seconds");  // Mensaje indicando que reintentará en 5 segundos
      // Espera 5 segundos antes de intentar reconectar
      delay(5000);
    }
  }
}

