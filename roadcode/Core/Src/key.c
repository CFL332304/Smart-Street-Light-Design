#include "key.h"
#include <stdlib.h>
uint8_t alarm_info;
uint8_t ui;
uint8_t key_down,key_up,key_value,key_last;
void key_pro(void)
{
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5)==SET) key_value=1;
	else if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4)==SET) key_value=2;
	else key_value=0;
	key_down=key_value&(key_value^key_last);
	key_up=~key_value&(key_value^key_last);
	key_last=key_value;
	
	if(key_down==1){
		if(++ui>1) ui=0;
		OLED_Clear();
	}
	
	
	if(filtered_density>100) {
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET);
		alarm_info=1;

	}
	else {
	  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET);
		alarm_info=0;
	}

}




uint8_t string[30];
void OLED_showdata(void)
{
	//HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_12);
	if(ui==0){
	sprintf((char*)string,"  LX: %-d xls",LightData_Hex);
	OLED_ShowString(9,1,string);
	sprintf((char*)string,"  H: %-.1f %%RH",humidity);
	OLED_ShowString(9,3,string);
	sprintf((char*)string,"  T: %-.1f C",tempertaure);
	OLED_ShowString(9,5,string);
	}
	else if(ui==1){
		sprintf((char*)string,"  N:%-.1f dB ",rms);
		OLED_ShowString(9,1,string);
		sprintf((char*)string,"  D:%-.1f ug/m3",filtered_density);
		OLED_ShowString(9,3,string);	
		if(alarm_info==1){
				sprintf((char*)string," exceed ");
				OLED_ShowString(18,6,string);
		}
		else {
				sprintf((char*)string," not exceed ");
				OLED_ShowString(18,6,string);
		}
	}
}

/*

UART1发送数据给从机
每1s发送一次

*/


uint8_t tx_buffer[100];


uint8_t DecData[15];

uint8_t test_data[20];


void extract_float_parts(float *input,uint8_t *output, uint8_t count) {

// 动态分配内存

    for (uint8_t i = 0; i < count; i++) {
        uint8_t base_index = i * 3+1; // 每个数据占用3个位置
        
        // 提取百位数
        output[base_index] = (uint8_t)(input[i] / 100);
        
        // 提取十位和个位组成的两位数
        output[base_index + 1] = ((uint8_t)input[i]) % 100;
        
        // 提取小数位（小数点后第一位）
        float decimal_part = input[i] - (uint8_t)input[i];
        output[base_index + 2] = (uint8_t)(decimal_part * 10 + 0.5); // 加0.5是为了四舍五入
    }
		// 使用完后记得释放内存
		memset(&input,0,sizeof(input));
		output[0]='S';
		output[16]='E';
			for(uint8_t i=0;i<20;i++)
			{
				test_data[i]=output[i];
			}
}
float Data_array[5];

void UART1_SendData(void)
{

		memset(&Data_array,0,sizeof(Data_array));
		Data_array[0]=tempertaure;
		Data_array[1]=humidity;
		Data_array[2]=LightData_Hex;
		Data_array[3]=rms;
		Data_array[4]=filtered_density;
		extract_float_parts(Data_array,DecData,5);
		HAL_UART_Transmit_DMA(&huart1, (uint8_t *)DecData, sizeof(tx_buffer));
}

