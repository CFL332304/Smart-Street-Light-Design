#include "user.h"
#include "usart.h"
#include "string.h"
#include <stdio.h>

uint16_t point2 = 0;
char c;//指令 0:停止  1:开始
__align(8) unsigned char USART2_TX_BUF[USART2_MAX_SEND_LEN]; 	//发送缓冲,最大USART2_MAX_SEND_LEN字节
unsigned char USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//接收缓冲,最大USART2_MAX_RECV_LEN个字节.

void USART2_CLR_Buf(void)                           // 串口缓存清理
{
	memset(USART2_RX_BUF, 0, USART2_MAX_RECV_LEN);      //清空
  point2 = 0;                    
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart2)	//串口2接收
	{
		if(point2<USART2_MAX_RECV_LEN)		//还可以接收数据
		{
			USART2_RX_BUF[point2++]=c;		//记录接收到的值	 
		} 
	
	}
	

	//重新设置中断
		HAL_UART_Receive_IT(huart, (uint8_t *)&c, 1);  
}
