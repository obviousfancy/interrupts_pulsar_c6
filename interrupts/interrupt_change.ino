// PULSAR ESP32-C6 — Interrupciones RISING + FALLING en D7
// IRAM_ATTR: la ISR vive en RAM interna, ejecución garantizada

const int PIN_INT = D7;   // D7 del PULSAR
const int PIN_LED = 6;   // LED integrado (GPIO6 confirmado)

// volatile: le dice al compilador que esta variable
// puede cambiar en cualquier momento (desde la ISR)
// sin volatile el compilador la "optimiza" y loop()
// nunca ve el cambio real
volatile bool eventoNuevo   = false;
volatile int  estadoActual  = HIGH;
volatile unsigned long tEvento = 0;
volatile int  cntRising  = 0;
volatile int  cntFalling = 0;

// IRAM_ATTR: función en RAM interna, no en flash
void IRAM_ATTR manejadorISR() {
  estadoActual = digitalRead(PIN_INT);
  tEvento      = millis();
  eventoNuevo  = true;

  if (estadoActual == HIGH) cntRising++;
  else                      cntFalling++;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== Test interrupciones ESP32-C6 ===");
  Serial.println("Pin D7 con INPUT_PULLUP");
  Serial.println("FALLING = presionar boton");
  Serial.println("RISING  = soltar boton");
  Serial.println("====================================");

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  // INPUT_PULLUP: pin en HIGH en reposo
  // no necesitas resistencia externa
  pinMode(PIN_INT, INPUT_PULLUP);

  // CHANGE detecta ambos flancos en una sola ISR
  attachInterrupt(
    digitalPinToInterrupt(PIN_INT),
    manejadorISR,
    CHANGE
  );
}

void loop() {
  if (eventoNuevo) {
    eventoNuevo = false;

    // Parpadeo LED como confirmación visual
    digitalWrite(PIN_LED, HIGH);
    delay(50);
    digitalWrite(PIN_LED, LOW);

    // Imprimir qué flanco fue
    if (estadoActual == HIGH) {
      Serial.print("[RISING  ↑] subida  #");
      Serial.print(cntRising);
    } else {
      Serial.print("[FALLING ↓] bajada  #");
      Serial.print(cntFalling);
    }

    Serial.print("  |  pin=");
    Serial.print(estadoActual == HIGH ? "HIGH (1)" : "LOW  (0)");
    Serial.print("  |  t=");
    Serial.print(tEvento);
    Serial.println(" ms");
  }else{
     //Serial.println("Sin evento");
  }
}