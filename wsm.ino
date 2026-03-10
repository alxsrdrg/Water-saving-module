#include <Manchester.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#define TX_PIN 1 
#define ONE_WIRE_BUS 3
#define LDR_PIN A1
#define SOIL_PIN A0

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

volatile int watchdog_counter = 0;

ISR(WDT_vect) {
  watchdog_counter++;
}

void setup_watchdog() {
  MCUSR &= ~(1<<WDRF);
  WDTCR |= (1<<WDCE) | (1<<WDE);
  WDTCR = (1<<WDIE) | (1<<WDP3) | (1<<WDP0);
}

void enter_sleep() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();
  sleep_disable(); 
}

void setup() {
  ADCSRA &= ~(1<<ADEN); 
  
  man.setupTransmit(TX_PIN, MAN_1200);
  sensors.begin();
  setup_watchdog();
}

void loop() {
  if (watchdog_counter >= 225 || watchdog_counter == 0) {
    watchdog_counter = 0;

    sensors.requestTemperatures();
    float tempC = sensors.getTempCByIndex(0);
    int16_t tempToSend = (int16_t)(tempC * 100);

    ADCSRA |= (1<<ADEN);
    delay(10);
    int16_t ldrToSend = analogRead(LDR_PIN);
    int16_t soilToSend = analogRead(SOIL_PIN);
    ADCSRA &= ~(1<<ADEN);

    for (int i = 0; i < 3; i++) {
      man.transmit(1);
      man.transmit(tempToSend);
      
      man.transmit(2);
      man.transmit(ldrToSend);
      
      man.transmit(3);
      man.transmit(soilToSend);
      
      delay(50); 
    }

    digitalWrite(TX_PIN, LOW);
  }

  enter_sleep();
}