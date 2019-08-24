/* *********************************************************************************************
    Original work Copyright (c) 2016-2017 M5Stack  
    Modified work Copyright (c) 2019 Salvatore Cavallero 

    Licensed under the MIT license.
    
    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
    
 ************************************************************************************/

#include "SH200Q.h"
#include <math.h>
#include <Arduino.h>

SH200Q::SH200Q(){}

void SH200Q::I2C_Read_NBytes(uint8_t driver_Addr, uint8_t start_Addr, uint8_t number_Bytes, uint8_t *read_Buffer)
{
  uint8_t i = 0;
	
  Wire.beginTransmission(driver_Addr);
  Wire.write(start_Addr);  
  Wire.endTransmission(false);
  Wire.requestFrom(driver_Addr,number_Bytes);

  while (Wire.available()) 
	{
    read_Buffer[i++] = Wire.read();
  }        
}

void SH200Q::I2C_Write_NBytes(uint8_t driver_Addr, uint8_t start_Addr, uint8_t number_Bytes, uint8_t *write_Buffer)
{
    Wire.beginTransmission(driver_Addr);
    Wire.write(start_Addr);  
    Wire.write(*write_Buffer); 
    Wire.endTransmission();
}

void SH200Q::ADCReset(void)
{
  unsigned char tempdata[1];
  //set 0xC2 bit2 1-->0
  I2C_Read_NBytes(SH200Q_ADDRESS, SH200Q_ADC_RESET, 1, tempdata);
  
  tempdata[0] = tempdata[0] | 0x04;
  I2C_Write_NBytes(SH200Q_ADDRESS, SH200Q_ADC_RESET, 1, tempdata);
  delay(1);
  
  tempdata[0] = tempdata[0] & 0xFB;
  I2C_Write_NBytes(SH200Q_ADDRESS, SH200Q_ADC_RESET, 1, tempdata);
}

void SH200Q::Reset(void)
{
  unsigned char tempdata[1];
  
  I2C_Read_NBytes(SH200Q_ADDRESS, SH200Q_RESET, 1, tempdata);
  
  //SH200Q_RESET
  tempdata[0] = tempdata[0] | 0x80;
  I2C_Write_NBytes(SH200Q_ADDRESS, SH200Q_RESET, 1, tempdata);
  
  delay(1);
      
  tempdata[0] = tempdata[0] & 0x7F;
  I2C_Write_NBytes(SH200Q_ADDRESS, SH200Q_RESET, 1, tempdata);
}



int SH200Q::Init(void) 
{
  unsigned char tempdata[1];
  

  I2C_Read_NBytes(SH200Q_ADDRESS, SH200Q_WHOAMI, 1, tempdata);
  if(tempdata[0] != 0x18)
  {
    return tempdata[0];
  }
    
  ADCReset();
  
  I2C_Read_NBytes(SH200Q_ADDRESS, 0xD8, 1, tempdata);
  
  tempdata[0] = tempdata[0] | 0x80;
  I2C_Write_NBytes(SH200Q_ADDRESS, 0xD8, 1, tempdata);
  
  delay(1);
      
  tempdata[0] = tempdata[0] & 0x7F;
  I2C_Write_NBytes(SH200Q_ADDRESS, 0xD8, 1, tempdata);
  
  tempdata[0] = 0x61;
  I2C_Write_NBytes(SH200Q_ADDRESS, 0x78, 1, tempdata);
  
  delay(1);
      
  tempdata[0] = 0x00;
  I2C_Write_NBytes(SH200Q_ADDRESS, 0x78, 1, tempdata);
  
  //set acc odr 256hz
  tempdata[0] = 0x91; //0x81 1024hz   //0x89 512hz    //0x91  256hz 
  I2C_Write_NBytes(SH200Q_ADDRESS, SH200Q_ACC_CONFIG, 1, tempdata);
  
  //set gyro odr 500hz
  tempdata[0] = 0x13; //0x11 1000hz    //0x13  500hz   //0x15  256hz
  I2C_Write_NBytes(SH200Q_ADDRESS, SH200Q_GYRO_CONFIG, 1, tempdata);
  
  //set gyro dlpf 50hz
  tempdata[0] = 0x03; //0x00 250hz   //0x01 200hz   0x02 100hz  0x03 50hz  0x04 25hz
  I2C_Write_NBytes(SH200Q_ADDRESS, SH200Q_GYRO_DLPF, 1, tempdata);
  
  //set no buffer mode
  tempdata[0] = 0x00;
  I2C_Write_NBytes(SH200Q_ADDRESS, SH200Q_FIFO_CONFIG, 1, tempdata);
  
  //set acc range +-4G
  tempdata[0] = AFS_4G;
  I2C_Write_NBytes(SH200Q_ADDRESS, SH200Q_ACC_RANGE, 1, tempdata);
  
  //set gyro range +-2000
  tempdata[0] = 0x00;
  I2C_Write_NBytes(SH200Q_ADDRESS, SH200Q_GYRO_RANGE, 1, tempdata);
  
  tempdata[0] = 0xC0;
  I2C_Write_NBytes(SH200Q_ADDRESS, SH200Q_REG_SET1, 1, tempdata);

  I2C_Read_NBytes(SH200Q_ADDRESS, SH200Q_REG_SET2, 1, tempdata);
  
  //ADC Reset
  tempdata[0] = tempdata[0] | 0x10;
  I2C_Write_NBytes(SH200Q_ADDRESS, SH200Q_REG_SET2, 1, tempdata);
  
  delay(1);
        
  tempdata[0] = tempdata[0] & 0xEF;
  I2C_Write_NBytes(SH200Q_ADDRESS, SH200Q_REG_SET2, 1, tempdata);
  
  delay(10);

  getAres();
  getGres();
  return 0x18;
}


void SH200Q::getGres()
{
  switch (Gscale)
  {
    case GFS_125DPS:
      gRes = 125.0/32768.0;
      break;
    case GFS_250DPS:
      gRes = 250.0/32768.0;
      break;
    case GFS_500DPS:
      gRes = 500.0/32768.0;
      break;
    case GFS_1000DPS:
      gRes = 1000.0/32768.0;
      break;
    case GFS_2000DPS:
      gRes = 2000.0/32768.0;
      break;
  }
}

void SH200Q::getAres()
{
  switch (Ascale)
  {
    case AFS_4G:
      aRes = 4.0/32768.0;
      break;
    case AFS_8G:
      aRes = 8.0/32768.0;
      break;
    case AFS_16G:
      aRes = 16.0/32768.0;
      break;
  }
}

void SH200Q::getAccelAdc(int16_t* ax, int16_t* ay, int16_t* az)
{
   uint8_t buf[6];  
   I2C_Read_NBytes(SH200Q_ADDRESS,SH200Q_OUTPUT_ACC,6,buf);
	
   *ax=(int16_t)((buf[1]<<8)|buf[0]);  
   *ay=(int16_t)((buf[3]<<8)|buf[2]);  
   *az=(int16_t)((buf[5]<<8)|buf[4]);
}

void SH200Q::getAccelData(float* ax, float* ay, float* az)
{
   uint8_t buf[6];  
   I2C_Read_NBytes(SH200Q_ADDRESS,SH200Q_OUTPUT_ACC,6,buf);
	
   *ax=(int16_t)((buf[1]<<8)|buf[0]) * aRes;  
   *ay=(int16_t)((buf[3]<<8)|buf[2]) * aRes;  
   *az=(int16_t)((buf[5]<<8)|buf[4]) * aRes;
}

void SH200Q::getGyroAdc(int16_t* gx, int16_t* gy, int16_t* gz)
{
   uint8_t buf[6];  
   I2C_Read_NBytes(SH200Q_ADDRESS,SH200Q_OUTPUT_GYRO,6,buf);
	
   *gx=(int16_t)((buf[1]<<8)|buf[0]);  
   *gy=(int16_t)((buf[3]<<8)|buf[2]);  
   *gz=(int16_t)((buf[5]<<8)|buf[4]);
}

void SH200Q::getGyroData(float* gx, float* gy, float* gz)
{
   uint8_t buf[6];  
   I2C_Read_NBytes(SH200Q_ADDRESS,SH200Q_OUTPUT_GYRO,6,buf);
	
   *gx=(int16_t)((buf[1]<<8)|buf[0]) * gRes;  
   *gy=(int16_t)((buf[3]<<8)|buf[2]) * gRes;  
   *gz=(int16_t)((buf[5]<<8)|buf[4]) * gRes;
}

void SH200Q::getTempAdc(int16_t *t)
{
  uint8_t buf[2];  
  I2C_Read_NBytes(SH200Q_ADDRESS,SH200Q_OUTPUT_TEMP,2,buf);

  *t=(int16_t)((buf[1]<<8)|buf[0]);  
}

void SH200Q::getTempData(float *t)
{
  uint8_t buf[2];  
  I2C_Read_NBytes(SH200Q_ADDRESS,SH200Q_OUTPUT_TEMP,2,buf);

  *t=(int16_t)((buf[1]<<8)|buf[0]) / 333.87 + 21.0;
}
