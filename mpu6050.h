#ifndef MPU6050_H_
#define MPU6050_H_

#include "main.h" // Ð? dùng các hàm HAL và I2C
#include <math.h> // Ð? tính toán góc

// --- C?u trúc d? li?u ch?a t?t c? thông tin c?a MPU6050 ---
typedef struct {
    // 1. C?u hình ph?n c?ng
    I2C_HandleTypeDef *i2c_handle;

    // 2. D? li?u thô (Raw Data)
    int16_t Accel_X_RAW;
    int16_t Accel_Y_RAW;
    int16_t Accel_Z_RAW;
    int16_t Gyro_X_RAW;
    int16_t Gyro_Y_RAW;
    int16_t Gyro_Z_RAW;

    // 3. D? li?u hi?u ch?nh (Calibration)
    long Gyro_X_Cal;
    long Gyro_Y_Cal;
    long Gyro_Z_Cal;

    // 4. Góc tính toán (Output)
    float Angle_Pitch;
    float Angle_Roll;
    
    // 5. Bi?n ph? tr? cho thu?t toán Joop
    float Angle_Pitch_Acc;
    float Angle_Roll_Acc;
    int Set_Gyro_Angles; // C? dánh d?u l?n ch?y d?u

} MPU6050_t;

// --- Khai báo các hàm cho ngu?i dùng g?i ---
uint8_t MPU6050_Init(MPU6050_t *dev, I2C_HandleTypeDef *i2c_handle);
void MPU6050_Calibrate(MPU6050_t *dev);
void MPU6050_Read_All(MPU6050_t *dev);
void MPU6050_Process_Angles(MPU6050_t *dev); // Hàm chính ch?y thu?t toán Joop


#endif /* MPU6050_H_ */
