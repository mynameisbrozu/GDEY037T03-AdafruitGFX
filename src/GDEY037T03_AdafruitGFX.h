/*!
 * @file GDEY037T03_AdafruitGFX.h
 *
 * This library provides a GDEY037T03 e-paper display driver
 * integrated with the Adafruit GFX graphics library. It gives full
 * display control (full refresh, fast refresh, partial refresh, sleep)
 * as well as a canvas class that lets you draw using the standard
 * Adafruit GFX API before pushing the buffer to the display. 
 *
 * This library depends on:
 * - Adafruit GFX Library https://github.com/adafruit/Adafruit-GFX-Library
 *
 * GDEY037T03 driver was based on GooDisplay sample code found in https://www.good-display.com/companyfile/1117.html
 *
 * Written by Bruno Cerna (BrozuTechworks)
 * MIT License
 */
#ifndef _GDEY037T03_AdafruitGFX_H
#define _GDEY037T03_AdafruitGFX_H

#include <Adafruit_GFX.h>
#include <Arduino.h>
#include <SPI.h>

/** @brief Display black color */
#define EPD_BLACK   0x00

/** @brief Display white color */
#define EPD_WHITE   0xFF

/** @brief Display width in pixels (physical horizontal resolution). */
#define EPD_WIDTH   240 

/** @brief Display height in pixels (physical vertical resolution). */
#define EPD_HEIGHT  416

/** @brief Total byte size of the display framebuffer (WIDTH * HEIGHT / 8). */
#define EPD_ARRAY  (EPD_WIDTH*EPD_HEIGHT)/8  

/// IO settings
#define isEPD_BUSY  digitalRead(_busyPin)
#define EPD_RST_0   digitalWrite(_resPin,LOW)
#define EPD_RST_1   digitalWrite(_resPin,HIGH)
#define EPD_DC_0    digitalWrite(_dcPin,LOW)
#define EPD_DC_1    digitalWrite(_dcPin,HIGH)
#define EPD_CS_0    digitalWrite(_csPin,LOW)
#define EPD_CS_1    digitalWrite(_csPin,HIGH)

/*!
 * @brief Low-level driver class for the GDEY037T03 e-paper display.
 *
 * Handles SPI communication, initialization sequences, full/fast/partial
 * refresh modes, power management, and deep-sleep entry.
 */
class GDEY037T03 {
  public:
    /*!
     * @brief  Construct a GDEY037T03 driver instance.
     * @param  spiClock  SPI bus frequency in Hz. Defaults to 10 MHz.
     */
    GDEY037T03(uint32_t spiClock = 10000000);
    /*!
     * @brief  Initialize the driver and underlying SPI bus (standard pinout).
     */
    void begin(uint8_t cs = 255, uint8_t busy = 255, uint8_t res = 255, uint8_t dc = 255);
    #if defined(ESP32) || defined(ESP8266) || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_STM32)
    /*!
     * @brief  Initialize the driver with explicit SPI pin mapping (compatible boards only).
     *
     * Use this overload when the SPI bus pins differ.
     */
    void begin(int8_t sck, int8_t miso, int8_t mosi, uint8_t cs = 255, uint8_t busy = 255, uint8_t res = 255, uint8_t dc = 255);
    #endif
    /*!
     * @brief  Perform a full-screen refresh with new image data.
     *
     * Uploads the provided bitmap to the display and triggers a full refresh
     * cycle. Puts the panel into deep-sleep when done.
     *
     * @param  data  Pointer to the framebuffer to display. Must be
     *               EPD_ARRAY bytes long, 1 bit per pixel (w/b).
     * @param  fast  Pass true to use the 1.5s refresh. Defaults to false (2.0s refresh).
     */
    void screenShow(const unsigned char * data, bool fast = 0);
    /*!
     * @brief  Fill the entire display with white. Uses full-screen refresh. 
     * @param  fast  Pass true to use the 1.5s refresh. Defaults to false (2.0s refresh).
     */
    void screenWhite(bool fast = 0);
    /*!
     * @brief  Fill the entire display with black. Uses full-screen refresh.
     * @param  fast  Pass true to use the 1.5s refresh. Defaults to false (2.0s refresh).
     */
    void screenBlack(bool fast = 0);
    /**
     * @brief  Perform a partial-update refresh with new image data.
     *
     * Uses the partial-refresh which eliminates flicker and is faster
     * than a full refresh. Creates screen ghosting.

     * @warning After 5 consecutive partial-updates, do a full-screen refresh to
     *          clear screen ghosting.
     * @warning dpSleep parameter can only be set to false if doing consecutives
     *          partial-updates. On the last partial-update set again to true, otherwise
     *          screen lifespan will be reduced.
     *
     * @param  data     Pointer to the new framebuffer (EPD_ARRAY bytes).
     * @param  dpSleep  Pass true (default) to enter deep-sleep after the
     *                  update. Pass false to keep the panel powered, e.g.
     *                  when chaining multiple partial updates.
     */
    void screenShowPart(const unsigned char * data, bool dpSleep = 1); 

  private:
    uint8_t _csPin;
    uint8_t _busyPin;
    uint8_t _resPin;
    uint8_t _dcPin;
    uint32_t _spiClock;
    uint8_t _spiMode;
    uint8_t _bitOrder;
    unsigned char oldData[12480];
    void writeData(unsigned char data);
    void writeCmd(unsigned char command);
    void lcdCheckStatus();
    void init();   
    void initFast();    
    void initPart();
    void deepSleep();
    void powerOff();
    void epdUpdate();  
};

/*!
 * @brief  Off-screen canvas for the GDEY037T03 backed by Adafruit GFX.
 *
 * Allocates a 1-bit-per-pixel framebuffer matching the display dimensions
 * and inherits the full Adafruit GFX drawing API (lines, circles, text,
 * bitmaps, etc.). After drawing, pass getBuffer() to the
 * GDEY037T03 driver to push the canvas to the physical display.
 */
class GDcanvas : public Adafruit_GFX {
  public:
    GDcanvas();
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void clearBuffer(uint8_t color = 0xFF);
    /**********************************************************************/
    /*!
      @brief    Get a pointer to the internal buffer memory
      @returns  A pointer to the allocated buffer
    */
    /**********************************************************************/
    uint8_t* getBuffer() { return _buffer; }

  private:
    uint8_t _buffer[EPD_ARRAY];
};



#endif // GDEY037T03_AdafruitGFX_H