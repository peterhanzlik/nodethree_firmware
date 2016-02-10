float get_ldr(void) {
  uint16_t key = 75;
  long read_val = 0;
  int analog_reads = 10;
  float val;

  DEBUG_LINE("LDR (phototransistor): ");
  pinMode(A1, OUTPUT);
  pinMode(A2, INPUT);
  digitalWrite(A1, HIGH);

  DEBUG_LINE("Switching voltage reference to read LDR voltage.");
  pinMode(VREF_EN_PIN, OUTPUT);
  digitalWrite(VREF_EN_PIN, HIGH);
  analogReference(EXTERNAL);

  // Stabilize VREF
  Sleepy::loseSomeTime(100);

  for (int a = 0; a < analog_reads; a++)
  {
    analogRead(A2);
    read_val = read_val + analogRead(A2);
  }

  analogReference(DEFAULT);
  digitalWrite(VREF_EN_PIN, LOW);

  DEBUG_LINE("Turning LDR off");
  digitalWrite(A1, LOW);

  val = (double) read_val / (float) analog_reads;

  DEBUG_PRINT("LDR (all readings / average): ");
  DEBUG_PRINT(read_val);
  DEBUG_PRINT(" / ");
  DEBUG_LINE(val);

  memcpy(&data[3], &key, sizeof(uint16_t));
  memcpy(&data[5], &val, sizeof(float));
  return val;
}
