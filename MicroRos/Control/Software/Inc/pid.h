/*-------------------------------------------------------------------------------------------------------------------
--
-- Проект      :
-- Модуль      : pid
-- Автор       : Караваев К.А.
-- Компания    : МИФИ
-- Файлы       : pid.cpp, pid.h
--
---------------------------------------------------------------------------------------------------------------------
--
-- Описание    : Программа для пид-регулирования
--
---------------------------------------------------------------------------------------------------------------------
--
-- V0.0 (01.01.2026)  - исходная версия
--
---------------------------------------------------------------------------------------------------------------------
*/

#ifndef PID_H
#define PID_H

#define RPM_MIN 0
#define RPM_MAX 107
#define K_P 0.6
#define K_I 0.8
#define K_D 0.5

class PID
{
    public:
        PID(float min_val, float max_val, float kp, float ki, float kd);
        float compute(float setpoint, float measured_value, float dt);
        void updateConstants(float kp, float ki, float kd);

    private:
        float min_val_;
        float max_val_;
        float kp_;
        float ki_;
        float kd_;
        double integral_;
        double derivative_;
        double prev_error_;
};

#endif
