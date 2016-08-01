//**********************
// AM2320.cpp for mbed
//
// AM2320 am2320(P0_5,P0_4);
// or
// I2C i2c(P0_5,P0_4);
// AM2320 am2320(i2c);
//
// (C)Copyright 2015 All rights reserved by Y.Onodera
// http://einstlab.web.fc2.com
//**********************

#include "mbed.h"
#include "AM2320.h"

AM2320::AM2320 (PinName sda, PinName scl) : _i2c(sda, scl) {
}
AM2320::AM2320 (I2C& p_i2c) : _i2c(p_i2c) {
}


void AM2320::get()
{
    // step 1:wakeup
    _i2c.write(AM2320_ADDR, NULL, 0);
    wait_us(800);   // 800us - 3ms

    // step 2:command
    buf[0] = 0x03;  // Get Humidity and Temperature
    buf[1] = 0x00;  // Start address
    buf[2] = 0x04;  // Length
    _i2c.write(AM2320_ADDR, buf, 3);
    wait_us(1500);  // 1.5ms

    // step 3:data
    _i2c.read( AM2320_ADDR, buf, 8);

}

unsigned short AM2320::humidity()
{

    // get hum
    get();
    hum.byte.HB=buf[2];
    hum.byte.LB=buf[3];
    return hum.Val;
    
}

signed short AM2320::temperature()
{

    // get temp
    get();
    temp.byte.HB=buf[4];
    temp.byte.LB=buf[5];
    if(temp.Val&0x8000){
        temp.Val&=0x7FFF;
        temp.Val=0xFFFF-temp.Val+1;
    }
    return temp.S;
    
}


