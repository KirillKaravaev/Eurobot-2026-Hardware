/*-------------------------------------------------------------------------------------------------------------------
--
-- Проект      :
-- Модуль      : motor_jgb37_bl
-- Автор       : Караваев К.А.
-- Компания    : МИФИ
-- Файлы       : motor_jgb37_bl.cpp, motor_jgb37_bl.h
--
---------------------------------------------------------------------------------------------------------------------
--
-- Описание    : Программа для управления бесколлекторными моторами jgb37 brushless и считывания оборотов
--
---------------------------------------------------------------------------------------------------------------------
--
-- V0.0 (01.01.2026)  - исходная версия с ШИМ-управлением моторами и считыванием оборотов через прерывание (измерение
--                      длительности импульсов с датчиков холла таймером в режиме захвата + скользящее среднее) или
--                      специальный регистр ETR + скользящее среднее (для минимизации погрешности)
--
---------------------------------------------------------------------------------------------------------------------
*/

#ifndef __MOTORS_H__
#define __MOTORS_H__

#include <main.h>

extern "C"{
#include "DSPlib.h"
}

/*Wiring:
 * Blue		- pwm input to speed control
 * White	- cw/ccw input to control the direction of rotation
 * Yellow	- output for hall sensor, 6 pulses per full rotate*/
//int motor_jgb37_bl_default_init();
//https://probitteh.detektorpoligraf.ru/stm32-schetchik-vneshnikh-impul-sov/?ysclid=md1pmcawhh690149958
//Макроопределение для быстрого задания регистра CCR соответствующего канала
#define __HAL_TIM_SET_COMPARE_FAST(__HANDLE__, __CHANNEL__, __COMPARE__) \
do { \
    volatile uint32_t* _ccr = &(__HANDLE__)->Instance->CCR1; \
    *((__IO uint32_t*)((uint32_t)_ccr + (__CHANNEL__))) = (__COMPARE__); \
} while(0)

typedef enum{
	RPM_UNDEFINED,
	RPM_ETR,
	RPM_CAPTURE
}rpm_type_t;

extern uint32_t init_error;


class JGB37BL
{
	public:
//		JGB37BL() = default;
		JGB37BL();
		void motor_init(bool inversion, TIM_HandleTypeDef *pwm_htim, uint32_t pwm_chan, GPIO_TypeDef * dir_port, uint16_t dir_pin);
		void encoder_init(float rpm_div, uint16_t avg_num, TIM_HandleTypeDef *etr_htim);
		void encoder_init_it(float rpm_div, uint16_t avg_num, TIM_HandleTypeDef *capture_htim, uint32_t capture_chan);

		void spin(int speed);
		void stop(void);
		float get_rpm(float dt_s);
		float get_rpm_it(void);
		void rpm_capture_it(TIM_HandleTypeDef *htim);

	private:
		//Пользовательские параметры
		float speed_norn_coef_;			//Нормировочный коэффициент для приведения диапазона speed к [0;100]. Равен period/100
		bool inversion_;
		float rpm_div_;
		uint16_t avg_num_;


		//Хардверная часть
		TIM_HandleTypeDef *pwm_htim_;
		uint32_t pwm_chan_;
		GPIO_TypeDef *dir_port_;
		uint16_t dir_pin_;

		TIM_HandleTypeDef *etr_htim_;

		TIM_HandleTypeDef *capture_htim_;
		uint32_t capture_chan_;
		uint16_t capture_active_chan_;		//fucking system, cant use the same defines for timer chan and active chan!!!!


		//Служебнаые переменные и параметры
//		float capture_tick_to_hz_;			//Множитель для перевода тиков таймера захвата в секунды
		float cntr_tick_to_rpm_;			//Множитель для перевода тиков таймера (захвата или ETR) в обороты в минуту
		int cur_speed_;				    	//Текущая скорость вращения
//		bool capture_flag_;					//Флаг захвата, сигнализирующий о вращении. Выставляется в 1 при захвате в прерывании, сбрасывается при расчете
		float capture_cntr_it_;
		unsigned long prev_rpm_time_;
		uint16_t prev_capture_rpm_;
		rpm_type_t rpm_type_;
		Rolling_Avg_Statef avg_st_;
//		uint32_t *prev_captured_val;		//Указатель на динамический массив с предыдущими значениями захваченных значений таймера измерения оборотов в режиме capture



};

#endif /*__MOTORS_H__*/
