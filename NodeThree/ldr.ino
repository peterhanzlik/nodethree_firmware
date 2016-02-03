float get_ldr(void) {
  uint16_t key = 75;
  float read_val = 0.0;
  float val;

  DEBUG_LINE("LDR (phototransistor): ");
  
  pinMode(A1, OUTPUT);
  pinMode(A2, INPUT);
  digitalWrite(A1, HIGH);
  Sleepy::loseSomeTime(100);

  for (int a = 0; a < 32; a++)
  {
    analogRead(A2);
    read_val += analogRead(A2);
  }

  digitalWrite(A1, LOW);

  val = read_val / 32.0;

#if DEBUG
  DEBUG_PRINT("LDR (32 readings / average): ");
  DEBUG_PRINT(read_val);
  DEBUG_PRINT(" / ");
  DEBUG_LINE(val);
#endif

  memcpy(&data[3], &key, sizeof(int16_t));
  memcpy(&data[5], &val, sizeof(float));
  return val;
}
