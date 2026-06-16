/*-------------------------------------------------------------------------------------------------------------------
--
-- Проект      :
-- Модуль      : sonar
-- Автор       : Караваев К.А.
-- Компания    : МИФИ
-- Файлы       : sonar.cpp, sonar.h
--
---------------------------------------------------------------------------------------------------------------------
*/

#include "sonar.h"
#include <stdio.h>

void SONAR::init(TIM_HandleTypeDef *capture_htim, uint32_t capture_chan_rise, uint32_t capture_chan_fall, GPIO_TypeDef * sonar_fire_port, uint16_t sonar_fire_pin){ //, uint32_t max_distance_mm){
	if(capture_htim == nullptr || capture_htim->Instance == nullptr)
	{
		printf("You have selected an uninitialized timer. Error 5");
		init_error = 5;
	}
	if( (capture_chan_rise != TIM_CHANNEL_1) && \
		(capture_chan_rise != TIM_CHANNEL_2) && \
		(capture_chan_rise != TIM_CHANNEL_3) && \
		(capture_chan_rise != TIM_CHANNEL_4) )
	{
		printf("Incorrect capture channel is set during sonar initialization. Error 6");
		init_error = 6;
	}
	else if( (capture_chan_fall != TIM_CHANNEL_1) && \
		(capture_chan_fall != TIM_CHANNEL_2) && \
		(capture_chan_fall != TIM_CHANNEL_3) && \
		(capture_chan_fall != TIM_CHANNEL_4) )
	{
		printf("Incorrect capture channel is set during sonar initialization. Error 7");
		init_error = 7;
	}

	capture_htim_ = capture_htim;
	capture_chan_rise_ = capture_chan_rise;
	capture_chan_fall_ = capture_chan_fall;
	sonar_fire_port_ = sonar_fire_port;
	sonar_fire_pin_ = sonar_fire_pin;
//	max_distance_mm_ = max_distance_mm;

	capture_tick_to_ms_ =  (capture_htim_->Init.Prescaler * 1000.) / HAL_RCC_GetSysClockFreq(); //множитель 1000 - для перевода в мс.
	mks_to_systick_ =  HAL_RCC_GetSysClockFreq() /  1000000;


	HAL_TIM_IC_Start(capture_htim_, capture_chan_rise_);
	HAL_TIM_IC_Start(capture_htim_, capture_chan_fall_);
}

void SONAR::fire(void){

	//Обнуляем счетчик, чтобы при измерении не было переполнения, когда захват фронта произошел перед переполнением, а захват среза - после.
	__HAL_TIM_SET_COUNTER(capture_htim_, 0);

	//Выдаем прямоугольный импульс длительностью FIRE_DURATION_MKS на пин Trig для генерации звуковой волны датчиком
	HAL_GPIO_WritePin(sonar_fire_port_, sonar_fire_pin_, GPIO_PIN_SET);
//	uint32_t dwt_tick = DWT->CYCCNT;
//    while( (DWT->CYCCNT) < (dwt_tick + FIRE_DURATION_MKS *mks_to_systick_) ){};
	for(int i = 0; i < (int)(FIRE_DURATION_MKS *mks_to_systick_); i++){
		__NOP();
	}
	HAL_GPIO_WritePin(sonar_fire_port_, sonar_fire_pin_, GPIO_PIN_RESET);
}

uint32_t SONAR::measure(void){
	uint32_t captured_value;
	uint32_t distance;
	//Считаем разницу в тиках таймера между захватом фронта и среза импульса Echo с датчика
	captured_value = HAL_TIM_ReadCapturedValue(capture_htim_, capture_chan_fall_) - HAL_TIM_ReadCapturedValue(capture_htim_, capture_chan_rise_);
//	//Ограничиваем максимально измеряемое расстояние
//	distance = ( (distance = capture_tick_to_ms_*captured_value * 340./2.) > max_distance_mm_) ? max_distance_mm_ : distance;
	distance = capture_tick_to_ms_*captured_value * 340./2.;
	return distance;  // делим на 2, так как волна проходит удвоенный путь датчик-стена-датчик
}

