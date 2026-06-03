/*!
 * @file demo.ino
 *
 * Example sketch for the GDEY037T03 e-paper display driver.
 *
 * Demonstrates full refresh, fast refresh, and partial refresh using the
 * GDEY037T03 driver paired with the GDcanvas Adafruit GFX canvas.
 *
 * Hardware connections to e-paper
 * | Signal | ESP32-C5 pin|
 * |--------|-------------|
 * | SCK    | 6           |
 * | MISO   | –1 (unused) |
 * | MOSI   | 7           |
 * | CS     | 1           |
 * | BUSY   | 4           |
 * | RST    | 23          |
 * | DC     | 24          |
 *
 * Dependencies
 * - Adafruit GFX Library https://github.com/adafruit/Adafruit-GFX-Library
 *
 * GDEY037T03 driver was based on GooDisplay sample code found in https://www.good-display.com/companyfile/1117.html
 *
 * Written by Bruno Cerna (BrozuTechworks)
 * MIT License
 */

#include <GDEY037T03_AdafruitGFX.h>
#include "demoImg.h"

#define EPD_SCK   6
#define EPD_MOSI  7
#define EPD_CS    1
#define EPD_BUSY  4
#define EPD_RST   23
#define EPD_DC    24

GDEY037T03 epaper;  //< Global driver instance. Use driver methods to show images to screen.
GDcanvas canvas;    //< Off-screen canvas. Use Adafruit GFX calls to compose images here.

/// Complete display demonstration sequence.
void playDemo(){
  /// 1. Show a splash bitmap with a full (standard) refresh preceded by a white-clear screen.
  canvas.clearBuffer(EPD_WHITE);
  canvas.drawBitmap(0, 0, start, EPD_HEIGHT, EPD_WIDTH, EPD_BLACK, EPD_WHITE);
  epaper.screenWhite(0);
  delay(2000);
  epaper.screenShowPart(canvas.getBuffer());
  delay(4000);

  /// 2. Cycle through four rotation demo bitmaps using partial updates.
  canvas.setRotation(0);
  canvas.drawBitmap(0, 0, rot0, EPD_HEIGHT, EPD_WIDTH, 0x00, 0xFF);
  epaper.screenShow(canvas.getBuffer());
  delay(3000);
  canvas.setRotation(1);
  canvas.drawBitmap(0, 0, rot1,EPD_WIDTH, EPD_HEIGHT, 0x00, 0xFF);
  epaper.screenShowPart(canvas.getBuffer());
  delay(3000);
  canvas.setRotation(2);
  canvas.drawBitmap(0, 0, rot2, EPD_HEIGHT, EPD_WIDTH, 0x00, 0xFF);
  epaper.screenShowPart(canvas.getBuffer());
  delay(3000);
  canvas.setRotation(3);
  canvas.drawBitmap(0, 0, rot3, EPD_WIDTH, EPD_HEIGHT, 0x00, 0xFF);
  epaper.screenShowPart(canvas.getBuffer());
  delay(3000);

  /*! 3. Display a QR-code bitmap using the fast full-refresh.
   *  QR goes to image2LCD converter. Params to convert are:
   *  Scan Mode: Horizontal scan
   *  Output Grayscale: 2-color
   *  Scan Options：Scan from bottom to top, Color reversal
   */ 
  canvas.setRotation(1);
  canvas.drawBitmap(0, 0 , qr, EPD_WIDTH, EPD_HEIGHT, 0x00, 0xFF);
  epaper.screenShow(canvas.getBuffer(), 1);
  delay(10000);

  /// 4. Clear to white using the fast full-refresh.
  epaper.screenWhite(1);
  delay(2000);
}

void setup() {
  // epaper.begin(EPD_CS, EPD_BUSY, EPD_RST , EPD_DC)  //< For Arduino boards. NOT TESTED!!!!
  epaper.begin(EPD_SCK, -1, EPD_MOSI, EPD_CS, EPD_BUSY, EPD_RST, EPD_DC); //< Initialize the e-paper driver with custom pins
}

void loop() {
  playDemo();
  while(1);  // The program stops here   
}

