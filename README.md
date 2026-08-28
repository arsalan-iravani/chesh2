# RealisticEye — Procedural Digital Eye for ESP8266 + GC9A01

This project renders a realistic, procedural digital eye on an ESP8266 with a GC9A01 240×240 round TFT and provides a mobile web control UI served from the ESP8266 SoftAP.

Key features
- Procedural iris with multi-scale fibers, rings, crypt-like details, and configurable color.
- Iris cached in RAM as RGB565 to allow complex detail without per-frame recomputation.
- Pupil shading, layered highlights, realistic sclera with subtle vessels.
- Parametric eyelids and smooth blink animation.
- Gaze control with easing and micro-saccades.
- Lightweight tears for crying expression.
- WiFi Access Point: SSID `RealisticEye`, password `12345678`, IP `192.168.4.1`.
- Web UI to set color, gaze, expression, pupil, and toggles.

Required libraries
- Arduino ESP8266 core
- TFT_eSPI (Bodmer)
- ESP8266WiFi (comes with core)
- ESP8266WebServer (comes with core)

Wiring
- SCLK -> D5 (GPIO14)
- MOSI -> D7 (GPIO13)
- MISO -> D6 (GPIO12)
- CS   -> D8 (GPIO15)
- DC   -> D3 (GPIO0)
- RST  -> D4 (GPIO2)
- VCC  -> 3.3V
- GND  -> GND
- BL   -> 3.3V (or tied to 3.3V via resistor if needed)

TFT_eSPI configuration
- Edit TFT_eSPI/User_Setup.h as described in the repository file 'TFT_User_Setup_INSTRUCTIONS.txt'.
