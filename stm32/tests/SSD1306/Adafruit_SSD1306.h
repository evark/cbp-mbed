/*********************************************************************
This is a library for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

These displays use SPI to communicate, 4 or 5 pins are required to  
interface

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

/*
 *  Modified by Neal Horman 7/14/2012 for use in mbed
 *  Edited by Francesco Adamo 2015/09/03: 
 *	 - code cleaned up in many points for a better readability
 *	 - removed SPIPreinit and I2CPreinit classes
 *	 - moved various method implementations from .h files to the corresponding .cpp ones
 *	 - the splash() and clearDisplay() methods now directly update the display; no more need to call the display() method after them
 *	
 */

#ifndef _ADAFRUIT_SSD1306_H_
#define _ADAFRUIT_SSD1306_H_

#include "mbed.h"
#include "Adafruit_GFX.h"

#include <vector>
#include <algorithm>

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_CHARGEPUMP 0x8D

#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

// A DigitalOut sub-class that provides a constructed default state
class DigitalOut2 : public DigitalOut
{
public:
	DigitalOut2(PinName pin, bool active = false) : DigitalOut(pin) { write(active); };
	DigitalOut2& operator= (int value) { write(value); return *this; };
	DigitalOut2& operator= (DigitalOut2& rhs) { write(rhs.read()); return *this; };
	operator int() { return read(); };
};


/** The pure base class for the SSD1306 display driver.
 *
 * You should derive from this for a new transport interface type,
 * such as the SPI and I2C drivers.
 */
class Adafruit_SSD1306 : public Adafruit_GFX
{
protected:
	virtual void sendDisplayBuffer() = 0;
	DigitalOut2 rst;

	// the memory buffer for the LCD
	std::vector<uint8_t> buffer;

public:
	Adafruit_SSD1306(PinName RST, uint8_t rawHeight = 32, uint8_t rawWidth = 128);
	void begin(uint8_t switchvcc = SSD1306_SWITCHCAPVCC);
	
	// These must be implemented in the derived transport driver
	virtual uint8_t command(uint8_t c) = 0;
	virtual void data(uint8_t c) = 0;
	virtual void drawPixel(int16_t x, int16_t y, uint16_t color);

	// Clear the display buffer    
	void clearDisplay(void);
	virtual void invertDisplay(bool i);

	// Cause the display to be updated with the buffer content.
	void display();

	// Fill the buffer with the AdaFruit splash screen.
	virtual void splash();
};


/** This is the SPI SSD1306 display driver transport class
 *
 */
class Adafruit_SSD1306_SPI : public Adafruit_SSD1306
{
public:
	/*
	 * Create a SSD1306 SPI transport display driver instance with the specified DC, RST, and CS pins, as well as the display dimensions
	 *
	 * Required parameters
	 * @param MOSI (SPI MOSI pin name)
	 * @param CLK (SPI clock pin name)
	 * @param CS (Chip Select) pin name
	 * @param DC (Data/Command) pin name
	 * @param RST (Reset) pin name
	 *
	 * Optional parameters
	 * @param rawHeight - the vertical number of pixels for the display, defaults to 32
	 * @param rawWidth - the horizontal number of pixels for the display, defaults to 128
	 */
	Adafruit_SSD1306_SPI(PinName MOSI, PinName CLK, PinName CS, PinName DC, PinName RST, uint8_t rawHeight = 32, uint8_t rawWidth = 128);
	virtual uint8_t command(uint8_t);
	virtual void data(uint8_t);
	virtual void sendDisplayBuffer(void);
	
protected:
	DigitalOut2 cs, dc;
	SPI mspi;
};


/*
 * This is the I2C SSD1306 display driver transport class
 *
 */
class Adafruit_SSD1306_I2C : public Adafruit_SSD1306
{
public:
	#define SSD_I2C_ADDRESS     0x78
	/** Create a SSD1306 I2C transport display driver instance with the specified RST pin name, the I2C address, as well as the display dimensions
	 *
	 * Required parameters
	 * @param i2c - A reference to an initialized I2C object
	 * @param RST - The Reset pin name
	 *
	 * Optional parameters
	 * @param i2cAddress - The i2c address of the display
	 * @param rawHeight - The vertical number of pixels for the display, defaults to 32
	 * @param rawWidth - The horizonal number of pixels for the display, defaults to 128
	 */
	Adafruit_SSD1306_I2C(PinName SDA, PinName SCL, PinName RST, uint8_t i2cAddress = SSD_I2C_ADDRESS, uint8_t rawHeight = 32, uint8_t rawWidth = 128);
	virtual uint8_t command(uint8_t c);
	virtual void data(uint8_t c);
	virtual void sendDisplayBuffer();

protected:
	I2C mi2c;
	uint8_t mi2cAddress;
};

#endif