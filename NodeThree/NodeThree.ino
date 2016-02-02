#define LED_PIN 4
#define VREF_EN_PIN 8

#define SHT_ON 1
#define RADIO_ON 1
#define DEBUG 0

#include <SPI.h>
#include <RH_RF95.h>
#include <Wire.h>
#include <Sodaq_SHT2x.h>
#include <JeeLib.h>

uint8_t data[11] = { 0x80, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a };

uint32_t serial = 1;
int this_loop = 0;

ISR(WDT_vect) {
  Sleepy::watchdogEvent();
}

// Singleton instance of the radio driver
RH_RF95 rf95;

void setup() {
  // set serial number to the packet template
  memcpy(&data[3], &serial, sizeof(uint32_t));

  // SHT power supply
  pinMode(A0, OUTPUT);
  digitalWrite(A0, LOW);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  Wire.begin();
#if DEBUG
  Serial.begin(115200);
  Serial.println("DEBUG MODE");
#endif

#if RADIO_ON
  if (!rf95.init()) {
    while (true) {
#if DEBUG
      Serial.println("debug|init of rfm module failed");
#endif
      digitalWrite(LED_PIN, HIGH);
      Sleepy::loseSomeTime(300);
      digitalWrite(LED_PIN, LOW);
      Sleepy::loseSomeTime(300);
    }
  } else {
    rf95.setFrequency(434.0);
#if DEBUG
    Serial.println("debug|init OK");
#endif
  }
  Sleepy::loseSomeTime(300);
  // rf95.printRegisters();
  Sleepy::loseSomeTime(1000);
#endif

  digitalWrite(LED_PIN, LOW);
}


void loop() {
  this_loop++;
#if DEBUG
  Serial.print("Loop number: ");
  Serial.println(this_loop);
  Serial.flush();
#endif

  get_temperature();
  transmit();
  Sleepy::loseSomeTime(30000);

  get_humidity();
  transmit();
  Sleepy::loseSomeTime(30000);

  if (this_loop > 100) {
    get_battery();
    transmit();
    Sleepy::loseSomeTime(20000);
    this_loop = 0;
  }
}

void transmit() {
#if DEBUG
  for (int i = 0; i < sizeof(data); i++) {
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
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
  Serial.println("End of TX");
  Serial.flush();
#endif
#endif
}

float get_temperature(void) {
  uint16_t key = 1;
  float val = 0.0;

  // Enable SHT sensor supply
  digitalWrite(A0, HIGH);

#if SHT_ON
  Sleepy::loseSomeTime(200);
  val = SHT2x.GetTemperature();
#endif

  digitalWrite(A0, LOW);

#if DEBUG
  Serial.print("Temperature: ");
  Serial.println(val);
#endif

  memcpy(&data[1], &key, sizeof(uint16_t));
  memcpy(&data[7], &val, sizeof(float));
}

float get_humidity(void) {
  uint16_t key = 2;
  float val = 0.0;

  // Enable SHT sensor supply
  digitalWrite(A0, HIGH);

#if SHT_ON
  Sleepy::loseSomeTime(200);
  val = SHT2x.GetHumidity();
#endif

  digitalWrite(A0, LOW);

#if DEBUG
  Serial.print("Humidity: ");
  Serial.println(val);
#endif

  memcpy(&data[1], &key, sizeof(int16_t));
  memcpy(&data[7], &val, sizeof(float));
}

float get_battery(void) {
  uint16_t key = 0;
  float read_val = 0.0;
  float val;

#if DEBUG
  Serial.println("Switching voltage reference to read battery voltage.");
#endif
  pinMode(VREF_EN_PIN, OUTPUT);
  digitalWrite(VREF_EN_PIN, HIGH);
  analogReference(EXTERNAL);
  Sleepy::loseSomeTime(100);

  for (int a = 0; a < 10; a++)
  {
    analogRead(A6);
    read_val += analogRead(A6);
  }

  analogReference(DEFAULT);
  digitalWrite(VREF_EN_PIN, LOW);

  val = read_val / 10.0;

#if DEBUG
  Serial.print("Battery reading (10 readings / average): ");
  Serial.print(read_val);
  Serial.print(" / ");
  Serial.println(val);
#endif

  memcpy(&data[1], &key, sizeof(int16_t));
  memcpy(&data[7], &val, sizeof(float));
  return val;
}


