#include "bno055.h"
#include <stdio.h>

HAL_StatusTypeDef BNO055_Init(I2C_HandleTypeDef *hi2c)
{
    uint8_t tx_data[1];
    HAL_StatusTypeDef ret;
    uint8_t chip_id = 0;

    ret = HAL_I2C_IsDeviceReady(hi2c, BNO055_I2C_ADDR, 2, 100);
    if (ret != HAL_OK)
    {
        printf("BNO055 not found!\n");
        return ret;
    }
    printf("BNO055 detected.\n");

    ret = HAL_I2C_Mem_Read(hi2c, BNO055_I2C_ADDR, BNO055_CHIP_ID_REG, I2C_MEMADD_SIZE_8BIT, &chip_id, 1, 100);
    if (ret != HAL_OK || chip_id != BNO055_CHIP_ID_VAL)
    {
        printf("BNO055 Chip ID error! Read: 0x%02X\n", chip_id);
        return HAL_ERROR;
    }
    printf("Chip ID verified.\n");

    tx_data[0] = BNO055_CONFIG_MODE;
    ret = HAL_I2C_Mem_Write(hi2c, BNO055_I2C_ADDR, BNO055_OPR_MODE_ADDR, I2C_MEMADD_SIZE_8BIT, tx_data, 1, 100);
    if (ret != HAL_OK)
    {
        printf("Failed to enter CONFIG mode.\n");
        return ret;
    }
    HAL_Delay(20);

    tx_data[0] = BNO055_NDOF_MODE;
    ret = HAL_I2C_Mem_Write(hi2c, BNO055_I2C_ADDR, BNO055_OPR_MODE_ADDR, I2C_MEMADD_SIZE_8BIT, tx_data, 1, 100);
    if (ret != HAL_OK)
    {
        printf("Failed to enter NDOF mode.\n");
        return ret;
    }
    HAL_Delay(500);

    printf("BNO055 initialized in NDOF mode.\n");
    return HAL_OK;
}

HAL_StatusTypeDef BNO055_Read_Euler(I2C_HandleTypeDef *hi2c, BNO055_EulerData_t *euler_data)
{
    uint8_t rx_buffer[6];
    int16_t euler_x_raw, euler_y_raw, euler_z_raw;

    HAL_StatusTypeDef ret = HAL_I2C_Mem_Read(
        hi2c,
        BNO055_I2C_ADDR,
        BNO055_EULER_H_LSB,
        I2C_MEMADD_SIZE_8BIT,
        rx_buffer,
        6,
        100);

    if (ret == HAL_OK)
    {
        euler_z_raw = ((int16_t)rx_buffer[1] << 8) | rx_buffer[0];
        euler_y_raw = ((int16_t)rx_buffer[3] << 8) | rx_buffer[2];
        euler_x_raw = ((int16_t)rx_buffer[5] << 8) | rx_buffer[4];

        euler_data->z = (float)euler_z_raw / 16.0f;
        euler_data->y = (float)euler_y_raw / 16.0f;
        euler_data->x = (float)euler_x_raw / 16.0f;

        return HAL_OK;
    }
    else
    {
        return ret;
    }
}
