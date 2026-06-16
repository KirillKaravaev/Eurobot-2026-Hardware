/*-------------------------------------------------------------------------------------------------------------------
--
-- Проект      :
-- Модуль      : optic_odom_PAA5160E1 (OTOS - Optical Tracking Odometry Sensor)
-- Автор       : Мосиевских Илья
-- Компания    : МИФИ
-- Файлы       : optic_odom_PAA5160E1.cpp, optic_odom_PAA5160E1.h
--
---------------------------------------------------------------------------------------------------------------------
*/

#include "optic_odom_PAA5160E1.h"

/* ================= INIT ================= */
/*NOTE - use this function before other functions and before OTOS_IsAlive too!*/
HAL_StatusTypeDef OTOS_Init(OTOS_Handle *dev, I2C_HandleTypeDef *hi2c)
{
    if (!dev || !hi2c)
        return HAL_ERROR;

    dev->hi2c = hi2c;
    dev->address = OTOS_I2C_ADDR;

    return HAL_OK;
}

/* ================= LOW LEVEL ================= */

HAL_StatusTypeDef OTOS_IsAlive(OTOS_Handle *dev)
{
    return HAL_I2C_IsDeviceReady(
        dev->hi2c,
        dev->address,
        1,
        OTOS_I2C_TIMEOUT);
}

static HAL_StatusTypeDef OTOS_ReadRaw6(
        OTOS_Handle *dev,
        uint8_t reg,
        int16_t *x,
        int16_t *y,
        int16_t *theta)
{
//    if (OTOS_IsAlive(dev) != HAL_OK)
//        return HAL_ERROR;

    uint8_t buf[6];

    HAL_StatusTypeDef status =
        HAL_I2C_Mem_Read(
            dev->hi2c,
            dev->address,
            reg,
            I2C_MEMADD_SIZE_8BIT,
            buf,
            6,
            OTOS_I2C_TIMEOUT);

    if (status != HAL_OK)
        return status;

    *x = (int16_t)((buf[1] << 8) | buf[0]);
    *y = (int16_t)((buf[3] << 8) | buf[2]);
    *theta = (int16_t)((buf[5] << 8) | buf[4]);

    return HAL_OK;
}

/* ================= RESET ================= */

HAL_StatusTypeDef OTOS_ResetTracking(OTOS_Handle *dev)
{
//    if (OTOS_IsAlive(dev) != HAL_OK)
//        return HAL_ERROR;

    uint8_t cmd = 1;

    return HAL_I2C_Mem_Write(
        dev->hi2c,
        dev->address,
        OTOS_REG_RESET,
        I2C_MEMADD_SIZE_8BIT,
        &cmd,
        1,
        OTOS_I2C_TIMEOUT);
}

/* ================= READ DATA ================= */

HAL_StatusTypeDef OTOS_ReadPose(OTOS_Handle *dev, OTOS_Data *pose)
{
    int16_t x, y, theta;

    HAL_StatusTypeDef status =
        OTOS_ReadRaw6(dev, OTOS_REG_POS_X_L, &x, &y, &theta);

    if (status != HAL_OK)
        return status;

    pose->x = x * OTOS_POS_SCALE;
    pose->y = y * OTOS_POS_SCALE;
    pose->theta = theta * OTOS_ANGLE_SCALE;

    return HAL_OK;
}

HAL_StatusTypeDef OTOS_ReadVelocity(OTOS_Handle *dev, OTOS_Data *vel)
{
    int16_t x, y, theta;

    HAL_StatusTypeDef status =
        OTOS_ReadRaw6(dev, OTOS_REG_VEL_X_L, &x, &y, &theta);

    if (status != HAL_OK)
        return status;

    vel->x = x * OTOS_VEL_SCALE;
    vel->y = y * OTOS_VEL_SCALE;
    vel->theta = theta * OTOS_ANGLE_VEL_SCALE;

    return HAL_OK;
}

HAL_StatusTypeDef OTOS_ReadAcceleration(OTOS_Handle *dev, OTOS_Data *acc)
{
    int16_t x, y, theta;

    HAL_StatusTypeDef status =
        OTOS_ReadRaw6(dev, OTOS_REG_ACC_X_L, &x, &y, &theta);

    if (status != HAL_OK)
        return status;

    acc->x = x * OTOS_ACC_SCALE;
    acc->y = y * OTOS_ACC_SCALE;
    acc->theta = theta * OTOS_ANGLE_ACC_SCALE;

    return HAL_OK;
}

/* ================= STD DEV ================= */

HAL_StatusTypeDef OTOS_ReadStdDevPosition(OTOS_Handle *dev, OTOS_Data *std_pose)
{
    int16_t x, y, theta;

    HAL_StatusTypeDef status =
        OTOS_ReadRaw6(dev, OTOS_REG_STD_POS, &x, &y, &theta);

    if (status != HAL_OK)
        return status;

    std_pose->x = x * OTOS_POS_SCALE;
    std_pose->y = y * OTOS_POS_SCALE;
    std_pose->theta = theta * OTOS_ANGLE_SCALE;

    return HAL_OK;
}

HAL_StatusTypeDef OTOS_ReadStdDevVelocity(OTOS_Handle *dev, OTOS_Data *std_vel)
{
    int16_t x, y, theta;

    HAL_StatusTypeDef status =
        OTOS_ReadRaw6(dev, OTOS_REG_STD_VEL, &x, &y, &theta);

    if (status != HAL_OK)
        return status;

    std_vel->x = x * OTOS_VEL_SCALE;
    std_vel->y = y * OTOS_VEL_SCALE;
    std_vel->theta = theta * OTOS_ANGLE_VEL_SCALE;

    return HAL_OK;
}

HAL_StatusTypeDef OTOS_ReadStdDevAcceleration(OTOS_Handle *dev, OTOS_Data *std_acc)
{
    int16_t x, y, theta;

    HAL_StatusTypeDef status =
        OTOS_ReadRaw6(dev, OTOS_REG_STD_ACC, &x, &y, &theta);

    if (status != HAL_OK)
        return status;

    std_acc->x = x * OTOS_ACC_SCALE;
    std_acc->y = y * OTOS_ACC_SCALE;
    std_acc->theta = theta * OTOS_ANGLE_ACC_SCALE;

    return HAL_OK;
}

/* ================= BLOCKING CALIBRATION ================= */

HAL_StatusTypeDef OTOS_CalibrateIMU(OTOS_Handle *dev, uint8_t samples, uint8_t waitUntilDone)
{
    HAL_StatusTypeDef status;
    uint8_t value;

    status = HAL_I2C_Mem_Write(
            dev->hi2c,
            dev->address,
			OTOS_REG_CALIB,
            I2C_MEMADD_SIZE_8BIT,
            &samples,
            1,
			OTOS_I2C_TIMEOUT);

    if(status != HAL_OK)
        return status;

    HAL_Delay(3);

    if(!waitUntilDone)
        return HAL_OK;

    for(uint8_t attempts = samples; attempts > 0; attempts--)
    {
        status = HAL_I2C_Mem_Read(
                dev->hi2c,
                dev->address,
                0x06,
                I2C_MEMADD_SIZE_8BIT,
                &value,
                1,
				OTOS_I2C_TIMEOUT);

        if(status != HAL_OK)
            return status;

        if(value == 0)
            return HAL_OK;

        HAL_Delay(3);
    }

    return HAL_TIMEOUT;
}

/* ================= NON-BLOCKING CALIBRATION ================= */

HAL_StatusTypeDef OTOS_StartCalibration(OTOS_Handle *dev, uint8_t samples)
{
    if (OTOS_IsAlive(dev) != HAL_OK)
        return HAL_ERROR;

    return HAL_I2C_Mem_Write(
        dev->hi2c,
        dev->address,
        OTOS_REG_CALIB,
        I2C_MEMADD_SIZE_8BIT,
        &samples,
        1,
        OTOS_I2C_TIMEOUT);
}

HAL_StatusTypeDef OTOS_IsCalibrationDone(OTOS_Handle *dev, uint8_t *done)
{
    if (OTOS_IsAlive(dev) != HAL_OK)
        return HAL_ERROR;

    uint8_t value;

    HAL_StatusTypeDef status =
        HAL_I2C_Mem_Read(
            dev->hi2c,
            dev->address,
            OTOS_REG_CALIB,
            I2C_MEMADD_SIZE_8BIT,
            &value,
            1,
            OTOS_I2C_TIMEOUT);

    if (status != HAL_OK)
        return status;

    *done = (value == 0);

    return HAL_OK;
}
