#include <Manchester.h>

#define RX_PIN 4

uint8_t data_id = 0;

void setup() {
  Serial.begin(9600);
  man.setupReceive(RX_PIN, MAN_1200);
  man.beginReceive();
}

void loop() {
  if (man.receiveComplete()) {
    uint16_t receivedData = man.getMessage();
    man.beginReceive();

    if (data_id == 0) {
      if (receivedData == 1 || receivedData == 2 || receivedData == 3) {
        data_id = receivedData;
      }
    } else {
      if (data_id == 1) {
        Serial.print("T:");
        Serial.println(((int16_t)receivedData) / 100.0);
      } else if (data_id == 2) {
        Serial.print("L:");
        Serial.println(receivedData);
      } else if (data_id == 3) {
        Serial.print("S:");
        Serial.println(receivedData);
      }
      data_id = 0;
    }
  }
}