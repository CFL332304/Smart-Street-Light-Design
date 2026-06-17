/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "user.h"
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
unsigned long  Time_Cont = 0;       //定时器计数器
#define Success 1U
#define Failure 0U

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */


const char *mqtt_server = "mqtts.heclouds.com"; //onenet 的 IP地址218.201.45.2
const char *port = "1883";                     //端口号

#define mqtt_pubid "VXp0a3H7mC"       //产品ID
#define mqtt_devid "air_test" //设备ID

#define mqtt_token "version=2018-10-31&res=products%2FVXp0a3H7mC%2Fdevices%2Fair_test&et=2228989936&method=md5&sign=%2BvhWX2wsZwS6H0WgKpvnrg%3D%3D" //鉴权信息


#define subscribe "$sys/VXp0a3H7mC/air_test/thing/property/post/reply"	//参考mqtt_pubid和mqtt_devid一起替换对应位置
#define publish "$sys/VXp0a3H7mC/air_test/thing/property/post"		  	//参考mqtt_pubid和mqtt_devid一起替换对应位置

#define paramsName	"temp_value"

uint8_t count = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	
	HAL_UART_Receive_IT(&huart1, (uint8_t *)&c, 1);
	HAL_UART_Receive_IT(&huart2, (uint8_t *)&c, 1);
	
	
	initair780();
	HAL_Delay(100);
	
	
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		
		uint8_t temp_value = 10;
		
		connectMQTTServer();//连接MQTT并订阅
		
//		while(count++ < 5)
//		{
//			publishDataToServer(paramsName,temp_value);	//发送一组数据
//			temp_value += 1;
//			printf("\r\n...Pause\r\n");
//			HAL_Delay(600);
//		}
//		count = 0;
		HAL_Delay(1000);
		publishDataToServer(paramsName,temp_value);
//		if (sendCommand("AT+MIPCLOSE\r\n", "OK\r\n", 5000, 10) == Success);
//		else errorLog(13);

//		if (sendCommand("AT+CIPSHUT\r\n", "OK\r\n", 5000, 10) == Success);
//		else errorLog(13);
//		
		
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

struct __FILE
{
  int handle;
  /* Whatever you require here. If the only file you are using is */
  /* standard output using printf() for debugging, no file handling */
  /* is required. */
};
/* FILE is typedef’d in stdio.h. */
FILE __stdout;
int fputc(int ch, FILE *f) 
{
  /* Your implementation of fputc(). */
	uint8_t temp[1]={ch};
	HAL_UART_Transmit(&huart1,temp,1,2);
  return ch;
}



void publishDataToServer(char* name,uint8_t dat)
{
	char send_buf[255] = {0};
	char tmp[255] = {0};
	memset(send_buf, 0, 255);    //清空
	memset(tmp, 0, 255);    //清空
	strcat(send_buf, "AT+MPUB=\"");
	strcat(send_buf, publish);
	strcat(send_buf, "\", 0, 0,\"");
	sprintf(tmp, "{\\22id\\22:\\22123\\22,\\22params\\22:{\\22%s\\22:{\\22value\\22:%d}}}\"\r\n"
	       ,name ,dat );
	strcat(send_buf, tmp);
	if (sendCommand(send_buf ,"success\"}\r\n",3000, 10) == Success);
	else errorLog(1);
	HAL_Delay(100);
}

void connectMQTTServer(void)
{
	char send_buf[150] = {0};
	memset(send_buf, 0, 150);    //清空
	strcpy(send_buf, "AT+MCONFIG=\"");
	strcat(send_buf, mqtt_devid);
	strcat(send_buf, "\",\"");
	strcat(send_buf, mqtt_pubid);
	strcat(send_buf, "\",\"");
	strcat(send_buf, mqtt_token);
	strcat(send_buf, "\"\r\n");

	if (sendCommand(send_buf, "OK\r\n", 10000, 5) == Success);
	else errorLog(8);


	memset(send_buf, 0, 150);    //清空
	strcpy(send_buf, "AT+MIPSTART=\"");
	strcat(send_buf, mqtt_server);
	strcat(send_buf, "\",");
	strcat(send_buf, port);
	strcat(send_buf, "\r\n");

	if (sendCommand(send_buf, "CONNECT OK\r\n", 10000, 5) == Success);
	else errorLog(8);

	if (sendCommand("AT+MCONNECT=1,120\r\n", "CONNACK OK\r\n", 10000, 5) == Success);
	else errorLog(8);

	memset(send_buf, 0, 150);    //清空
	strcpy(send_buf, "AT+MSUB=\"");
	strcat(send_buf, subscribe);
	strcat(send_buf, "\",0\r\n");
	if (sendCommand(send_buf ,"SUBACK\r\n",3000, 10) == Success);
	else errorLog(10);
	HAL_Delay(100);
}


void initair780(void)
{
	if (sendCommand("AT\r\n", "OK\r\n", 500, 10) == Success);
	else errorLog(1);

	if (sendCommand("AT&F\r\n", "OK\r\n", 3000, 10) == Success);
	else errorLog(1);

	if (sendCommand("AT+CSQ\r\n", "OK\r\n", 3000, 10) == Success);
	else errorLog(1);
	HAL_Delay(100);

	if (sendCommandReceive2Keyword("AT+CPIN?\r\n", "READY","OK\r\n" ,3000, 10) == Success);
	else errorLog(1);
	HAL_Delay(100);

	if (sendCommand("AT+COPS?\r\n", "OK\r\n", 3000, 10) == Success);
	else errorLog(1);
	HAL_Delay(100);

	if (sendCommandReceive2Keyword("AT+CREG?\r\n", ",1","OK\r\n", 3000, 10) == Success);	//本地SIM卡
	else if(sendCommandReceive2Keyword("AT+CREG?\r\n", ",5", "OK\r\n",3000, 10) == Success	);//漫游SIM卡
	else	errorLog(3);

	if (sendCommand("AT+CIPSHUT\r\n", "OK\r\n", 5000, 10) == Success);
	else errorLog(13);

	if (sendCommandReceive2Keyword("AT+CGATT?\r\n", "+CGATT: 1","OK\r\n" ,3000, 10) == Success);
	else errorLog(1);
	HAL_Delay(100);

	if (sendCommand("AT+CSTT\r\n", "OK\r\n", 3000, 10) == Success);
	else errorLog(1);
	HAL_Delay(100);

	if (sendCommand("AT+CIICR\r\n", "OK\r\n", 3000, 10) == Success);
	else errorLog(1);
	HAL_Delay(100);

	if (sendCommandReceive2Keyword("AT+CIFSR\r\n", ".","\r\n" ,3000, 10) == Success);
	else errorLog(1);
	HAL_Delay(100);

}

void Sys_Soft_Reset(void)
{  
    SCB->AIRCR =0X05FA0000|(uint32_t)0x04;      
}

void errorLog(int num)
{
	printf("ERROR%d\r\n",num);
	while (1)
	{
		if (sendCommand("AT\r\n", "OK\r\n", 100, 10) == Success)
		{
			Sys_Soft_Reset();
		}
		HAL_Delay(200);
	}
}

unsigned char nibble2c(unsigned char c)
{
   if ((c>='0') && (c<='9'))
      return c-'0' ;
   if ((c>='A') && (c<='F'))
      return c+10-'A' ;
   if ((c>='a') && (c<='a'))
      return c+10-'a' ;
   return 0 ;
}

unsigned char hex2c(unsigned char c1, unsigned char c2)
{
   if((int)c1>= 0)
     return c1*16 + c2 ;
   return 0 ;
}

unsigned int sendCommandHex(char *Command, unsigned long Timeout, unsigned char Retry)
{
	unsigned char n;
	unsigned char dat = 0,dat1 = 0,dat_hex = 0;
	USART2_CLR_Buf();
	for (n = 0; n < Retry; n++)
	{
		char send_buf[100] = {0};
		memset(send_buf, 0, 100);    //清空 
		
		printf("\r\n***************send****************\r\n");
		printf((const char*)Command);
		
		for(int i = 0 ; i < strlen(Command) ;i++)
		{
			
			if(i%2==0)
			{
				dat = nibble2c(Command[i]);	//获取奇数位的数据
			}
				
			if(i%2==1)
			{
				dat1 = nibble2c(Command[i]);	//获取偶数位的数据

				dat_hex = hex2c(dat,dat1);	//汇总成一个HEX数据	

				send_buf[i/2]=dat_hex;	//拼接到send_buf，等待发送
			}
					
		}
		printf((const char*)send_buf);
		HAL_UART_Transmit(&huart2, (uint8_t *)send_buf, strlen(send_buf),0xFFFF);
		
		Time_Cont = 0;
		while (Time_Cont < Timeout)
		{
			HAL_Delay(100);
			Time_Cont += 100;
			if (strstr((const char*)USART2_RX_BUF, (const char*)send_buf) != NULL)
			{				
				printf("\r\n***************receive****************\r\n");
				printf((const char*)USART2_RX_BUF);
				USART2_CLR_Buf();
				return Success;
			}
			
		}
		Time_Cont = 0;
	}
	printf("\r\n***************receive****************\r\n");
	printf((const char*)USART2_RX_BUF);
	USART2_CLR_Buf();
	return Failure;
}

unsigned int sendCommand(char *Command, char *Response, unsigned long Timeout, unsigned char Retry)
{
	unsigned char n;
	USART2_CLR_Buf();
	for (n = 0; n < Retry; n++)
	{
		HAL_UART_Transmit(&huart2, (uint8_t *)Command, strlen(Command),0xFFFF); 
		
		printf("\r\n***************send****************\r\n");
		printf((const char*)Command);
		
		Time_Cont = 0;
		while (Time_Cont < Timeout)
		{
			HAL_Delay(100);
			Time_Cont += 100;
			if (strstr((const char*)USART2_RX_BUF, (const char*)Response) != NULL)
			{				
				printf("\r\n***************receive****************\r\n");
				printf((const char*)USART2_RX_BUF);
				USART2_CLR_Buf();
				return Success;
			}
			
		}
		printf("\r\n***************receive****************\r\n");
		printf((const char*)USART2_RX_BUF);
		Time_Cont = 0;
	}
	printf("\r\n***************receive****************\r\n");
	printf((const char*)USART2_RX_BUF);
	USART2_CLR_Buf();
	return Failure;
}

unsigned int sendCommandReceive2Keyword(char *Command, char *Response, char *Response2, unsigned long Timeout, unsigned char Retry)
{
	unsigned char n;
	USART2_CLR_Buf();
	for (n = 0; n < Retry; n++)
	{
		HAL_UART_Transmit(&huart2, (uint8_t *)Command, strlen(Command),0xFFFF); 
		
		printf("\r\n***************send****************\r\n");
		printf((const char*)Command);
		
		Time_Cont = 0;
		while (Time_Cont < Timeout)
		{
			HAL_Delay(100);
			Time_Cont += 100;
			if (strstr((const char*)USART2_RX_BUF, (const char*)Response) != NULL  && strstr((const char*)USART2_RX_BUF, (const char*)Response2) != NULL)
			{				
				printf("\r\n***************receive****************\r\n");
				printf((const char*)USART2_RX_BUF);
				USART2_CLR_Buf();
				return Success;
			}
			
		}
		Time_Cont = 0;
	}
	printf("\r\n***************receive****************\r\n");
	printf((const char*)USART2_RX_BUF);
	USART2_CLR_Buf();
	return Failure;
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
