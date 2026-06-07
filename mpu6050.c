#include "mpu6050.h"

// Ð?a ch? I2C (0x68 << 1)
#define MPU6050_ADDR 0xD0
#define WHO_AM_I     0x75
#define PWR_MGMT_1   0x6B
#define GYRO_CONFIG  0x1B
#define ACCEL_CONFIG 0x1C
#define CONFIG       0x1A
#define ACCEL_XOUT_H 0x3B

// 1. Hàm Kh?i t?o
uint8_t MPU6050_Init(MPU6050_t *dev, I2C_HandleTypeDef *i2c_handle) {
    dev->i2c_handle = i2c_handle;
    dev->Set_Gyro_Angles = 0;
    dev->Angle_Pitch = 0;
    dev->Angle_Roll = 0;

    uint8_t check;
    uint8_t data;

    // Ki?m tra k?t n?i
    HAL_I2C_Mem_Read(dev->i2c_handle, MPU6050_ADDR, WHO_AM_I, 1, &check, 1, 100);

    if (check == 104) { // 0x68
        // Wake up
        data = 0x00;
        HAL_I2C_Mem_Write(dev->i2c_handle, MPU6050_ADDR, PWR_MGMT_1, 1, &data, 1, 100);

        // Gyro 500dps
        data = 0x08;
        HAL_I2C_Mem_Write(dev->i2c_handle, MPU6050_ADDR, GYRO_CONFIG, 1, &data, 1, 100);

        // Accel +/- 8g
        data = 0x10;
        HAL_I2C_Mem_Write(dev->i2c_handle, MPU6050_ADDR, ACCEL_CONFIG, 1, &data, 1, 100);

        // DLPF ~43Hz
        data = 0x03;
        HAL_I2C_Mem_Write(dev->i2c_handle, MPU6050_ADDR, CONFIG, 1, &data, 1, 100);

        return 0; // OK
    }
    return 1; // Error
}

// 2. Hàm Ð?c d? li?u thô
void MPU6050_Read_All(MPU6050_t *dev) {
    uint8_t Rec_Data[14];
    HAL_I2C_Mem_Read(dev->i2c_handle, MPU6050_ADDR, ACCEL_XOUT_H, 1, Rec_Data, 14, 100);

    dev->Accel_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
    dev->Accel_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data[3]);
    dev->Accel_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data[5]);
    dev->Gyro_X_RAW  = (int16_t)(Rec_Data[8] << 8 | Rec_Data[9]);
    dev->Gyro_Y_RAW  = (int16_t)(Rec_Data[10] << 8 | Rec_Data[11]);
    dev->Gyro_Z_RAW  = (int16_t)(Rec_Data[12] << 8 | Rec_Data[13]);
}

// 3. Hàm Calibration (Ch?y lúc d?u)
void MPU6050_Calibrate(MPU6050_t *dev) {
    dev->Gyro_X_Cal = 0;
    dev->Gyro_Y_Cal = 0;
    dev->Gyro_Z_Cal = 0;

    for (int i = 0; i < 2000; i++) {
        MPU6050_Read_All(dev);
        dev->Gyro_X_Cal += dev->Gyro_X_RAW;
        dev->Gyro_Y_Cal += dev->Gyro_Y_RAW;
        dev->Gyro_Z_Cal += dev->Gyro_Z_RAW;
        HAL_Delay(3);
    }
    dev->Gyro_X_Cal /= 2000;
    dev->Gyro_Y_Cal /= 2000;
    dev->Gyro_Z_Cal /= 2000;
}

// Luu ý: Hàm này gi? d?nh du?c g?i trong vòng l?p 250Hz (4ms)
void MPU6050_Process_Angles(MPU6050_t *dev) {
    // Tr? sai s? tinh
    dev->Gyro_X_RAW -= dev->Gyro_X_Cal;
    dev->Gyro_Y_RAW -= dev->Gyro_Y_Cal;
    dev->Gyro_Z_RAW -= dev->Gyro_Z_Cal;

    // --- Gyro Angle Calculations ---
    // 0.0000611 = 1 / (250Hz * 65.5)
    dev->Angle_Pitch += dev->Gyro_X_RAW * 0.0000611;
    dev->Angle_Roll  += dev->Gyro_Y_RAW * 0.0000611;

    // 0.000001066 = 0.0000611 * (PI / 180) -> Yaw Transfer
    dev->Angle_Pitch += dev->Angle_Roll * sin(dev->Gyro_Z_RAW * 0.000001066);
    dev->Angle_Roll  -= dev->Angle_Pitch * sin(dev->Gyro_Z_RAW * 0.000001066);

    // --- Accel Angle Calculations ---
    long acc_total_vector = sqrt((long)dev->Accel_X_RAW * dev->Accel_X_RAW +
                                 (long)dev->Accel_Y_RAW * dev->Accel_Y_RAW +
                                 (long)dev->Accel_Z_RAW * dev->Accel_Z_RAW);

    if (abs(dev->Accel_Y_RAW) < acc_total_vector) {
        dev->Angle_Pitch_Acc = asin((float)dev->Accel_Y_RAW / acc_total_vector) * 57.296;
    }
    if (abs(dev->Accel_X_RAW) < acc_total_vector) {
        dev->Angle_Roll_Acc = asin((float)dev->Accel_X_RAW / acc_total_vector) * -57.296;
    }

    // Calibration Offset (n?u c?n ch?nh b?ng tay)
    dev->Angle_Pitch_Acc -= 0.0;
    dev->Angle_Roll_Acc  -= 0.0;

    // --- Complementary Filter ---
    if (dev->Set_Gyro_Angles) {
        dev->Angle_Pitch = dev->Angle_Pitch * 0.9996 + dev->Angle_Pitch_Acc * 0.0004;
        dev->Angle_Roll  = dev->Angle_Roll  * 0.9996 + dev->Angle_Roll_Acc  * 0.0004;
    } else {
        dev->Angle_Pitch = dev->Angle_Pitch_Acc;
        dev->Angle_Roll  = dev->Angle_Roll_Acc;
        dev->Set_Gyro_Angles = 1;
    }
}
