#include "schedule.h"

typedef struct{
	void(*func)(void);
	uint32_t time;
	uint32_t last_time;
}task_pro;

static task_pro tasks[]={
	{led_pro,500,0},
	{key_pro,100,0},
	{OLED_showdata,100,0},
	{BH117_rData,100,0},
	{DUST_Read,50,0},
	{DHT11_ReadData,800,0},
	{Noise_pro,10,0},
	{PWM_AdjustRoad,20,0},
	{UART1_SendData,1000,0}
};

uint8_t task_num;
void schedule_init(void)
{
	task_num=sizeof(tasks)/sizeof(tasks[0]);
}

uint8_t i;
uint32_t nowtime;
void schedule_pro(void)
{
	
	for(i=0;i<task_num;i++)
	{
		nowtime=HAL_GetTick();
		if(nowtime-tasks[i].last_time>=tasks[i].time)
		{
			tasks[i].func();
			tasks[i].last_time=nowtime;
		}
	}
}
