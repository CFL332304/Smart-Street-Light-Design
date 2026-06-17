#ifndef __BSPSYSTEM_H__
#define __BSPSYSTEM_H__

#include "main.h"
#include "schedule.h"
#include "key.h"
#include "RegularProcess.h"
#include "oled.h"
#include "user.h"
#include "i2c.h"
#include "stdio.h"
#include "tim.h"
#include "adc.h"
#include "string.h"
#include "math.h"
#include "usart.h"
extern uint32_t LightData_Hex;
extern float filtered_density;	
extern float  humidity,tempertaure;
extern float rms;
extern uint8_t string[30];

extern float voltage_rms;

#endif
