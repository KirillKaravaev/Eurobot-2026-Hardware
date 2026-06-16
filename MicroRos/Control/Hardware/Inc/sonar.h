/*-------------------------------------------------------------------------------------------------------------------
--
-- Проект      :
-- Модуль      : sonar
-- Автор       : Караваев К.А.
-- Компания    : МИФИ
-- Файлы       : sonar.cpp, sonar.h
--
---------------------------------------------------------------------------------------------------------------------
--
-- Описание    : Программа для управления сонарами типа SR04M
--
---------------------------------------------------------------------------------------------------------------------
--
-- V0.0 (01.01.2026)  - исходная версия с измерением длительности импульса таймером в режиме захвата
--
---------------------------------------------------------------------------------------------------------------------
*/

#ifndef __SONAR_H__
#define __SONAR_H__

#include <main.h>

#define FIRE_DURATION_MKS 10

extern uint32_t init_error;

class SONAR
{
	public:
		SONAR() = default;
		void init(TIM_HandleTypeDef *capture_htim, uint32_t capture_chan_rise, uint32_t capture_chan_fall, GPIO_TypeDef * sonar_fire_port, uint16_t sonar_fire_pin);//, uint32_t max_distance_mm);
		void fire(void);
		uint32_t measure(void);

	private:
		TIM_HandleTypeDef *capture_htim_;
		uint32_t capture_chan_rise_;
		uint32_t capture_chan_fall_;
		uint32_t distance_;
		GPIO_TypeDef * sonar_fire_port_;
		uint16_t sonar_fire_pin_;
//		uint32_t max_distance_mm_;
//		uint32_t prev_distance_;
		float mks_to_systick_;
		float capture_tick_to_ms_;
};




#endif /*__SONAR_H__*/
