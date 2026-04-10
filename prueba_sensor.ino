#include <Wire.h>
void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // (SDA, SCL)
  Serial.println("Escaneando bus I2C...");
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.print("Dispositivo I2C encontrado en direccion 0x");
      Serial.println(address, HEX);
    }
  }
}
void loop() {}
