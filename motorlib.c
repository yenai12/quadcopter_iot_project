#include "motor.h"


void Motor_Init(void){
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4);
	Motor_Setspeed(1000,1000,1000,1000);
}
void Motor_Setspeed(uint16_t m1,uint16_t m2,uint16_t m3,uint16_t m4){
	if (m1 < 1000) m1 =1000;if ( m1 > 2000) m1=2000;
	if (m2 < 1000) m2 =1000;if ( m2 > 2000) m2=2000;
	if (m3 < 1000) m3 =1000;if ( m3 > 2000) m3=2000;
	if (m4 < 1000) m4 =1000;if ( m4 > 2000) m4=2000;
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,m1);
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,m2);
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,m3);
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,m4	);
}
