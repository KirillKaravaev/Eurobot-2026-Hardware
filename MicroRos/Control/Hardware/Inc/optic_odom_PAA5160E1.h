/*-------------------------------------------------------------------------------------------------------------------
--
-- Проект      :
-- Модуль      : optic_odom_PAA5160E1 (OTOS - Optical Tracking Odometry Sensor)
-- Автор       : Мосиевских Илья
-- Компания    : МИФИ
-- Файлы       : optic_odom_PAA5160E1.cpp, optic_odom_PAA5160E1.h
--
---------------------------------------------------------------------------------------------------------------------
--
-- Описание    : Программа для чтения данных с оптического датчика перемещений PAA5160E1
--
---------------------------------------------------------------------------------------------------------------------
--
-- V0.0 (01.01.2026)  - Исходная версия с функциями чтения положения (в глобальной СК), скорости, ускорения, а также их
--                      среднеквадратичных отклонений
--
-- V0.1 (28.03.2026)  - Изменил тип данных OTOS_Pose -> OTOS_Data
-- V0.2 (15.04.2026)  - Значительно переписана библиотека, введена универсальная функция чтения OTOS_ReadRaw6,
--                      добавлена функция проверки подключения датчика OTOS_IsAlive, добавлена неблокирующая калибровка
--
---------------------------------------------------------------------------------------------------------------------
*/

#ifndef __OTOS_H__
#define __OTOS_H__

#include "stm32h7xx_hal.h"
#include <stdint.h>

#define OTOS_I2C_ADDR (0x17 << 1)

/* Registers */
#define OTOS_REG_POS_X_L 0x20
#define OTOS_REG_VEL_X_L 0x26
#define OTOS_REG_ACC_X_L 0x2C
#define OTOS_REG_STD_POS 0x32
#define OTOS_REG_STD_VEL 0x38
#define OTOS_REG_STD_ACC 0x3A
#define OTOS_REG_RESET   0x07
#define OTOS_REG_CALIB   0x06

/* Timeout */
#define OTOS_I2C_TIMEOUT 10  // ms

/* Scale */
#define OTOS_POS_SCALE   (10.0f / 32768.0f)
#define OTOS_VEL_SCALE   (5.0f / 32768.0f)
#define OTOS_ACC_SCALE   ((16.0f * 9.80665f) / 32768.0f)
#define OTOS_ANGLE_SCALE (3.1415926f / 32768.0f)
#define OTOS_ANGLE_VEL_SCALE ((2000.0f * 3.1415926f / 180.0f) / 32768.0f)
#define OTOS_ANGLE_ACC_SCALE ((3.1415926f * 1000.0f) / 32768.0f)

typedef struct
{
    float x;
    float y;
    float theta;
} OTOS_Data;

typedef struct
{
    I2C_HandleTypeDef *hi2c;
    uint16_t address;
} OTOS_Handle;

/* API */

/*NOTE - use this function before other functions and before OTOS_IsAlive too!*/
HAL_StatusTypeDef OTOS_Init(OTOS_Handle *dev, I2C_HandleTypeDef *hi2c);

/*Can be used after OTOS_Init function*/
HAL_StatusTypeDef OTOS_IsAlive(OTOS_Handle *dev);

HAL_StatusTypeDef OTOS_ResetTracking(OTOS_Handle *dev);

HAL_StatusTypeDef OTOS_ReadPose(OTOS_Handle *dev, OTOS_Data *pose);
HAL_StatusTypeDef OTOS_ReadVelocity(OTOS_Handle *dev, OTOS_Data *vel);
HAL_StatusTypeDef OTOS_ReadAcceleration(OTOS_Handle *dev, OTOS_Data *acc);

HAL_StatusTypeDef OTOS_ReadStdDevPosition(OTOS_Handle *dev, OTOS_Data *std_pose);
HAL_StatusTypeDef OTOS_ReadStdDevVelocity(OTOS_Handle *dev, OTOS_Data *std_vel);
HAL_StatusTypeDef OTOS_ReadStdDevAcceleration(OTOS_Handle *dev, OTOS_Data *std_acc);

/* Calibration (blocking) */
HAL_StatusTypeDef OTOS_CalibrateIMU(OTOS_Handle *dev, uint8_t samples, uint8_t waitUntilDone);

/* Calibration (non-blocking) */
HAL_StatusTypeDef OTOS_StartCalibration(OTOS_Handle *dev, uint8_t samples);
HAL_StatusTypeDef OTOS_IsCalibrationDone(OTOS_Handle *dev, uint8_t *done);

#endif /*__OTOS_H__*/
