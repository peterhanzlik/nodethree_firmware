float get_temperature(void) {
  uint16_t key = 67;
  float val = 0.0;

  // Enable SHT sensor supply
  digitalWrite(A0, HIGH);

#if SHT_ON
  Sleepy::loseSomeTime(200);
  val = SHT2x.GetTemperature();
#endif

  digitalWrite(A0, LOW);

  DEBUG_PRINT("Temperature: ");
  DEBUG_LINE(val);

  memcpy(&data[3], &key, sizeof(uint16_t));
  memcpy(&data[5], &val, sizeof(float));
}

float get_humidity(void) {
  uint16_t key = 74;
  float val = 0.0;

  // Enable SHT sensor supply
  digitalWrite(A0, HIGH);

#if SHT_ON
  Sleepy::loseSomeTime(200);
  val = SHT2x.GetHumidity();
#endif

  digitalWrite(A0, LOW);

  DEBUG_PRINT("Humidity: ");
  DEBUG_LINE(val);

  memcpy(&data[3], &key, sizeof(int16_t));
  memcpy(&data[5], &val, sizeof(float));
}
