#define LED_PIN 4
#define VREF_EN_PIN 8

#define SHT_ON 1
#define RADIO_ON 1
#define DEBUG 0

#if DEBUG
#define DEBUG_LINE(thing)  Serial.println(thing); Serial.flush();
#define DEBUG_PRINT(thing) Serial.print(thing);   Serial.flush();
#define DEBUG_FLUSH()      Serial.flush();
#else
#define DEBUG_LINE(thing)  /* No output */
#define DEBUG_PRINT(thing) /* No output */
#define DEBUG_FLUSH()      /* No output */
#endif

#include <SPI.h>
#include <RH_RF95.h>
#include <Wire.h>
#include <Sodaq_SHT2x.h>
#include <JeeLib.h>

uint8_t data[9] = { 0x80, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };

uint32_t serial = 1;
int this_loop = 0;

ISR(WDT_vect) {
  Sleepy::watchdogEvent();
}

// Singleton instance of the radio driver
RH_RF95 rf95;

void setup() {
  // set serial number to the packet template
  memcpy(&data[1], &serial, sizeof(uint32_t));

  // SHT power supply
  pinMode(A0, OUTPUT);
  digitalWrite(A0, LOW);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  Wire.begin();

#if DEBUG
  Serial.begin(115200);
  DEBUG_LINE("DEBUG MODE");
  DEBUG_PRINT("Serial no: ");
  DEBUG_LINE(serial);
#endif

#if RADIO_ON
  if (!rf95.init()) {
    while (true) {
      DEBUG_LINE("debug|init of rfm module failed");
      digitalWrite(LED_PIN, HIGH);
      Sleepy::loseSomeTime(300);
      digitalWrite(LED_PIN, LOW);
      Sleepy::loseSomeTime(300);
    }
  } else {
    rf95.setFrequency(868.0);
    rf95.setTxPower(13);
    DEBUG_LINE("debug|init OK");
  }
#if DEBUG
  rf95.printRegisters();
#endif
  Sleepy::loseSomeTime(1000);
#endif

  digitalWrite(LED_PIN, LOW);
}


void loop() {
  this_loop++;

  DEBUG_PRINT("Loop number: ");
  DEBUG_LINE(this_loop);

  get_ldr();
  transmit();
  Sleepy::loseSomeTime(50000);

  get_temperature();
  transmit();
  Sleepy::loseSomeTime(50000);

  get_humidity();
  transmit();
  Sleepy::loseSomeTime(50000);

  //  if (this_loop > 100) {
  get_battery();
  transmit();
  Sleepy::loseSomeTime(50000);
  this_loop = 0;
  //  }
}

void transmit() {
#if DEBUG
  for (int i = 0; i < sizeof(data); i++) {
    Serial.print(data[i], HEX);
    DEBUG_PRINT(" ");
  }
  DEBUG_LINE();
#endif

#if RADIO_ON
#if DEBUG
  digitalWrite(LED_PIN, HIGH);
#endif
  rf95.send(data, sizeof(data));
  rf95.waitPacketSent();
  rf95.sleep();
#if DEBUG
  digitalWrite(LED_PIN, LOW);
  DEBUG_LINE("End of TX");
#endif
#endif
}
