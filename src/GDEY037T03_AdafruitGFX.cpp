/*!
 * @file GDEY037T03_AdafruitGFX.cpp
 *
 * Implementation of the GDEY037T03 e-paper driver and GDcanvas.
 *
 * See GDEY037T03_AdafruitGFX.h for full class documentation.
 *
 * GDEY037T03 driver was based on GooDisplay sample code found in https://www.good-display.com/companyfile/1117.html
 *
 * Written by Bruno Cerna (BrozuTechworks)
 * MIT License
 */
#include "GDEY037T03_AdafruitGFX.h"

/// GDEY037T03 Driver methods
GDEY037T03::GDEY037T03(uint32_t spiClock)
  : _spiClock(spiClock),
    _spiMode(SPI_MODE0),
    _bitOrder(MSBFIRST)
{}

void GDEY037T03::begin(uint8_t cs, uint8_t busy, uint8_t res, uint8_t dc){
  _csPin = cs;
  _busyPin = busy;
  _resPin = res;
  _dcPin = dc;
  pinMode(_busyPin, INPUT); ///< BUSY
  pinMode(_resPin, OUTPUT); ///< RES 
  pinMode(_dcPin, OUTPUT);  ///< DC   
  pinMode(_csPin, OUTPUT);  ///< CS
  digitalWrite(_csPin, HIGH); 

  SPI.begin();
}

#if defined(ESP32) || defined(ESP8266) || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_STM32)
void GDEY037T03::begin(int8_t sck, int8_t miso, int8_t mosi, uint8_t cs, uint8_t busy, uint8_t res, uint8_t dc){
  _csPin = cs;
  _busyPin = busy;
  _resPin = res;
  _dcPin = dc;
  pinMode(_busyPin, INPUT); ///< BUSY
  pinMode(_resPin, OUTPUT); ///< RES 
  pinMode(_dcPin, OUTPUT);  ///< DC   
  pinMode(_csPin, OUTPUT);  ///< CS
  digitalWrite(_csPin, HIGH); 

  SPI.begin(sck, miso, mosi, -1);
}
#endif

void GDEY037T03::writeCmd(unsigned char command){
  SPI.beginTransaction(SPISettings(_spiClock, _bitOrder, _spiMode));
  EPD_CS_0;
  EPD_DC_0;  ///< D/C#   0:command  1:data  
  SPI.transfer(command);
  EPD_CS_1;
  SPI.endTransaction();
}

void GDEY037T03::writeData(unsigned char data){
  SPI.beginTransaction(SPISettings(_spiClock, _bitOrder, _spiMode));
  EPD_CS_0;
  EPD_DC_1;  ///< D/C#   0:command  1:data
  SPI.transfer(data);
  EPD_CS_1;
  SPI.endTransaction();
}

void GDEY037T03::lcdCheckStatus(){
  while(!isEPD_BUSY); ///< 0:BUSY, 1:FREE
}

void GDEY037T03::init(){
  EPD_RST_0;    ///< Module reset
  delay(10);///< At least 10ms delay 
  EPD_RST_1;
  delay(10);///< At least 10ms delay 
  
  writeCmd(0x04);  ///< Power on
  lcdCheckStatus();        ///< waiting for the electronic paper IC to release the idle signal

  writeCmd(0X50);  ///< VCOM AND DATA INTERVAL SETTING     
  writeData(0x97); ///< WBmode:VBDF 17|D7 VBDW 97 VBDB 57    WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7
}

void GDEY037T03::initFast(){
  EPD_RST_0;    ///< Module reset
  delay(10);///< At least 10ms delay 
  EPD_RST_1;
  delay(10);///< At least 10ms delay 
  
  writeCmd(0x04);  ///< Power on
  lcdCheckStatus();        ///< waiting for the electronic paper IC to release the idle signal

  writeCmd(0xE0);
  writeData(0x02); 

  writeCmd(0xE5);
  writeData(0x5F);  ///< 0x5F--1.5s
}

void GDEY037T03::initPart(){
  EPD_RST_0;    ///< Module reset
  delay(10);///< At least 10ms delay 
  EPD_RST_1;
  delay(10);///< At least 10ms delay 
    
  writeCmd(0x04);  ///< Power on
  lcdCheckStatus();        ///< waiting for the electronic paper IC to release the idle signal

  writeCmd(0xE0);
  writeData(0x02); 

  writeCmd(0xE5);
  writeData(0x6E);
    
  writeCmd(0x50); 
  writeData(0xD7);
}

void GDEY037T03::deepSleep(){
  writeCmd(0X02);   //< power off
  lcdCheckStatus();          //< waiting for the electronic paper IC to release the idle signal
  writeCmd(0X07);   //< deep sleep
  writeData(0xA5); 
}

void GDEY037T03::powerOff(){
  writeCmd(0x02); //< POWER ON
  lcdCheckStatus();
}

void GDEY037T03::epdUpdate(){
  ///update
  writeCmd(0x12);   //< DISPLAY update   
  delay(1);              //< !!!The delay here is necessary, 200uS at least!!!     
  lcdCheckStatus();          //< waiting for the electronic paper IC to release the idle signal
}

void GDEY037T03::screenShow(const unsigned char * data, bool fast){
  if(fast) initFast(); else init();
  
  unsigned int i;
  /// Write Data
  writeCmd(0x10);        //< Transfer old data
  for(i=0;i<EPD_ARRAY;i++)    
  { 
    writeData(oldData[i]);  //< Transfer the actual displayed data
  } 
  writeCmd(0x13);        //< Transfer new data
  for(i=0;i<EPD_ARRAY;i++)       
  {
    writeData(data[i]);  //< Transfer the actual displayed data
    oldData[i]=data[i]; 
  }   
  epdUpdate();    
  powerOff();

  deepSleep();
}

void GDEY037T03::screenWhite(bool fast){
  if(fast) initFast(); else init();

  unsigned int i;
  /// Write Data
  writeCmd(0x10);      
  for(i=0;i<EPD_ARRAY;i++)       
  {
    writeData(oldData[i]);  
  }
  writeCmd(0x13);      
  for(i=0;i<EPD_ARRAY;i++)       
  {
    writeData(0xff);  
    oldData[i]=0xff; 
  }
  epdUpdate();    
  powerOff();

  deepSleep();
}

void GDEY037T03::screenBlack(bool fast){
  if(fast) initFast(); else init();

  unsigned int i;
  /// Write Data
  writeCmd(0x10);      
  for(i=0;i<EPD_ARRAY;i++)       
  {
    writeData(oldData[i]);  
  }
  writeCmd(0x13);      
  for(i=0;i<EPD_ARRAY;i++)       
  {
    writeData(0x00);  
    oldData[i]=0x00; 
  }
  epdUpdate();    
  powerOff();

  deepSleep();
}

void GDEY037T03::screenShowPart(const unsigned char * data, bool dpSleep){
  unsigned int i;
  initPart();
  /// Write Data
  writeCmd(0x10);        //< Transfer old data
  for(i=0;i<EPD_ARRAY;i++)    
  { 
    writeData(oldData[i]);  //< Transfer the actual displayed data
  } 
  writeCmd(0x13);        //< Transfer new data
  for(i=0;i<EPD_ARRAY;i++)       
  {
    writeData(data[i]);  //< Transfer the actual displayed data
    oldData[i]=data[i]; 
  }  
    
  epdUpdate();    
  powerOff();

  if(dpSleep) deepSleep();
}

/// Integration canvas with adafruit GFX methods

GDcanvas::GDcanvas() : Adafruit_GFX(EPD_HEIGHT, EPD_WIDTH) {
  memset(_buffer, 0xFF, sizeof(_buffer));
}

void GDcanvas::drawPixel(int16_t x, int16_t y, uint16_t color) {
  int16_t t;
  switch (rotation) {
    case 1:
      t = x;
      x = WIDTH - 1 - y;
      y = t;
      break;
    case 2:
      x = WIDTH  - 1 - x;
      y = HEIGHT - 1 - y;
      break;
    case 3:
      t = x;
      x = y;
      y = HEIGHT - 1 - t;
      break;
  }
  if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;

  uint32_t index = x * (HEIGHT / 8) + y / 8;
  uint8_t  bit   = 7 - (y & 7);

  if (color) _buffer[index] |=  (1 << bit);
  else       _buffer[index] &= ~(1 << bit);
}

void GDcanvas::clearBuffer(uint8_t color) {
  memset(_buffer, color, sizeof(_buffer));
}