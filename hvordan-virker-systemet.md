# Hvordan Trafiklyskontrol Systemet Virker

## Oversigt

Dette system styrer et trafiksignal med tre LED'er (grøn, gul, rød) via en webserver på en ESP32. Når det grønne lys er tændt, vises en gående stregmand-animation på en OLED-skærm.

## Hardware Komponenter

- **ESP32**: Microcontroller med WiFi
- **3 LED'er**: Tilsluttet GPIO pins 33 (grøn), 25 (gul), 26 (rød)
- **OLED Display**: 128x32 pixels, tilsluttet via I2C (SDA: pin 21, SCL: pin 22)

## Software Arkitektur

### Biblioteker

```cpp
#include <WiFi.h>           // WiFi forbindelse
#include <WebServer.h>      // HTTP server
#include <Wire.h>           // I2C kommunikation
#include <Adafruit_GFX.h>   // Grafik funktioner
#include <Adafruit_SSD1306.h> // OLED driver
```

### Hovedkomponenter

#### 1. Webserver API

Systemet eksponerer et REST API endpoint:

```
GET http://ESP32_IP/led?led={led}&state={state}
```

**Parametre:**

- `led`: "green", "yellow", eller "red"
- `state`: "on" eller "off"

**Eksempel:**

```
http://192.168.5.5/led?led=green&state=on
```

#### 2. LED Kontrol

Når en HTTP request modtages:

1. Systemet parser URL parametrene
2. Tænder/slukker den specificerede LED
3. Hvis grønt lys tændes, starter animationen
4. Hvis grønt lys slukkes, nulstilles animationen
5. Sender JSON respons tilbage

#### 3. OLED Animation

**Stregmand Design:**

- Hoved: Cirkel ved position (x, 10)
- Krop: Lodret linje fra y=13 til y=22
- Arme: Diagonale linjer fra kroppen
- Ben: To animerede linjer der skifter position

**Animation:**

- Frame 0: Ben i neutral position
- Frame 1: Ben i gang-position
- Skifter mellem frames hvert 100ms
- Flytter 2 pixels til højre per frame
- Nulstiller position når personen når skærmens kant

#### 4. Program Flow

**Setup (køres én gang):**

1. Start seriel kommunikation (115200 baud)
2. Initialiser I2C på pins 21 og 22
3. Konfigurer LED pins som outputs
4. Initialiser OLED display
5. Forbind til WiFi netværk
6. Start webserver på port 80

**Loop (køres konstant):**

1. Håndter indkommende HTTP requests
2. Ryd display buffer
3. Hvis grønt lys er tændt:
   - Tegn stregmand på nuværende position
   - Flyt position 2 pixels til højre
   - Skift animations-frame
   - Nulstil position ved skærmens kant
4. Opdater display
5. Vent 100ms

## Integration med React App

React app'en sender HTTP requests til ESP32:

```javascript
const controlLED = async (led, state) => {
  await fetch(`http://${ESP32_IP}/led?led=${led}&state=${state}`);
};
```

**Trafiklyskontrol sekvens:**

1. Grøn (10 sek) → Person går
2. Gul (2 sek) → Person stopper
3. Rød (10 sek) → Person venter
4. Rød + Gul (2 sek) → Forbereder sig
5. Tilbage til grøn

## Tekniske Detaljer

### WiFi Forbindelse

- **SSID**: MediaCollege
- **Password**: vms12345
- **IP**: Tildeles dynamisk via DHCP (vises i Serial Monitor)

### I2C Konfiguration

- **SDA**: GPIO 21
- **SCL**: GPIO 22
- **Display Adresse**: 0x3C

### Timing

- **Animation hastighed**: 100ms per frame
- **Person hastighed**: 2 pixels per frame
- **Animation smoothness**: 2-frame leg animation

## Troubleshooting

**OLED virker ikke:**

- Tjek I2C forbindelser (SDA: 21, SCL: 22)
- Verificer I2C adresse er 0x3C

**LED'er tænder ikke:**

- Verificer GPIO pin numre (33, 25, 26)
- Tjek at LED'er er korrekt forbundet

**Kan ikke forbinde til WiFi:**

- Tjek SSID og password
- Se Serial Monitor for IP adresse

**Animation stopper:**

- Grønt lys skal være tændt
- Tjek `isGreen` variabel status

