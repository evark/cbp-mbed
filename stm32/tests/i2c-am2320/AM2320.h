//**********************
// AM2320.h for mbed
//
// (C)Copyright 2015 All rights reserved by Y.Onodera
// http://einstlab.web.fc2.com
//**********************
#ifndef AM2320_H_
#define AM2320_H_

#define AM2320_ADDR    0xB8
#define AM2320_HUM_H  0x00
#define AM2320_HUM_L  0x01
#define AM2320_TEMP_H  0x02
#define AM2320_TEMP_L  0x03

#include "mbed.h"
#include "typedef.h"

class AM2320{
public:
    AM2320 (PinName sda, PinName scl);
    AM2320 (I2C& p_i2c);

    void get();
    unsigned short humidity();
    signed short temperature();

protected:
    
    I2C _i2c;

    WORD_VAL hum;
    WORD_VAL temp;
    char buf[8];

};

#endif /* AM2320_H_ */



