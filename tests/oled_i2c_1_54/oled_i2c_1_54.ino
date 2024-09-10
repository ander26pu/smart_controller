#include <U8g2lib.h>
#include <Wire.h>

// Prueba con otro controlador, por ejemplo, el SSD1306
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void setup(void) {
  u8g2.begin();
  u8g2.clearBuffer();                  
  u8g2.setFont(u8g2_font_ncenB08_tr);  
  u8g2.drawStr(0, 20, "Hello, ESP32!"); 
  u8g2.sendBuffer();                   
}

void loop(void) {
  // Aquí puedes agregar más funcionalidad si lo deseas
  delay(1000);  // Espera 1 segundo
}
