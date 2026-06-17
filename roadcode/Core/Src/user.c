#include "user.h"

/*

根据光照强度调整PWM占空比，从而实现调光。

*/
uint16_t PWMCompare_value;
void PWM_AdjustRoad(void)
{
    if(LightData_Hex > 150) {
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 0);
        return;
    }
    if(LightData_Hex <= 20) {
        float normalized_light = LightData_Hex / 15.0;
        PWMCompare_value = 500 * exp(-1.2 * normalized_light);
    }
    else if(LightData_Hex <= 85) {
        float normalized_light = (LightData_Hex - 15) / 70.0;
         PWMCompare_value = 100 * exp(-2.0 * normalized_light);
    }
    else {
        float normalized_light = (LightData_Hex - 85) / 65.0;
        PWMCompare_value = 15 * exp(-3.0 * normalized_light);
    }
    if(PWMCompare_value < 0) {
        PWMCompare_value = 0;
    } else if(PWMCompare_value > 1000) {
        PWMCompare_value = 1000;
    }
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, PWMCompare_value);
}

/*

基于LM2407的噪声音量测量

*/

uint8_t Noise_index;
uint32_t Noise_adc_value[100];
uint32_t DC_offset;
uint8_t jianshi;
float rms;
float ac_value;
float voltage_rms;
void Noise_pro(void)
{
	HAL_ADC_Start(&hadc2);
  if(++Noise_index<100) {
		Noise_adc_value[Noise_index]=HAL_ADC_GetValue(&hadc2);
		DC_offset=DC_offset+Noise_adc_value[Noise_index];
		return;
	}
	for(uint16_t i=0;i<Noise_index;i++)
	{
		DC_offset=DC_offset+Noise_adc_value[i];
	}
	DC_offset=DC_offset/Noise_index;

  rms = (float)(DC_offset * 3.3 / 4096) / 3.3*100-30;
	DC_offset=0;
	Noise_index=0;
	memset(&Noise_adc_value,0,100);
}




/*

基于DHT11的温湿度数据测量
单总线PA5数据收、严格的时序要求，需要每500ms采取一次数据即可

*/

uint8_t dht_status=1;

void DHT11_Init(void)
{
	if(dht_status!=1) return;
	dht_status=0;
	GPIO_IO_OUT();
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);
	delay_us(1);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);
	HAL_Delay(20);
	
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);
	delay_us(30);
	
	GPIO_IO_IN();
}
uint8_t jianshistaus;
uint32_t timeout;
uint8_t DHT11_Check(void) 	   
{   
	uint8_t retry=0;
  jianshi=13;	
    while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET&&retry<100)//DHT11会拉低40~80us
	{
		retry++;
		delay_us(1);
	};
	
	if(retry>=100)return 1;
	else retry=0;
    while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_RESET&&retry<100)//DHT11拉低后会再次拉高40~80us
	{
		retry++;
		delay_us(1);
	};
	jianshistaus=1;
	if(retry>=100)return 1;	    
	return 0;
}

// 读取一个字节的数据
uint8_t byte ;
uint8_t jianshishuju;

uint8_t DHT11_Read_Byte(void) {
   byte = 0x00;
	uint8_t temp;
	jianshishuju=0;
    for (uint8_t i = 0; i < 8; i++) {
			//
        // 等待变为低电平
        timeout=0;
			jianshistaus=3;
			  while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET&&timeout<50){
					timeout++;
					delay_us(1);
				}
				timeout=0;
				while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_RESET&&timeout<50)
				{
					timeout++;
					delay_us(1);
				};
        // 电平持续26-28us表示0，70us表示1
        delay_us(40); // 延时40us后检测电平
        jianshistaus=4;
        
        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET) temp=1;
				else temp=0;
	
				jianshistaus=5;
				byte <<= 1;
				byte |= temp;
					jianshishuju++;
            // 等待高电平结束	
    }
    
    return byte;
}

float  humidity,tempertaure;
uint8_t data[5] = {0};
void DHT11_ReadData(void)
{
	  memset(&data,0,sizeof(data));// 启动读取过程
    DHT11_Init();
    if (!DHT11_Check()) {         // 检查DHT11响应
			for (uint8_t i_d = 0; i_d < 5; i_d++) {     // 读取40位数据
					data[i_d] = DHT11_Read_Byte();
			}
    }
		else {
		 dht_status = 1; // 返回就绪状态
      return;
		}
		GPIO_IO_OUT();
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);//收回总线，停止传输
		jianshistaus=6;
    if (data[4] == (data[0] + data[1] + data[2] + data[3])) {    //校验数据
		humidity= data[0]+ data[1] / 10.0f;    // 湿度整数 + 小数部分
    tempertaure= data[2]+data[3] / 10.0f;         // 温度整数 + 小数部分
    dht_status = 1; // 返回就绪状态
		jianshistaus=8;
    return;
    }
		else {
		  dht_status = 1; // 返回就绪状态
			jianshistaus=7;
      return ;
		}   
}

//配置GPIO为输入模式
static void GPIO_IO_IN(void)
{
	  GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
//配置GPIO为输出模式
static void GPIO_IO_OUT(void)  {
	  GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}
/*

基于GP2Y的烟尘数据测量

*/

volatile enum {
  DUST_STATE_IDLE,
  DUST_STATE_LED_ON,
  DUST_STATE_ADC_READY
} dust_state = DUST_STATE_IDLE;

uint32_t adc1_value;
uint8_t Data_ready;


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim==&htim2){
		switch(dust_state) {
            case DUST_STATE_IDLE: 
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);// 开启LED并设置280μs后转换状态
                dust_state = DUST_STATE_LED_ON;
								__HAL_TIM_SET_AUTORELOAD(&htim2,29);
								__HAL_TIM_SET_COUNTER(&htim2, 0); // 将定时器2的计数器清零
                break;
            case DUST_STATE_LED_ON:
                __HAL_TIM_SET_AUTORELOAD(&htim2,29);  // 280μs后启动ADC转换 (10kHz时钟，2.8个计数 ≈ 3)
                __HAL_TIM_SET_COUNTER(&htim2, 0); // 将定时器2的计数器清零
						    HAL_ADC_Start(&hadc1);
                    dust_state = DUST_STATE_ADC_READY;
                break;
            case DUST_STATE_ADC_READY:
								__HAL_TIM_SET_AUTORELOAD(&htim2,954);       // 320μs后读取ADC值 (3.2个计数 ≈ 3)
								__HAL_TIM_SET_COUNTER(&htim2, 0); // 将定时器2的计数器清零
                    if (HAL_ADC_PollForConversion(&hadc1, 1) == HAL_OK) {
                        adc1_value = HAL_ADC_GetValue(&hadc1);
                        Data_ready = 1;
                    }
                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);     // 关闭LED，等待周期结束
                    dust_state = DUST_STATE_IDLE;
									HAL_TIM_Base_Stop_IT(&htim2);
                break;
							}					
		}

}
float filtered_density ;
void  DUST_Read(void) {
    
    const float alpha = 0.2;  // 滤波系数
    HAL_TIM_Base_Start_IT(&htim2);
    if(Data_ready) {
        Data_ready = 0;
        
        // 计算电压值 (3.3V参考电压，12-bit ADC)
        float voltage = (adc1_value * 3.3f)*2 / 4095.0f;
        
        // 转换为粉尘浓度
        float density = 0;
        if (voltage >= 0.1f) {
            density = (0.17f * voltage - 0.1f) * 1000.0f;
            if (density < 0) density = 0;
        }
        
        // 一阶低通滤波
        filtered_density = alpha * density + (1 - alpha) * filtered_density;
    }
}



/*

基于BH117的光照强度测量

*/

uint8_t DataBuff[2];//存储数组
float LightData;    //光照强度（含小数位，单位lx）
uint32_t LightData_Hex;//光照强度（整数，单位lx）
void BH170_WriteReg(uint8_t reg_add,uint8_t reg_dat)
{
	HAL_I2C_Mem_Write(&hi2c1,0x46,reg_add,1,&reg_dat,1,1000);//调用HAL库IIC写驱动
}

void BH170_ReadData(uint8_t reg_add,unsigned char*Read,uint8_t num)
{
 HAL_I2C_Mem_Read(&hi2c1,0x46,reg_add,1,Read,num,1000);//调用HAL库IIC读驱动
}
uint8_t jump_flag=0;
uint32_t jump_task_time; 
void BH117_rData(void)
{
	if(jump_flag==0) 
		{
			BH170_WriteReg(0x01,0x00);// power on
			BH170_WriteReg(0x10,0x00);//H- resolution mode
			jump_flag=1;
			jump_task_time=uwTick;
		}
		if(uwTick-jump_task_time<180) return;
		else {
			jump_task_time=0;
			jump_flag=0;
		}
		BH170_ReadData(0,DataBuff,2);//读取数据
		LightData=((DataBuff[0]<<8)+DataBuff[1])/1.2f;//数据转换成光强度，单位lx
    LightData_Hex=LightData;//float转换成整数
}

void delay_us(uint32_t us)
{

    uint16_t differ=0xffff-us-5;                    //设定定时器计数器起始值

    __HAL_TIM_SET_COUNTER(&htim3,differ);

    HAL_TIM_Base_Start(&htim3);                     //启动定时器

 while(differ<0xffff-6)                             //补偿，判断

 {
  differ=__HAL_TIM_GET_COUNTER(&htim3);             //查询计数器的计数值

 }
 HAL_TIM_Base_Stop(&htim3);
}