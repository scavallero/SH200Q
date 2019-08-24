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


#ifndef __SH200Q_H__
#define __SH200Q_H__

#include <Wire.h>
#include <Arduino.h>

#define SH200Q_ADDRESS 		0x6C

// SH200Q Register
#define SH200Q_OUTPUT_ACC 	0x00
#define SH200Q_OUTPUT_GYRO 	0x06
#define SH200Q_OUTPUT_TEMP 	0x0C
#define SH200Q_ACC_CONFIG 	0x0E
#define SH200Q_GYRO_CONFIG 	0x0F
#define SH200Q_GYRO_DLPF 	  0x11
#define SH200Q_FIFO_CONFIG 	0x12
#define SH200Q_ACC_RANGE 	  0x16
#define SH200Q_GYRO_RANGE 	0x2B
#define SH200Q_WHOAMI 		  0x30
#define SH200Q_REG_SET1 	  0xBA
#define SH200Q_REG_SET2 	  0xCA
#define SH200Q_ADC_RESET  	0xC2
#define SH200Q_SOFT_RESET 	0x7F
#define SH200Q_RESET 		    0x75

#define RtA     57.324841
#define AtR    	0.0174533	
#define Gyro_Gr	0.0010653


class SH200Q 
{
  // Publick methods 
  public:
  
    SH200Q();
    void I2C_Read_NBytes(uint8_t driver_Addr, uint8_t start_Addr, uint8_t number_Bytes, uint8_t *read_Buffer);
    void I2C_Write_NBytes(uint8_t driver_Addr, uint8_t start_Addr, uint8_t number_Bytes, uint8_t *write_Buffer);
    void ADCReset(void);
    void Reset(void);

    int Init(void);

    void getGres();
    void getAres();
    void getAccelAdc(int16_t* ax, int16_t* ay, int16_t* az);
    void getGyroAdc(int16_t* gx, int16_t* gy, int16_t* gz);
    void getTempAdc(int16_t *t);

    void getAccelData(float* ax, float* ay, float* az);
    void getGyroData(float* gx, float* gy, float* gz);
    void getTempData(float *t);

  // Piblic members
  public:
  
    float aRes, gRes;

  protected:

    // Set initial input parameters
    enum Ascale 
	  {
      AFS_4G = 0,
      AFS_8G,
      AFS_16G
    };

    enum Gscale 
	  {
	    GFS_2000DPS = 0,
	    GFS_1000DPS,
	    GFS_500DPS,
	    GFS_250DPS,
      GFS_125DPS
    };

    // Setup default scale factor
    uint8_t Gscale = GFS_125DPS;
    uint8_t Ascale = AFS_4G;
  
};

#endif
