#ifndef __USER_H__
#define __USER_H__

#include "bspsystem.h"
void BH170_WriteReg(uint8_t reg_add,uint8_t reg_dat);
void BH170_ReadData(uint8_t reg_add,unsigned char*Read,uint8_t num);
void PWM_AdjustRoad(void);


void BH117_rData(void);
void  DUST_Read(void);
void delay_us(uint32_t us);
void GPIO_IO_IN(void);
void GPIO_IO_OUT(void);

void DHT11_Init(void);
uint8_t DHT11_CheckReponse(void);
uint8_t DHT11_Read_Byte(void);
void DHT11_ReadData(void);
//uint8_t DHT11_Read_Data(uint8_t *temp,uint8_t *humi) ;

void Noise_pro(void);




#endif
