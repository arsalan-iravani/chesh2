# CHESH2 — Realistic Code-Generated Digital Eye

This project implements a procedural, animated digital eye on ESP8266 + GC9A01 240x240 round TFT.

WARNING: You must configure the TFT_eSPI library before compiling. See "Library Configuration" below.

Features
- Procedural iris generated in code (no images)
- Dynamic pupil
- Smooth gaze animation and micro-saccades
- Blinking
- Expressions framework
- WiFi Access Point (ParaxEye / 12345678)
- Mobile web UI for real-time control

Hardware
- ESP8266 NodeMCU / ESP-12F
- GC9A01 240x240 round TFT

Wiring (use these pins exactly):
- SCLK -> D5 (GPIO14)
- MOSI -> D7 (GPIO13)
- MISO -> D6 (GPIO12)
- CS   -> D8 (GPIO15)
- DC   -> D3 (GPIO0)
- RST  -> D4 (GPIO2)

Library Configuration
- Install Arduino core for ESP8266
- Install TFT_eSPI library
- Open TFT_eSPI/User_Setup_Select.h or User_Setup.h in the library and create a project-specific User_Setup.h that matches the GC9A01 wiring above.

Example User_Setup additions (in the library's User_Setup.h):
- Define GC9A01 driver and map pins:
  #define GC9A01_DRIVER
  #define TFT_MISO 12
  #define TFT_MOSI 13
  #define TFT_SCLK 14
  #define TFT_CS   15
  #define TFT_DC   0
  #define TFT_RST  2

Notes on performance
- The code aims to be simple and clear. The procedural iris is sampled per-pixel which may be slow on the ESP8266 for very high animation rates.
- If needed, performance improvements: precompute iris texture, use SPI DMA/fast block transfers, reduce redraw area, optimize math with lookup tables.

Building
- Open chesh2.ino in Arduino IDE
- Configure board to NodeMCU 1.0 (ESP-12E Module) or equivalent
- Ensure TFT_eSPI is configured as above
- Upload

Web UI
- Connect to WiFi SSID: ParaxEye
- Password: 12345678
- Open http://192.168.4.1 in your phone

License
- MIT

