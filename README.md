# GDEY037T03-AdafruitGFX
This library provides a GDEY037T03 e-paper display driver integrated with the Adafruit GFX graphics library, offering:
- Full refresh
- Fast refresh
- Partial refresh
- Sleep/power control
- A canvas class for drawing with the standard Adafruit GFX API before pushing the buffer to the display
## About GooDisplay GDEY037T03
3.7" black-and-white e-paper display.
- 240x416 resolution.
- 4-wire SPI interface
## How to convert images to LCD bitmap array
Go to https://www.e-paper-display.com/Image2LCD.html open your image, modify parameters and click save.

Mandatory parameters
- Scan Mode: Horizontal scan
- Output Grayscale: 2-color
- Scan Options：Scan from bottom to top, Color reversal
## Installation
Download the library as a .zip file. On arduino IDE go Sketch → Include Library → Add .ZIP Library...
## License
MIT License.
