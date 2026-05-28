// ============================================================
//  PULSAR ESP32-C6 — Interrupción por flanco de BAJADA (FALLING)
//  Conexión: botón entre D7 y GND
//  Con INPUT_PULLUP: FALLING ocurre al PRESIONAR el botón
// ============================================================

const int PIN_INT = 7;    // D7 del PULSAR
const int PIN_LED = 6;    // LED integrado GPIO6

volatile bool     interrupcionOcurrida = false;
volatile unsigned long tiempoInterrupcion = 0;
volatile int      contadorBajada = 0;

// IRAM_ATTR: ISR en RAM interna — ejecución garantizada en ESP32
void IRAM_ATTR manejadorFalling() {
  interrupcionOcurrida = true;
  tiempoInterrupcion   = millis();
  contadorBajada++;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println(F("============================================"));
  Serial.println(F("  PULSAR ESP32-C6 — Test FALLING"));
  Serial.println(F("  Flanco de bajada: HIGH → LOW"));
  Serial.println(F("  Con pull-up: ocurre al PRESIONAR el botón"));
  Serial.println(F("============================================"));

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  // INPUT_PULLUP: reposo en HIGH, presionar = LOW (FALLING)
  pinMode(PIN_INT, INPUT_PULLUP);

  attachInterrupt(
    digitalPinToInterrupt(PIN_INT),
    manejadorFalling,
    FALLING
  );
}

void loop() {
  if (interrupcionOcurrida) {
    interrupcionOcurrida = false;

    // Parpadeo LED como confirmación visual
    digitalWrite(PIN_LED, HIGH);
    delay(100);
    digitalWrite(PIN_LED, LOW);

    Serial.print(F("[FALLING ↓] Interrupción #"));
    Serial.print(contadorBajada);
    Serial.print(F("  |  pin=LOW  (0)"));
    Serial.print(F("  |  t="));
    Serial.print(tiempoInterrupcion);
    Serial.println(F(" ms"));
  }
}
