/*-------------------------------------------------------------------------------------------------------------------
--
-- Проект      :
-- Модуль      : servo_mg90s
-- Автор       : Караваев К.А.
-- Компания    : МИФИ
-- Файлы       : servo_mg90s.cpp, servo_mg90s.h
--
---------------------------------------------------------------------------------------------------------------------
--
-- Описание    : Программа для управления серводвигателями типа mg90 (ШИМ 50 Гц)
--
---------------------------------------------------------------------------------------------------------------------
--
-- V0.0 (01.01.2026)  - исходная версия с ШИМ-управлением сервоприводами
--
---------------------------------------------------------------------------------------------------------------------
*/

#ifndef __SERVO_H__
#define __SERVO_H__

#include <main.h>

/*Wiring:
 * Blue		- pwm input to speed control
 * White	- cw/ccw input to control the direction of rotation
 * Yellow	- output for hall sensor, 6 pulses per full rotate*/
//int motor_jgb37_bl_default_init();
//https://probitteh.detektorpoligraf.ru/stm32-schetchik-vneshnikh-impul-sov/?ysclid=md1pmcawhh690149958
extern uint32_t init_error;

class MG90S
{
	public:
		MG90S() = default;
		void init(TIM_HandleTypeDef *pwm_htim, uint32_t pwm_chan, uint32_t max_angle);
//		void default_init();
		void spin(uint32_t angle);
	private:
		float angle_norn_coef_;
		float min_angle_ccr_;
		TIM_HandleTypeDef *pwm_htim_ = nullptr;
		uint32_t pwm_chan_;
		uint32_t max_angle_;
};

#endif /*__SERVO_H__*/
