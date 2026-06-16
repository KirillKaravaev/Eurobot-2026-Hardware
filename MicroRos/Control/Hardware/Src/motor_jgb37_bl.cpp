/*-------------------------------------------------------------------------------------------------------------------
--
-- Проект      :
-- Модуль      : motor_jgb37_bl
-- Автор       : Караваев К.А.
-- Компания    : МИФИ
-- Файлы       : motor_jgb37_bl.cpp, motor_jgb37_bl.h
--
---------------------------------------------------------------------------------------------------------------------
*/

#include <motor_jgb37_bl.h>
#include <math.h>
#include <stdio.h>
#include <rmw_microros/rmw_microros.h>


uint32_t cptr;
//extern int32_t e_cntr;

JGB37BL::JGB37BL():
		rpm_type_(RPM_UNDEFINED)
//		Rolling_Avg_Statef()
	{}
/**
  * @brief  Функция инициализации управляющих пинов для моторов типа jgb37 brushless по умолчению (stm32f411).
  * 		PB6 - TIM4 PWM Channel 1
  * 		PB7 - TIM4 PWM Channel 2
  *
  *	@param	inversion				-- Инверсия вращения мотора. 1 - инвертирован, 0 - нет
  *
  *	@param	pwm_htim				-- Хардверный таймер, управляющий генерацией ШИМ сигнала для мотора.
  *									   Должен быть предварительно сконфигурирован в кубе, может быть общим
  *									   у нескольких ОДИНАКОВЫХ моторов
  *
  * @param	pwm_chan				-- Канал выбранного хардверного таймера, который генерирует ШИМ для мотора
  * 								   Индивидуален для каждого мотора. Для f411 может принимать следующие значения
  * 								   				TIM_CHANNEL_1
  * 								   				TIM_CHANNEL_2
  * 								   				TIM_CHANNEL_3
  * 								   				TIM_CHANNEL_4
  *
  * @param	dir_port				-- Порт мк, один из пинов которого управляет направлением вращения мотора
  *
  * @param	dir_pin					-- Сам пин выбранного порта мк , который управляет направлением вращения мотора
  *
  * @retval none
  */

void JGB37BL::motor_init(bool inversion, TIM_HandleTypeDef *pwm_htim, uint32_t pwm_chan, GPIO_TypeDef * dir_port, uint16_t dir_pin){
	if(pwm_htim == nullptr || pwm_htim->Instance == nullptr)
	{
		printf("You have selected an uninitialized timer. Error 3");
		init_error = 3;
		return;
	}
	if( (pwm_chan != TIM_CHANNEL_1) && \
		(pwm_chan != TIM_CHANNEL_2) && \
		(pwm_chan != TIM_CHANNEL_3) && \
		(pwm_chan != TIM_CHANNEL_4) )
	{
		printf("Incorrect PWM channel is set during motor initialization. Error 4");
		init_error = 4;
		return;
	}
	inversion_ = inversion;
	pwm_htim_ = pwm_htim;
	pwm_chan_ = pwm_chan;
	dir_port_ = dir_port;
	dir_pin_ = dir_pin;

	speed_norn_coef_ = pwm_htim_->Init.Period / 100.;

	HAL_TIM_PWM_Start(pwm_htim_, pwm_chan_);
//	HAL_TIM_Base_Start(rpm_htim_);
//	HAL_TIM_Base_Start_IT(rpm_htim_);
}

void JGB37BL::encoder_init(float rpm_div, uint16_t avg_num, TIM_HandleTypeDef *etr_htim){
	if(etr_htim == nullptr || etr_htim->Instance == nullptr)
	{
		printf("You have selected an uninitialized timer. Error 3");
		init_error = 3;
		return;
	}
	rpm_type_ = RPM_ETR;
	rpm_div_ = rpm_div;
	avg_num_ = (avg_num > 1) ? avg_num : 2;	//минимальное усреднение по 2 значениям
	etr_htim_ = etr_htim;

//	cntr_tick_to_rpm_ = 60./rpm_div_;

	avg_st_._In_Buff = (float*)malloc(sizeof(float)*avg_num_);
	//Если не удалось выделить память, то выводим сообщение об ошибке и блокируем вычисление rpm путем установки rpm_type_ = RPM_UNDEFINED
	if(avg_st_._In_Buff == nullptr){
		rpm_type_ = RPM_UNDEFINED;
		printf("Can't malloc memory. Error 20");
		init_error = 20;
		return;
	}
	avg_st_._sum = 0;

	HAL_TIM_Base_Start(etr_htim_);
//	HAL_TIM_IC_Start(etr_htim_, TIM_CHANNEL_1);
}
void JGB37BL::encoder_init_it(float rpm_div, uint16_t avg_num, TIM_HandleTypeDef *capture_htim, uint32_t capture_chan){
	if(capture_htim == nullptr || capture_htim->Instance == nullptr)
	{
		printf("You have selected an uninitialized timer. Error 3");
		init_error = 3;
		return;
	}
	rpm_type_ = RPM_CAPTURE;
	rpm_div_ = rpm_div;
	avg_num_ = (avg_num > 1) ? avg_num : 2;	//минимальное усреднение по 2 значениям
	capture_htim_ = capture_htim;
	capture_chan_ = capture_chan;
	capture_active_chan_ = 1 << (capture_chan_ >> 2); //to convert capture chan to active we should devide it on 4 and rise 2 to resulted power. res = capture / 4 ; active = 2^res

/*Устанавливаем это значение для того, чтобы даже 32битный таймер сбрасывался по достижении 16 битного значения. Это гарантирует, что при вычитании
 * uint16_t res = current_rpm - previous_rpm получим верное значение res, даже если previous_rpm > current_rpm. Например:
 * uint16_t res = 300 - 65536 = 300 - верное выражени
 * uint32_t res = 300 - 65536 != 300 - неверно, т.к у uint32_t границы переполнения другие
 * глобально это нами используется при расчете rpm в функции get_rpm_it, где тип uint16_t prev_capture_rpm, поэтому задаем перид 65535 */
	capture_htim_->Init.Period = 65535;

	//capture_tick_to_hz_ =  (float)HAL_RCC_GetSysClockFreq() / (capture_htim_->Init.Prescaler)  ;
	//TODO вместо системной частоты HAL_RCC_GetSysClockFreq нужна частота шины, на которой сидит таймер
	float capture_tick_to_hz =  (float)HAL_RCC_GetSysClockFreq() / (capture_htim_->Init.Prescaler)  ;
	cntr_tick_to_rpm_ = capture_tick_to_hz*60. / rpm_div_;


	avg_st_._In_Buff = (float*)malloc(sizeof(float)*avg_num_);
	//Если не удалось выделить память, то выводим сообщение об ошибке и блокируем вычисление rpm путем установки rpm_type_ = RPM_UNDEFINED
	if(avg_st_._In_Buff == nullptr){
		rpm_type_ = RPM_UNDEFINED;
		printf("Can't malloc memory. Error 20");
		init_error = 20;
		return;
	}
	avg_st_._sum = 0;

	HAL_TIM_IC_Start_IT(capture_htim_, capture_chan_);
}

void JGB37BL::spin(int speed){

//	if(speed > 100) speed = 100;
//	else if(speed < -100) speed = -100;
	speed = fminf(100, fmaxf(-100, speed));		//Ограничиваем скорость в пределах [-100;100]
	cur_speed_ = speed;
	//Инверсия необходима только на этапе выдачи управляющего воздействия на мотор
	if(inversion_) speed *= -1;

	if(speed >= 0 ){
		HAL_GPIO_WritePin(dir_port_, dir_pin_, GPIO_PIN_SET);
	}
	else if (speed < 0){
		HAL_GPIO_WritePin(dir_port_, dir_pin_, GPIO_PIN_RESET);
		speed *= -1;
	}
	__HAL_TIM_SET_COMPARE(pwm_htim_, pwm_chan_, (uint32_t)(speed*speed_norn_coef_));
}

void JGB37BL::stop(void){
	__HAL_TIM_SET_COMPARE(pwm_htim_, pwm_chan_, 0);
}


float JGB37BL::get_rpm(float dt_s){
	if(rpm_type_ == RPM_ETR){
		int32_t etr_cntr = __HAL_TIM_GET_COUNTER(etr_htim_);
		//TODO here we should connect to chan1 only!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		__HAL_TIM_SET_COUNTER(etr_htim_ , 0);
		etr_cntr = (cur_speed_ > 0) ? etr_cntr : -etr_cntr;
//    unsigned long now = uxr_millis();
//    float dt_s = (now - prev_rpm_time_) / 1000.0;
//    prev_rpm_time_ = now;
		float rpm = (etr_cntr*60.)/(rpm_div_ * dt_s);
		float avg_rpm = Rolling_Avg_f( rpm , avg_num_, &avg_st_);

		Rolling_Avg_rst_err(avg_num_, &avg_st_);
		return avg_rpm;
//		return (etr_cntr*60.)/(rpm_div_ * dt_s);

	}
	return 0;
}


//float JGB37BL::get_rpm(float dt_s){
//	if(rpm_type_ == RPM_ETR){
//		int32_t etr_cntr = __HAL_TIM_GET_COUNTER(etr_htim_);
//		int32_t capture_cntr = HAL_TIM_ReadCapturedValue(etr_htim_, TIM_CHANNEL_1);
//		float cntr_avg = (float)(etr_cntr + capture_cntr) / 2.;
//		__HAL_TIM_SET_COUNTER(etr_htim_ , 0);
//		cntr_avg = (speed_positive_ == true) ? cntr_avg : -cntr_avg;
////    unsigned long now = uxr_millis();
////    float dt_s = (now - prev_rpm_time_) / 1000.0;
////    prev_rpm_time_ = now;
//		float rpm = (cntr_avg*60.)/(rpm_div_ * dt_s);
//		float avg_rpm = Rolling_Avg_f( rpm , avg_num_, &avg_st_);
//
//		Rolling_Avg_rst_err(avg_num_, &avg_st_);
//		return avg_rpm;
////		return (etr_cntr*60.)/(rpm_div_ * dt_s);
//
//	}
//	return 0;
//}

float JGB37BL::get_rpm_it(void){
	if(rpm_type_ == RPM_CAPTURE){
//		float rpm_capture = capture_tick_to_hz_*60. / (capture_cntr_it_* rpm_div_);

		cptr = capture_cntr_it_;

		float rpm_capture = cntr_tick_to_rpm_ / capture_cntr_it_;
		if(cur_speed_ < 0 ){
			rpm_capture = -rpm_capture;
		}
		else if(__builtin_expect((cur_speed_ == 0), 0)){
			rpm_capture = 0;
			Rolling_Avg_f( 65535 , avg_num_, &avg_st_);		//Также дополнительно заполняем среднее максимально возможными значениями, что эквивалентно остановке
		}
//		rpm_capture = (cur_speed_ > 0) ? rpm_capture : -rpm_capture;
		Rolling_Avg_rst_err(avg_num_, &avg_st_);
//	capture_flag_ = false;
		return rpm_capture;
	}

	return 0;
}
//TODO при остановке моторов сделать расчет скользящего среднего в get_rpm_it (постепенный сброс скорости) и проверить, что не может быть ситуации, когда скорость вращения настолько низкая, что таймер переполняется , а значит в скеорости будет выброс

void JGB37BL::rpm_capture_it(TIM_HandleTypeDef *htim){
	if( (htim == capture_htim_) && (htim->Channel == (capture_active_chan_)) ){
//		capture_flag_ = true;
		//измеряем захваченное значение и обнуляем таймер, тем самым каждое новое захваченное значение будет соответствовать длительности импульса
		uint16_t temp_capture_rpm = HAL_TIM_ReadCapturedValue(capture_htim_, capture_chan_);
//		__HAL_TIM_SET_COUNTER(capture_htim_, 0);
		capture_cntr_it_ = Rolling_Avg_f( (uint16_t)(temp_capture_rpm - prev_capture_rpm_) , avg_num_, &avg_st_);
		prev_capture_rpm_ = temp_capture_rpm;
	}
}


//float JGB37BL::get_rps(void){
//	uint32_t cntr = __HAL_TIM_GET_COUNTER(rpm_htim_);
//	e_cntr = cntr;
//	__HAL_TIM_SET_COUNTER(rpm_htim_ , 0);
//    unsigned long now = uxr_millis();
//    float dt_s = (now - prev_rpm_time_) / 1000.0;
//    prev_rpm_time_ = now;
//	return ((float)cntr)/(rpm_div_ * dt_s);
//}

