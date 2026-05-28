// ============================================================
//  PULSAR ESP32-C6 — Interrupción por flanco de SUBIDA (RISING)
//  Conexión: botón entre D7 y GND
//  Con INPUT_PULLUP: RISING ocurre al SOLTAR el botón
// ============================================================

const int PIN_INT = 7;    // D7 del PULSAR
const int PIN_LED = 6;    // LED integrado GPIO6

volatile bool     interrupcionOcurrida = false;
volatile unsigned long tiempoInterrupcion = 0;
volatile int      contadorSubida = 0;

// IRAM_ATTR: ISR en RAM interna — ejecución garantizada en ESP32
void IRAM_ATTR manejadorRising() {
  interrupcionOcurrida = true;
  tiempoInterrupcion   = millis();
  contadorSubida++;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println(F("============================================"));
  Serial.println(F("  PULSAR ESP32-C6 — Test RISING"));
  Serial.println(F("  Flanco de subida: LOW → HIGH"));
  Serial.println(F("  Con pull-up: ocurre al SOLTAR el botón"));
  Serial.println(F("============================================"));

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  // INPUT_PULLUP: reposo en HIGH, presionar = LOW, soltar = HIGH (RISING)
  pinMode(PIN_INT, INPUT_PULLUP);

  attachInterrupt(
    digitalPinToInterrupt(PIN_INT),
    manejadorRising,
    RISING
  );
}

void loop() {
  if (interrupcionOcurrida) {
    interrupcionOcurrida = false;

    // Parpadeo LED como confirmación visual
    digitalWrite(PIN_LED, HIGH);
    delay(100);
    digitalWrite(PIN_LED, LOW);

    Serial.print(F("[RISING ↑] Interrupción #"));
    Serial.print(contadorSubida);
    Serial.print(F("  |  pin=HIGH (1)"));
    Serial.print(F("  |  t="));
    Serial.print(tiempoInterrupcion);
    Serial.println(F(" ms"));
  }
}
