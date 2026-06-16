/*-------------------------------------------------------------------------------------------------------------------
--
-- Проект      :
-- Модуль      : servo_mg90s
-- Автор       : Караваев К.А.
-- Компания    : МИФИ
-- Файлы       : servo_mg90s.cpp, servo_mg90s.h
--
---------------------------------------------------------------------------------------------------------------------
*/

#include <servo_mg90s.h>
#include <math.h>
#include <stdio.h>
/**
  * @brief  Функция инициализации управляющих пинов для моторов типа jgb37 brushless по умолчению (stm32f411).
  * 		PB6 - TIM4 PWM Channel 1
  * 		PB7 - TIM4 PWM Channel 2
  *
  *
  *	@note   Функция не контроллирует входные порядки фильтров, чтобы не было переполнения контроллируется только
  *			выход за максимальный порядок MAX_ORDER. Так что корректный результат будет только в том случае,
  *			если сумма порядков входных фильтров не больше MAX_ORDER
  *
  *	@note   Обратите внимание, что параллельное соединение возможно только для фильтров одинакового порядка!!!
  *
  *	@param	fp1						-- Структура, содержащая параметры фильтра 1
  *	@param	fp2						-- Структура, содержащая параметры фильтра 2
  * @param	connection_type			-- Тип включения фильтров. SERIAL(0) - последовательное, PARALLEL(1) - параллельное
  * @retval Filter_Parameters		-- Структура, содержащая весовые коэффициенты результирующего фильтра.
  */

//#define __get_pin(pin){(pin ## _Pin)}
//#define __get_port(pin){(pin ## _GPIO_Port)}


//Т.к задали default в заголовке.
//MG90S::MG90S(){}

/*Используем отложенную инициализацию, дабы избежать ситуаций, когда pwm_htim еще не создан кубом,
но уже используется при инициализации в конструкторе. Поэтому конструктор default*/
void MG90S::init(TIM_HandleTypeDef *pwm_htim, uint32_t pwm_chan, uint32_t max_angle){
	if(pwm_htim == nullptr || pwm_htim->Instance == nullptr)
	{
		printf("You have selected an uninitialized timer. Error 1");
		init_error = 1;
	}
	if( (pwm_chan != TIM_CHANNEL_1) && \
		(pwm_chan != TIM_CHANNEL_2) && \
		(pwm_chan != TIM_CHANNEL_3) && \
		(pwm_chan != TIM_CHANNEL_4) )
	{
		printf("Incorrect PWM channel is set during servo initialization. Error 2");
		init_error = 2;
	}
	pwm_htim_  = pwm_htim;
	pwm_chan_  = pwm_chan;
	max_angle_ = max_angle;


	HAL_TIM_PWM_Start(pwm_htim_, pwm_chan_);
/*Для классических сервоприводов типа mg90s минимальный угол соответсвует заполнению 5%, а максимальный - 10%, то есть диапазон
изменения, отражаемый в angle_norn_coef_, равен 5%, или 0.05 от ARR регистра + дополнительная нормировка на max_angle_, чтобы
отнормировать в диапазоне [0;1] задаваемый пользователем в функции spin угол */


	min_angle_ccr_ = pwm_htim_->Init.Period *0.05;
	angle_norn_coef_ = pwm_htim_->Init.Period*0.05 / max_angle_;
}

void MG90S::spin(uint32_t angle){
	angle = (angle < max_angle_) ? angle : max_angle_;
	__HAL_TIM_SET_COMPARE(pwm_htim_, pwm_chan_, (uint32_t)(min_angle_ccr_ + angle*angle_norn_coef_));
}
