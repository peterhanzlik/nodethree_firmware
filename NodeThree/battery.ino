float get_battery(void) {
  uint16_t key = 66;
  float read_val = 0.0;
  float val;

  DEBUG_LINE("Switching voltage reference to read battery voltage.");
  
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
  DEBUG_PRINT("Battery reading (10 readings / average): ");
  DEBUG_PRINT(read_val);
  DEBUG_PRINT(" / ");
  DEBUG_LINE(val);
#endif

  memcpy(&data[3], &key, sizeof(int16_t));
  memcpy(&data[5], &val, sizeof(float));
  return val;
}
