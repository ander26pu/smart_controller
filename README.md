# Smart Controller ESP32 🌟

Bienvenido a este emocionante proyecto basado en el microcontrolador **ESP32R4 (RobotDyn)**. 🚀 Este sistema está diseñado para recolectar datos de varios sensores conectados a través de I2C y controlar actuadores de manera automática usando MQTT. 🛰️✨

## 🚀 **Características principales**
- 🔌 **Control de actuadores** por MQTT.
- 📡 **Recepción de datos** de sensores conectados por I2C a través de MQTT.
- 🤖 **Automatización** de los relés según los valores de los sensores.
- 📊 Compatible con los sensores:
  - 🌡️ **BME280**: Temperatura, presión y humedad.
  - 💨 **BME680**: Calidad del aire y medición de gases.
  - 🌬️ **CCS811**: Monitorización de gases como CO2 y COV.
  - 🖥️ **OLED 1.54 pulgadas**: Visualización de los datos en tiempo real.

## 📋 **Requisitos del hardware**
- 🧠 **ESP32R4 (RobotDyn)**.
- 🌡️ **BME280** - Sensor de temperatura, presión y humedad.
- 💨 **BME680** - Sensor de gases.
- 🌬️ **CCS811** - Sensor de calidad del aire.
- 🖥️ **Pantalla OLED de 1.54 pulgadas**.
- 🔌 **Actuadores** (relés, ventiladores, etc.).
- 🌐 Conexión a internet para el envío y recepción de datos por MQTT.

## 🛠️ **Configuración del software**
1. ⚙️ **Clona** este repositorio:
   ```bash
   git clone https://github.com/tuusuario/tuproyecto.git

2. 📦 **Instala** las bibliotecas necesarias en el IDE de Arduino o PlatformIO:

* Adafruit BME280 Library.
* Adafruit BME680 Library.
* Adafruit CCS811 Library.
* U8g2 para la pantalla OLED.
* PubSubClient para manejar MQTT.
3. 📝 **Configura** las credenciales de WiFi y MQTT en el archivo de configuración:

cpp
Copiar código
const char* ssid = "tu_wifi";
const char* password = "tu_contraseña";
const char* mqtt_server = "broker.emqx.io";
🚀 Sube el código al ESP32R4 y asegúrate de que todos los sensores estén conectados correctamente.

📡 Cómo funciona el sistema
Sensores: Los sensores I2C envían datos al ESP32R4, que luego se transmiten mediante MQTT al broker.
Actuadores: Los actuadores son controlados por comandos MQTT enviados desde un sistema remoto o una interfaz gráfica.
Automatización: Un programa embebido toma decisiones automáticas basadas en los valores de los sensores para activar o desactivar los relés.
🛠️ Circuito
Aquí te dejamos una referencia del circuito:

🧠 ESP32R4 conectado a los sensores BME280, BME680, y CCS811 por I2C.
🖥️ Pantalla OLED mostrando los valores en tiempo real.
🔌 Relés conectados a dispositivos que se activan o desactivan según los datos.
🌐 MQTT Tópicos
Sensores:
sensors/temperature
sensors/humidity
sensors/pressure
sensors/air_quality
Actuadores:
actuators/relay1
actuators/relay2
🤖 Automatización
El sistema tomará decisiones basadas en los valores de los sensores. Por ejemplo:

Si la temperatura supera los 30°C, se activa un ventilador.
Si la calidad del aire disminuye, se encienden sistemas de purificación.
🛡️ Contribuciones
¡Nos encantaría que contribuyeras! 🤗 Si tienes ideas para mejorar el proyecto, abre un issue o envía un pull request.

💬 Contacto
Si tienes alguna pregunta o sugerencia, no dudes en contactarnos a través de GitHub Issues o por correo. 📧