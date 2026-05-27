# PULSAR ESP32-C6 — Interrupciones externas

Ejemplos de interrupciones externas para la tarjeta **UNIT Electronics PULSAR ESP32-C6**
usando Arduino IDE. Incluye flanco de subida, bajada y detección de ambos flancos,
con notas específicas del hardware de esta tarjeta.

---

## Requisitos

| Elemento | Valor |
|---|---|
| Tarjeta | UNIT Electronics PULSAR ESP32-C6 |
| IDE | Arduino IDE 2.x |
| Core ESP32 | Espressif ESP32 Arduino Core 3.0.0 o superior |
| Board definition | PULSAR ESP32-C6 |

### Configuración en Arduino IDE

```
Board              → PULSAR ESP32-C6
USB CDC On Boot    → Enabled
JTAG Adapter       → Integrated USB JTAG
Upload Speed       → 460800
CPU Frequency      → 160MHz (WiFi)
Flash Size         → 4MB (32Mb)
```

---

## Notas importantes del hardware

### LED integrado
El LED integrado del PULSAR ESP32-C6 **no está en el pin 13** como en un Arduino Nano estándar.
Usar `LED_BUILTIN` o directamente `GPIO 6`:

```cpp
// Correcto
pinMode(LED_BUILTIN, OUTPUT);  // resuelve a GPIO 6
pinMode(6, OUTPUT);             // equivalente

// Incorrecto en este board — causa crash
pinMode(13, OUTPUT);
```

### Por qué IRAM_ATTR es obligatorio

El ESP32-C6 ejecuta el código normalmente desde la flash externa.
Durante una interrupción, si la flash está ocupada, la ISR puede no ejecutarse.
`IRAM_ATTR` coloca la función en la RAM interna del chip, garantizando
ejecución inmediata sin importar el estado de la flash:

```cpp
// Siempre así en ESP32
void IRAM_ATTR miISR() {
  // código de la interrupción
}

// Nunca así — puede fallar silenciosamente
void miISR() {
  // código de la interrupción
}
```

### Por qué volatile en las variables de la ISR

El compilador optimiza variables que "nunca cambian" desde el punto de vista
del `loop()`. Con `volatile` le indicas que esa variable puede cambiar en
cualquier momento desde la ISR:

```cpp
volatile bool eventoNuevo = false;  // correcto
bool eventoNuevo = false;           // loop() puede nunca ver el cambio
```

---

## Pines recomendados para interrupciones

| Pin | GPIO | Estado | Notas |
|---|---|---|---|
| D4 | GPIO4 | Libre | Recomendado |
| D5 | GPIO5 | Libre | Recomendado |
| D6 | GPIO6 | LED builtin | Recomendado |
| D7 | GPIO7 | Libre | Recomendado — validado |
| A0 | GPIO0 | Libre | ADC + GPIO |
| A1 | GPIO1 | Libre | ADC + GPIO |
| A2 | GPIO2 | Libre | ADC + GPIO |
| A3 | GPIO3 | Libre | ADC + GPIO |
| D8 | GPIO8 | Condicional | Libre solo si JTAG Adapter = Disabled |
| D9 | GPIO9 | Condicional | Botón BOOT — usar con cuidado |
| D10–D12 | GPIO10–12 | Condicional | Libre solo si microSD inactiva |
| D2 | GPIO2 | Reservado | NeoPixel WS2812B integrado |
| D3 | GPIO3 | Reservado | Botón BOOT — puede afectar arranque |
| D13 | GPIO13 | Reservado | Bus SDIO interno — causa crash si se usa como salida |

---

## Conexión física para los ejemplos

Estos ejemplos usan un botón pulsador con el pull-up interno del ESP32-C6.
No se necesita resistencia externa:

```
PULSAR D7  ────────────  Terminal A del botón
PULSAR GND ────────────  Terminal B del botón
```

Con `INPUT_PULLUP`:
- Reposo → HIGH (3.3V)
- Presionado → LOW (0V)
- FALLING = presionar
- RISING = soltar

Para sensores externos que manejan su propio voltaje usar `INPUT` en lugar de `INPUT_PULLUP`.

---

## Ejemplos incluidos

### 1. Flanco de subida — `interrupt_rising.ino`

Detecta cuando el pin pasa de LOW a HIGH.
Con pull-up interno ocurre al **soltar** el botón.

### 2. Flanco de bajada — `interrupt_falling.ino`

Detecta cuando el pin pasa de HIGH a LOW.
Con pull-up interno ocurre al **presionar** el botón.

### 3. Ambos flancos — `interrupt_change.ino`

Detecta subida y bajada en la misma ISR usando `CHANGE`.
Incluye anti-rebote (debounce) por software de 50ms.
Cada presionar+soltar genera dos eventos diferenciados en Serial Monitor.

---

## Salida esperada en Serial Monitor (115200 baud)

```
=== Test interrupciones ESP32-C6 ===
Pin D7 con INPUT_PULLUP
FALLING = presionar boton
RISING  = soltar boton
====================================
[FALLING ↓] bajada  #1  |  pin=LOW  (0)  |  t=2405 ms
[RISING  ↑] subida  #1  |  pin=HIGH (1)  |  t=2957 ms
[FALLING ↓] bajada  #2  |  pin=LOW  (0)  |  t=3893 ms
[RISING  ↑] subida  #2  |  pin=HIGH (1)  |  t=4398 ms
```

---

## Solución de problemas al subir el sketch

El ESP32-C6 usa USB-JTAG integrado (`VID 303a:1001`) en lugar de chip USB-Serial externo.
Si el upload falla con "Write timeout":

1. Verifica `JTAG Adapter → Integrated USB JTAG` en Arduino IDE
2. Si persiste, entra manualmente en modo bootloader:
   - Desconecta el USB
   - Mantén presionado el botón **BOOT**
   - Conecta el USB
   - Suelta **BOOT**
   - Sube el sketch desde Arduino IDE

En Linux, si el puerto `/dev/ttyACM0` no tiene permisos:

```bash
sudo usermod -aG dialout $USER
```

Crear regla udev permanente para el dispositivo (`303a:1001`):

```bash
sudo nano /etc/udev/rules.d/99-espressif.rules
```

```
SUBSYSTEM=="usb", ATTRS{idVendor}=="303a", ATTRS{idProduct}=="1001", MODE="0666", GROUP="dialout"
SUBSYSTEM=="tty", ATTRS{idVendor}=="303a", ATTRS{idProduct}=="1001", MODE="0666", GROUP="dialout"
```

```bash
sudo udevadm control --reload-rules && sudo udevadm trigger
```

---

## Licencia

MIT License — libre para uso, modificación y distribución.
