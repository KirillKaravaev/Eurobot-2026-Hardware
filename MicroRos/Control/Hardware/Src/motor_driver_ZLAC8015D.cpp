/*-------------------------------------------------------------------------------------------------------------------
--
-- Проект      :
-- Модуль      : motor_driver_ZLAC8015D
-- Автор       : Караваев К.А.
-- Компания    : МИФИ
-- Файлы       : motor_driver_ZLAC8015D.cpp, motor_driver_ZLAC8015D.h
--
---------------------------------------------------------------------------------------------------------------------
*/
#include <motor_driver_ZLAC8015D.h>
#include <math.h>
#include <stdio.h>
//#include <rmw_microros/rmw_microros.h>


ZLAC8015D::ZLAC8015D():
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

void ZLAC8015D::motors_init(bool left_motor_inversion, bool right_motor_inversion, UART_HandleTypeDef *huart, GPIO_TypeDef * MAX485_enable_port, uint16_t MAX485_enable_pin){
	if(huart == nullptr || huart->Instance == nullptr)
	{
		printf("You have selected an uninitialized uart. Error 3");
		init_error = 3;
		return;
	}
	huart_ = huart;
	MAX485_enable_port_ = MAX485_enable_port;
	MAX485_enable_pin_ = MAX485_enable_pin;
	left_motor_inversion_ = (left_motor_inversion == true) ? -1 : 1 ;
	right_motor_inversion_ = (right_motor_inversion == true) ? -1 : 1;

}

void ZLAC8015D::encoders_init(float rpm_div, uint16_t avg_num, TIM_HandleTypeDef *etr_htim){
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
void ZLAC8015D::encoders_init_it(float rpm_div, uint16_t avg_num, TIM_HandleTypeDef *capture_htim, uint32_t capture_chan){
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

void ZLAC8015D::encoders_init_driver(){

}

void ZLAC8015D::enable(void){
    memset(hex_cmd, 0, sizeof(hex_cmd));
    hex_cmd[0] = ID;
    hex_cmd[1] = WRITE;
    hex_cmd[2] = 0x20;
    hex_cmd[3] = 0x0E;
    hex_cmd[4] = 0x00;
    hex_cmd[5] = 0x08;

    calculate_crc(WRITE_LEN);

    HAL_GPIO_WritePin(MAX485_enable_port_, MAX485_enable_pin_, GPIO_PIN_SET);
    HAL_UART_Transmit(huart_, hex_cmd, WRITE_LEN, 10);
}

void ZLAC8015D::spin(int16_t left_motor_rpm, int16_t right_motor_rpm){
    memset(hex_cmd, 0, sizeof(hex_cmd));
    hex_cmd[0] = ID;
    hex_cmd[1] = MULTI_WRITE;
    hex_cmd[2] = 0x20;
    hex_cmd[3] = 0x88;
    hex_cmd[4] = 0x00;
    hex_cmd[5] = 0x02;
    hex_cmd[6] = 0x04;

    left_motor_rpm = left_motor_rpm * left_motor_inversion_;
    hex_cmd[7] = (left_motor_rpm >> 8) & 0xFF;
    hex_cmd[8] = left_motor_rpm & 0xFF;

    right_motor_rpm = right_motor_rpm * right_motor_inversion_;
    hex_cmd[9] = (right_motor_rpm >> 8) & 0xFF;
    hex_cmd[10] = right_motor_rpm & 0xFF;

    calculate_crc(MULTI_WRITE_LEN);

    HAL_GPIO_WritePin(MAX485_enable_port_, MAX485_enable_pin_, GPIO_PIN_SET);
//    HAL_UART_Transmit_IT(huart_, hex_cmd, MULTI_WRITE_LEN);
    HAL_UART_Transmit(huart_, hex_cmd, MULTI_WRITE_LEN, 10);
}

void ZLAC8015D::stop(void){
    memset(hex_cmd, 0, sizeof(hex_cmd));
    hex_cmd[0] = ID;
    hex_cmd[1] = WRITE;
    hex_cmd[2] = 0x20;
    hex_cmd[3] = 0x0E;
    hex_cmd[4] = 0x00;
    hex_cmd[5] = 0x07;

    calculate_crc(WRITE_LEN);

    HAL_GPIO_WritePin(MAX485_enable_port_, MAX485_enable_pin_, GPIO_PIN_SET);
    HAL_UART_Transmit(huart_, hex_cmd, WRITE_LEN, 10);
}


float ZLAC8015D::get_rpm(float dt_s){
	if(rpm_type_ == RPM_ETR){
		int32_t etr_cntr = __HAL_TIM_GET_COUNTER(etr_htim_);
		//TODO here we should connect to chan1 only!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		__HAL_TIM_SET_COUNTER(etr_htim_ , 0);
		etr_cntr = (speed_positive_ == true) ? etr_cntr : -etr_cntr;
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

float ZLAC8015D::get_rpm_it(void){
	if(rpm_type_ == RPM_CAPTURE){
//		float rpm_capture = capture_tick_to_hz_*60. / (capture_cntr_it_* rpm_div_);
		float rpm_capture = cntr_tick_to_rpm_ / capture_cntr_it_;
		rpm_capture = (speed_positive_ == true) ? rpm_capture : -rpm_capture;
		Rolling_Avg_rst_err(avg_num_, &avg_st_);
		return rpm_capture;
	}
	return 0;
}

void ZLAC8015D::rpm_capture_it(TIM_HandleTypeDef *htim){
	if( (htim == capture_htim_) && (htim->Channel == (capture_active_chan_)) ){
		//измеряем захваченное значение и обнуляем таймер, тем самым каждое новое захваченное значение будет соответствовать длительности импульса
		uint16_t temp_capture_rpm = HAL_TIM_ReadCapturedValue(capture_htim_, capture_chan_);
//		__HAL_TIM_SET_COUNTER(capture_htim_, 0);
		capture_cntr_it_ = Rolling_Avg_f( (uint16_t)(temp_capture_rpm - prev_capture_rpm_) , avg_num_, &avg_st_);
		prev_capture_rpm_ = temp_capture_rpm;
	}
}

void ZLAC8015D::get_rpm_driver(float *rpm_left, float *rpm_right){
    memset(hex_cmd, 0, sizeof(hex_cmd));
    hex_cmd[0] = ID;
    hex_cmd[1] = READ;
    hex_cmd[2] = 0x20;
    hex_cmd[3] = 0xAB;

    hex_cmd[4] = 0x00;	//Читаем 2 регистра, начиная с 0x20AB, тем самым читаем и 0x20AC
    hex_cmd[5] = 0x02;

    calculate_crc(WRITE_LEN);

    //Записываем WRITE_LEN байт
    HAL_GPIO_WritePin(MAX485_enable_port_, MAX485_enable_pin_, GPIO_PIN_SET);
    HAL_UART_Transmit(huart_, hex_cmd, WRITE_LEN, 10);

    //Читаем MULTI_READ_LEN байт (см пример чтения в документации)
    HAL_GPIO_WritePin(MAX485_enable_port_, MAX485_enable_pin_, GPIO_PIN_RESET);
    HAL_UART_Receive(huart_, hex_cmd, MULTI_READ_LEN, 10);

    *rpm_left = ((int16_t)((hex_cmd[3] << 8) + hex_cmd[4]))*left_motor_inversion_/10.f ;
    *rpm_right = ((int16_t)((hex_cmd[5] << 8) + hex_cmd[6]))*right_motor_inversion_/10.f;
//    return (float)rpm_tenth / 10.0f;
}

void ZLAC8015D::calculate_crc(uint16_t len)
{
    // calculate crc and append to hex cmd
    unsigned short result = crc16(hex_cmd, len - 2);
    hex_cmd[len - 2] = result & 0xFF;
    hex_cmd[len - 1] = (result >> 8) & 0xFF;
}
