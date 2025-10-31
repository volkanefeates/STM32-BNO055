#ifndef INC_BNO055_H_
#define INC_BNO055_H_

#include "main.h"

#define BNO055_I2C_ADDR 0x50
#define BNO055_OPR_MODE_ADDR  0x3D
#define BNO055_EULER_H_LSB    0x1A
#define BNO055_CONFIG_MODE    0x00
#define BNO055_NDOF_MODE      0x0C
#define BNO055_CHIP_ID_REG    0x00
#define BNO055_CHIP_ID_VAL    0xA0


typedef struct {
    float x;
    float y;
    float z;
} BNO055_EulerData_t;

HAL_StatusTypeDef BNO055_Init(I2C_HandleTypeDef *hi2c);

HAL_StatusTypeDef BNO055_Read_Euler(I2C_HandleTypeDef *hi2c, BNO055_EulerData_t *euler_data);


#endif
