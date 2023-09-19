/**
 * \file HT1621_KWS-V20.cpp
 * \brief Implementation of a class for dealing with the Holtek HT1621 chip.
 * \author Enrico Formenti
 * \date 31 january 2015
 * \version 1.0
 * \copyright BSD license, check the License page on the blog for more information. All this text must be
 *  included in any redistribution.
 *  <br><br>
 *  See macduino.blogspot.com for more details.
 *
 */

#include "HT1621_KWS-V20.h"

byte setDigits( long number, byte *d, byte len ) // returns # of digits
{
  static byte  idx; // static stays

  long  fullrange = number;

  if ( fullrange < 0L )
  {
    fullrange *= -1L;
  }

  for ( idx = 0; idx < len; idx++ )
  {
    d[ idx ] = 0;
  }

  //                        Here is the actual conversion to array
  idx = 0;
  while (( fullrange ) && ( idx < len ))
  {
    d[ idx++ ] = fullrange % 10;
    fullrange /= 10;
  }
  //                        Finished actual conversion to array

  if( number < 0 && idx < len ) {
    d[ idx++ ] = 11;
  }


  return idx;
}

void HT1621::clear(int places) 
{
    for (uint8_t i = 0; i < places; i++)
        write(i, 0);
}

void HT1621::flush() 
{
    clear();
}

void HT1621::begin()
{
    pinMode(_DATA_pin, OUTPUT);
    pinMode(_RW_pin, OUTPUT);
    pinMode(_CS_pin, OUTPUT);
 
    digitalWrite(_CS_pin, HIGH);
    digitalWrite(_RW_pin, HIGH);
    digitalWrite(_DATA_pin, HIGH);

#ifndef __HT1621_READ
    register uint8_t i;
    
    for(i=0; i<16; i++)
        ram[i] = 0;
#endif

    delay(10);
    clear();
    
}

// OCIO !!!
// nell'esempio dopo ogni write viene dato un delay
// di 20 microsecondi...
void HT1621::writeBits(uint8_t data, uint8_t cnt)
{
    register uint8_t i;

    for(i=0;i<cnt;i++,data <<=1)
    {
        digitalWrite(_RW_pin, LOW);
        delayMicroseconds(20);
        digitalWrite(_DATA_pin, data&0x80 ? HIGH : LOW);
        delayMicroseconds(20);
        digitalWrite(_RW_pin, HIGH);
        delayMicroseconds(20);
    }
}

#ifdef __HT1621_READ

uint8_t HT1621::readBits(uint8_t cnt)
{
    uint8_t data, i, state;
    
    
    pinMode(_DATA_pin, INPUT);
    
    for(i=0, data=0; i<cnt; data <<= 1, i++) {
        digitalWrite(_RW_pin, LOW);
        delayMicroseconds(20);
        data |= (digitalRead(_DATA_pin) == HIGH);
        digitalWrite(_RW_pin, HIGH);
        delayMicroseconds(20);
    }
    
    pinMode(_DATA_pin, OUTPUT);
    
    return data;
}

#endif

void HT1621::sendCommand(uint8_t cmd, bool first, bool last)
{
    if (first) {
        TAKE_CS();
        writeBits(COMMAND_MODE, 4);
    }
    
    writeBits(cmd, 8);
    
    if (last)
        RELEASE_CS();
}

void HT1621::write(uint8_t address, uint8_t data)
{
    TAKE_CS();
    
    writeBits(WRITE_MODE, 3);
    writeBits(address<<3, 6); // 6 is because max address is 128
    writeBits(data, 8);
#ifndef __HT1621_READ
    ram[address] = data;
#endif
    
    RELEASE_CS();
}
void HT1621::writeChar(uint8_t address, uint8_t c, bool decimal)
{
    TAKE_CS();
    if( decimal )
        write(address, charMap[c]|DECIMAL);
    else
        write(address, charMap[c]);
    RELEASE_CS();
}

void HT1621::write(uint8_t address, uint8_t data, uint8_t cnt)
{
    register uint8_t i;
    
    TAKE_CS();
    
    writeBits(WRITE_MODE, 3);
    writeBits(address<<3, 6);
    for (i = 0; i < cnt; i++) {
        writeBits(data, 8);
#ifndef __HT1621_READ
        ram[i] = data;
#endif
    }
    
    RELEASE_CS();
}

void HT1621::write(uint8_t address, uint8_t *data, uint8_t cnt)
{
    register uint8_t i;
    
    TAKE_CS();
    
    writeBits(WRITE_MODE, 3);
    writeBits(address<<3, 6);
    for (i = 0; i < cnt; i++) {
        writeBits(data[i], 8);
#ifndef __HT1621_READ
        ram[i] = data[i];
#endif
    }
    
    RELEASE_CS();
}

#ifdef __HT1621_READ

uint8_t HT1621::read(uint8_t address)
{
    uint8_t data;
    
    TAKE_CS();
    
    writeBits(READ_MODE, 3);
    writeBits(address<<3, 6);
    data = readBits(8);
    
    RELEASE_CS();
    
    return data;
}

void HT1621::read(uint8_t address, uint8_t *data, uint8_t cnt)
{
    register uint8_t i;
    
    TAKE_CS();
    
    writeBits(READ_MODE, 3);
    writeBits(address<<3, 6);
    for (i = 0; i < cnt; i++)
        data[i] = readBits(8);
    
    RELEASE_CS();
}

#else

uint8_t HT1621::read(uint8_t address)
{
    return ram[address];
}

void HT1621::read(uint8_t address, uint8_t *data, uint8_t cnt)
{
    register uint8_t i;
    
    for (i = 0; i < cnt; i++)
        data[i] = ram[address+i];
}

void HT1621::printNumber(long number, int places, int dec, bool flushdisplay)
{
  if ( flushdisplay ) clear();
  byte d[ places ];
  byte converted;
  int decpoint = places - dec - 1;
  int len = setDigits( number, d, places );
  int pos = places - 1; //-1 as we start at index LEN-1 (end)
  for (int i = 0; i < len; i++) {
    bool addDec = false;
    if ( pos == decpoint ) addDec = true;
    writeChar(pos, d[i], addDec);
    pos--;
    if ( pos < 0 ) break;
  }
}

void HT1621::printFloat(double number, int places, int decimalplaces, bool flushdisplay) 
{
    long num = (long)(number * pow(10, decimalplaces));
    printNumber(num, places, decimalplaces, flushdisplay);
}

//KWS-V20のLCD用プログラム
void HT1621::writeV20(uint8_t address, uint8_t data){
  byte newdata = 0b00000000;
  byte newdata2 = 0b00000000;
  switch(address){
    case 3:
    case 2:
    case 1:
    case 0:
    case 14:
      for (int i=0;i<8;i++){ //bitを対照に
        bitWrite(newdata,7-i,bitRead(data,i));
      }
      switch(address){
        case 3:
          write(0, newdata);
          break;
        case 2:
          write(1, newdata);
          break;
        case 1:
          write(2, newdata);
          break;
        case 0:
          write(3, newdata);
          break;
        case 14:
          write(7, newdata);
          break;
      }
      break;
    case 4:
    case 5:
    case 6:
      for (int i=0;i<4;i++){
        bitWrite(newdata,i,bitRead(data,i+4)); //efghを4ビットずらす
        bitWrite(newdata,i+4,bitRead(data,i)); //abcdを4ビットずらす
      }
      data = newdata;
      for (int i=0;i<8;i++){ //bitを対照に
        bitWrite(newdata,7-i,bitRead(data,i));
      }
      write(address, newdata);
      break;
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
      for (int i=0;i<8;i++){ //bitを対照に
        bitWrite(newdata,7-i,bitRead(data,i));
      }
      data = newdata;
      //不足分の読み取り
      newdata = read(address+1);//abcdを使用(MSB側4桁は書き換え)
      newdata2 = read(address+2);//efghを使用(LSB側4桁は書き換え)
      for (int i=0;i<4;i++){
        bitWrite(newdata,i,bitRead(data,i)); //efghを4ビットずらす
        bitWrite(newdata2,i+4,bitRead(data,i+4)); //abcdを4ビットずらす
      }
      write(address+1,newdata);
      write(address+2,newdata2);
      break;
    case 13:
      for (int i=0;i<8;i++){ //bitを対照に
        bitWrite(newdata,7-i,bitRead(data,i));
      }
      data = newdata;
      //不足分の読み取り
      newdata = read(14);//abcdを使用(MSB側4桁は書き換え)
      newdata2 = read(8);//efghを使用(LSB側4桁は書き換え)
      for (int i=0;i<4;i++){
        bitWrite(newdata,i,bitRead(data,i)); //efghを4ビットずらす
        bitWrite(newdata2,i+4,bitRead(data,i+4)); //abcdを4ビットずらす
      }
      write(14,newdata);
      write(8,newdata2);
      break;
    default:
      write(address, data);
  }
}

void HT1621::writeCharV20(uint8_t address, uint8_t c, bool decimal){
    //TAKE_CS();
    if( decimal )
        writeV20(address, charMap[c]|DECIMAL);
    else
        writeV20(address, charMap[c]);
    //RELEASE_CS();
}

void HT1621::writeV20voltage(float N,int F,bool V){
  int V0 = 0;
  int V1 = 0;
  int V2 = 0;
  switch(F){
    case 0:
      V0 = N / 100;
      V1 = (N - (V0 * 100)) / 10;
      V2 = N - (V0 * 100) - (V1 * 10);
      if (V0 < 1){ //100の位が0のとき
        writeV20(0,0);
        if (V1 < 1){ //100と10の位が0のとき
          writeV20(1,0);
        }
        else{
          writeCharV20(1,V1,0);
        }
      }
      else{
        writeCharV20(0,V0,0);
        writeCharV20(1,V1,0);
      }
      writeCharV20(2,V2,V); //1の位
      break;
    case 1:
      V0 = (N / 10);
      V1 = N - (V0 * 10);
      V2 = 10 * (N - (V0 * 10) - V1);
      if (V0 < 1){ //10の位が0のとき
        writeV20(0,0);
      }
      else{
        writeCharV20(0,V0,0);
      }
      writeCharV20(1,V1,1);
      writeCharV20(2,V2,V);
      break;
    case 2:
      V0 = N;
      V1 = (10 * N) - (10 * V0);
      V2 = (100 * N) - (V0 * 100) - (V1 * 10);
      writeCharV20(0,V0,1);
      writeCharV20(1,V1,0);
      writeCharV20(2,V2,V);
      break;
  }
}

void HT1621::writeV20current(float N,int F,bool A){
  int A7 = 0;
  int A8 = 0;
  int A9 = 0;
  switch(F){
    case 0:
      A7 = N / 100;
      A8 = (N - (A7 * 100)) / 10;
      A9 = N - (A7 * 100) - (A8 * 10);
      if (A7 < 1){ //100の位が0のとき
        writeV20(7,0);
        if (A8 < 1){ //100と10の位が0のとき
          writeV20(8,0);
        }
        else{
          writeCharV20(8,A8,0);
        }
      }
      else{
        writeCharV20(7,A7,0);
        writeCharV20(8,A8,0);
      }
      writeCharV20(9,A9,A); //1の位
      break;
    case 1:
    case 2:
      A7 = N;
      A8 = (10 * N) - (10 * A7);
      A9 = (100 * N) - (A7 * 100) - (A8 * 10);
      writeCharV20(7,A7,1);
      writeCharV20(8,A8,0);
      writeCharV20(9,A9,A);
      break;
  }
}

void HT1621::writeV20time(int h,int m,bool colon,bool T){
  int T3 = h / 10;
  int T4 = h - (T3 * 10);
  int T5 = m / 10;
  int T6 = m - (T5 * 10);
  writeCharV20(3,T3,0);
  writeCharV20(4,T4,colon);
  writeCharV20(5,T5,0);
  writeCharV20(6,T6,T);
}

void HT1621::writeV20mAh(long N,bool mAh){
  int mAh10 = N / 10000;
  int mAh11 = (N - (mAh10 * 10000)) / 1000;
  int mAh12 = (N - (mAh10 * 10000) - (mAh11 * 1000)) / 100;
  int mAh13 = (N - (mAh10 * 10000) - (mAh11 * 1000) - (mAh12 * 100)) / 10;
  int mAh14 = N - (mAh10 * 10000) - (mAh11 * 1000) - (mAh12 * 100) - (mAh13 * 10);
  if (mAh10 < 1){
    writeV20(10,0);
    if (mAh11 < 1){
      writeV20(11,0);
      if(mAh12 < 1){
        writeV20(12,0);
        if (mAh13 < 1){
          writeV20(13,0);
        }
        else{
          writeCharV20(13,mAh13,0);
        }
      }
      else{
        writeCharV20(12,mAh12,0);
        writeCharV20(13,mAh13,0);
      }
    }
    else{
      writeCharV20(11,mAh11,0);
      writeCharV20(12,mAh12,0);
      writeCharV20(13,mAh13,0);
    }
  }
  else{
    writeCharV20(10,mAh10,0);
    writeCharV20(11,mAh11,0);
    writeCharV20(12,mAh12,0);
    writeCharV20(13,mAh13,0);
  }
  writeCharV20(14,mAh14,mAh);
  
}

#endif
