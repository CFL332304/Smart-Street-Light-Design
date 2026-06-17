#include "RegularProcess.h"

void led_pro(void)
{
	HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
//	DHT11_Read_Data(&tempertaure,&humidity);
}
