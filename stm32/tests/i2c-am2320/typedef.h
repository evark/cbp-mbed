//**********************
// typedef for mbed
//
// Condition:
//
// (C)Copyright 2014 All rights reserved by Y.Onodera
// http://einstlab.web.fc2.com
//**********************
#ifndef TYPEDEF_H
#define TYPEDEF_H

typedef unsigned char       BYTE;                           /* 8-bit unsigned  */
typedef unsigned short int  WORD;                           /* 16-bit unsigned */
typedef unsigned int        DWORD;                          /* 32-bit unsigned */
typedef unsigned long long  QWORD;                          /* 64-bit unsigned */


typedef union
{
    WORD Val;
    BYTE v[2];
    signed short S;
    struct
    {
        BYTE LB;
        BYTE HB;
    } byte;
} WORD_VAL;

typedef union
{
    DWORD Val;
    WORD w[2];
    BYTE v[4];
    struct
    {
        WORD LW;
        WORD HW;
    } word;
    struct
    {
        BYTE LB;
        BYTE HB;
        BYTE UB;
        BYTE MB;
    } byte;
    struct
    {
        WORD_VAL low;
        WORD_VAL high;
    }wordUnion;

} DWORD_VAL;

#endif /* TYPEDEF_H */
