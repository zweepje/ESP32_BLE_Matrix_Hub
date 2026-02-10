# ESP32 Multi-Function Clock Project

This project is a configurable ESP32-based device that combines **three core functions** in a single firmware, selectable through configuration rather than separate builds.

The firmware is designed to run on multiple ESP32 variants and provides audio output, a display interface, and a web-based configuration system.

---

## Supported Hardware

The project supports the following ESP32 variants using separate build targets:

- **ESP32 (classic)**
- **ESP32-C3**
- **ESP32-S3**

All variants share the same functionality. Hardware-specific differences (memory, USB, PSRAM, etc.) are handled at build time.

---

## Core Functions

The device provides three main operating modes:

### 1. Info Mode
Displays informational data such as:
- Current time
- Network status
- Device status information

This mode is intended as a passive display and system overview.

---

### 2. Kitchen Timer (Kookwekker)
A countdown timer used as a kitchen timer:
- User-configurable duration
- Visual feedback on the display
- Audible notification when the timer expires

---

### 3. Alarm Clock
A classic alarm clock function:
- Configurable alarm time (hours and minutes)
- Optional daily repeat
- Audible alarm output
- Designed to operate autonomously after configuration

---

### WebInterface
- x.x.x.x/        - configuration for each matrix
- x.x.x.x/wekker  - Alarm settings
- x.x.x.x/scan-ble  - Scan BLE devices



## Configuration

- Device configuration is performed via a **web interface** hosted on the ESP32.
- Network credentials and application settings are stored persistently.
- No functionality differences exist between firmware builds; behavior is defined by configuration.

---

## Audio and Display

- Audio output is handled via **I2S** (e.g. MAX98357 amplifier).
- The display is driven via **I²C** (OLED display).
- Audio playback and display updates are coordinated to avoid blocking system operation.

---

## Build System

The project uses **PlatformIO** with multiple environments:

- `esp32`
- `esp32c3`
- `esp32s3`

Each environment targets a specific ESP32 variant while sharing the same source code.

---

## Design Philosophy

- One codebase, multiple hardware targets
- No feature-based Git branches
- Configuration over compilation
- Minimal external dependencies
- Predictable memory usage (explicit partition tables where required)

---

## Status

This project is under active development and focuses on stability, clarity, and long-term maintainability rather than rapid prototyping.

---

## License

(Add your license here)




















Natuurlijk! In de softwarewereld is Engels de standaard voor README-bestanden, zeker als je later integraties met Home Assistant wilt toevoegen of je code wilt delen op GitHub.

Hier is de vertaalde en licht verbeterde versie van je README.md. Ik heb de technische termen strak gehouden en de structuur nog iets professioneler gemaakt.

Markdown
# ⏰ PixelWekker (ESP32-S3)

An intelligent ESP32-S3 based alarm clock featuring a responsive web interface for alarm management and optimized memory handling.

## 🌟 Features

- **Web Interface:** Easily set alarms for tomorrow (one-time), weekdays, and weekends directly from your browser.
- **Optimized Storage:** Uses the `Preferences` library to persist settings in the ESP32 NVS (Non-Volatile Storage).
- **RAM Caching:** High-speed alarm checks in the `loop()` function using cached values to prevent unnecessary Flash wear.
- **Responsive Design:** Dark-mode optimized web UI, built for both desktop and mobile devices.
- **BLE Ready:** Framework in place for presence detection via Bluetooth Low Energy (NimBLE).

## 🛠 Hardware Specifications

- **Microcontroller:** ESP32-S3
- **Display:** [e.g., WS2812B LED Matrix or Strip]
- **Pin Mapping:**
    - `LED Data Pin:` [e.g., GPIO 18]
    - `Battery Sense (Optional):` [e.g., GPIO 4]

## 💻 Software & Libraries

Built with **PlatformIO**, utilizing the following libraries:

- [NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino) - High-performance Bluetooth scanning.
- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) - Asynchronous web server for the UI.
- [Preferences](https://github.com/espressif/arduino-esp32/blob/master/libraries/Preferences/src/Preferences.h) - Secure NVS data persistence.

## 📂 Project Structure

- `src/main.cpp`: Core logic, alarm scheduling, and BLE scanner coordination.
- `src/web/Webserver.cpp`: Web routes and dynamic HTML template processing.
- `include/`: Header files for global configurations and pin definitions.

## 🚀 Getting Started

1.  **Clone the Project:** Open the folder in CLion with the PlatformIO plugin installed.
2.  **Library Installation:** PlatformIO will automatically fetch dependencies based on `platformio.ini`.
3.  **Flash the ESP32:** Connect your ESP32-S3 and trigger the 'Upload' task.
4.  **Setup:**
    - Connect to the clock's WiFi network.
    - Navigate to `http://[device-ip]` in your browser.
    - Set your alarm times and hit 'Save'.

## 📅 Roadmap

- [x] Webserver with dynamic HTML placeholders.
- [x] RAM-cache system with `alarmChanged` flags.
- [ ] BLE Chunksize & delay optimization.
- [ ] Home Assistant integration (via JSON API or MQTT).
- [ ] Real-time NTP time synchronization.

## 📄 License

This project is for personal use. Feel free to modify and adapt for your own smart home setup!


## 🙏 Credits
This project started with iPixel-ESP32 from **[ToBiDi0410](...)** which was a
reverse engineering of the protocol by **[lucagoc](https://github.com/lucagoc)** via his **[iPixel-CLI](https://github.com/lucagoc/iPixel-CLI)**.


## 🤖 AI Disclaimer
Some descriptions and code-snippets in this repository are generated by artificial intelligence.




**ESP32-iPixel** is a project to control **iPixel color matrices** using an ESP32 microcontroller.  
It exposes the device’s functionality via a **REST API** served by a built-in web server.

---

> [!CAUTION]  
> This docuementation is outdated as a rewrite of the webserver is going on!  
> Please use this work-tree for testing: https://github.com/ToBiDi0410/iPixel-ESP32/tree/0602925e64fb0f41a694e3bdc56e55c945941ec6

# ⚠️⚠️⚠️ UNSTABLE
This is currently only work-in-progress with the only supported device beeing the ESP32-S3 (SuperMicro).  
Support for further ESP32 boards is planned.  

---

## 📡 Endpoints
All endpoints are **device-specific**, identified by `:mac` (the device's MAC address).  
You have to determine this BLE-address on your own (e.g. using Android Apps).  

> ⚠️ When an endpoint is called for the first time, the ESP32 connects to the device.  
> This may temporarily return **HTTP 408 (Request Timeout)**.

| Endpoint                              | Description                    | Parameters                                                                                                                        |
| ------------------------------------- | ------------------------------ | --------------------------------------------------------------------------------------------------------------------------------- |
| `/device/:mac/setTime`                | Set device time                | `hour=0-23&minute=0-59&second=0-59`                                                                                               |
| `/device/:mac/setFunMode`             | Enable/disable fun mode        | `funMode=true/false`                                                                                                              |
| `/device/:mac/setOrientation`         | Set display orientation        | `orientation=0-2`                                                                                                                 |
| `/device/:mac/clear`                  | Clear the display              | —                                                                                                                                 |
| `/device/:mac/setBrightness`          | Adjust brightness              | `brightness=0-100`                                                                                                                |
| `/device/:mac/setSpeed`               | Adjust animation speed         | `speed=0-100`                                                                                                                     |
| `/device/:mac/ledOff`                 | Turn LEDs off                  | —                                                                                                                                 |
| `/device/:mac/ledOn`                  | Turn LEDs on                   | —                                                                                                                                 |
| `/device/:mac/deleteScreen`           | Delete a screen                | `screen=0-10`                                                                                                                     |
| `/device/:mac/setPixel`               | Set individual pixel color     | `x=0-255&y=0-255&r=0-255&g=0-255&b=0-255`                                                                                         |
| `/device/:mac/setClockMode`           | Configure clock mode           | `style=1-8&dayOfWeek=1-7&year=0-99&month=1-12&day=1-31`                                                                           |
| `/device/:mac/setRhythmLevelMode`     | Set rhythm-level visualization | `style=0-4&l0=0-15&...&l14=0-15`                                                                                                  |
| `/device/:mac/setRhythmAnimationMode` | Set rhythm animation           | `style=0-1&frame=0-7`                                                                                                             |
| `/device/:mac/sendText`               | Send text                      | `text=Hello&animation=0-7&save_slot=1-10&speed=0-100&colorR=0-255&colorG=0-255&colorB=0-255&rainbow_mode=0-9&matrix_height=0-255` |
| `/device/:mac/sendPNG`                | Send PNG                       | `hex=HEX-STRING` (checkout 'File to Hex' on Google to get the hex string)                                                         |
| `/device/:mac/sendGIF`                | Send GIF                       | `hex=HEX-STRING` (checkout 'File to Hex' on Google to get the hex string)                                                         |
| `/device/:mac/sendArbitrary`          | Send rainbow test framebuffer  |                                                                                                                                   |
---

## 🖼️ GFX
**ESP32-iPixel** features a very early stage GFX stack that allows rendering element-based views on the device by sending the structure via JSON.  
Take a look at the example in [GFX.md](GFX.md)


## ⚙️ Installation
1. Install **[PlatformIO](https://platformio.org/)**.
2. Build and upload the firmware to the ESP32 using PlatformIO.
3. Open a **Chromium-based browser** and visit [https://install.wled.me/](https://install.wled.me/).
4. Click **Install** and select your device.
5. Connect to WiFi:
   * Click **Connect to WiFi** and wait for the scan to finish.
   * Select your WiFi network and enter credentials.
   * Wait for the connection to establish.
6. Click **Visit Device** to get the IP address.

---
## 📝 Todo
* Support `sendText` for other matrices than 96x16
* Device pairings (save devices instead of always using the mac)
* Password or key-based protection
* Connection improvements (faster connect? better disconnect handling?)
* Clock Mode via NTP (set Display as clock)
* Web Flasher Support
---

## 🙏 Credits
Reverse engineering of the protocol by **[lucagoc](https://github.com/lucagoc)** via his **[iPixel-CLI](https://github.com/lucagoc/iPixel-CLI)**.

## 🤖 AI Disclaimer
Some descriptions and code-snippets in this repository are generated by artificial intelligence.  