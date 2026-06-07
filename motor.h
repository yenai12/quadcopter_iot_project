#ifndef MOTOR_H
#define MOTOR_H

#include "main.h"
extern TIM_HandleTypeDef htim2;

void Motor_Init(void);

void Motor_Setspeed(uint16_t m1,uint16_t m2,uint16_t m3,uint16_t m4);

#endif
