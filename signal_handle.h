#ifndef SIGNAL_HANDLE_H
#define SIGNAL_HANDLE_H
#include "main.h"
#include "string.h"
#include "stdlib.h"
typedef struct {
	
    int throttle;    // Ga (1000 - 2000)
    float pitch;     // Góc Pitch mong mu?n (Setpoint)
    float roll;      // Góc Roll mong mu?n (Setpoint)
    uint8_t is_new_data; // C? báo hi?u có d? li?u m?i

}remote_control;
void Uart_control_init(UART_HandleTypeDef *huart);
void Uart_control_init(UART_HandleTypeDef *huart);
void uart_control_handle(remote_control *rc);
#endif
