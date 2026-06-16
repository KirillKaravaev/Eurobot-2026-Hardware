/*-------------------------------------------------------------------------------------------------------------------
--
-- Проект      :
-- Модуль      : motor_driver_ZLAC8015D
-- Автор       : Караваев К.А.
-- Компания    : МИФИ
-- Файлы       : motor_driver_ZLAC8015D.cpp, motor_driver_ZLAC8015D.h
--
---------------------------------------------------------------------------------------------------------------------
--
-- Описание    : Программа для управления бесколлекторными моторами и считывания оборотов при помощи драйвера ZLAC8015D
--
---------------------------------------------------------------------------------------------------------------------
--
-- V0.0 (08.02.2026)  - исходная версия с заданием скорости по uart при помощи uart-rs485 конвертера и считыванием
--                      оборотов через прерывание (измерение длительности импульсов с датчиков холла таймером в режиме
--                      захвата + скользящее среднее) или специальный регистр ETR + скользящее среднее (для минимизации
--                      погрешности), или непосредственно с драйвера
--
-- V0.1 (14.02.2026)  - bugfix
-- V1.0 (17.02.2026)  - bugfix. Change UART IT transmittion to standard transmittion, add motor inversion
---------------------------------------------------------------------------------------------------------------------
*/

#ifndef __ZLAC8015D_H__
#define __ZLAC8015D_H__

#include <main.h>

extern "C"{
#include "DSPlib.h"
}

//Макроопределение для быстрого задания регистра CCR соответствующего канала
#define __HAL_TIM_SET_COMPARE_FAST(__HANDLE__, __CHANNEL__, __COMPARE__) \
do { \
    volatile uint32_t* _ccr = &(__HANDLE__)->Instance->CCR1; \
    *((__IO uint32_t*)((uint32_t)_ccr + (__CHANNEL__))) = (__COMPARE__); \
} while(0)

//typedef enum{
//	RPM_UNDEFINED,
//	RPM_ETR,
//	RPM_CAPTURE,
//	RPM_DRIVER,
//}rpm_type_t;

#include "motor_jgb37_bl.h"

extern uint32_t init_error;


class ZLAC8015D
{
	public:
		ZLAC8015D();
		void motors_init(bool left_motor_inversion, bool right_motor_inversion, UART_HandleTypeDef *huart, GPIO_TypeDef * MAX485_enable_port, uint16_t MAX485_enable_pin);
		void encoders_init(float rpm_div, uint16_t avg_num, TIM_HandleTypeDef *etr_htim);
		void encoders_init_it(float rpm_div, uint16_t avg_num, TIM_HandleTypeDef *capture_htim, uint32_t capture_chan);
		void encoders_init_driver();

		void enable(void);
		void spin(int16_t left_motor_rpm, int16_t right_motor_rpm);
		void stop(void);
		float get_rpm(float dt_s);
		float get_rpm_it(void);
		void rpm_capture_it(TIM_HandleTypeDef *htim);
		void get_rpm_driver(float *rpm_left, float *rpm_right);

	private:
		//Пользовательские параметры
		float rpm_div_;
		uint16_t avg_num_;
		int16_t left_motor_inversion_;
		int16_t right_motor_inversion_;


		//Хардверная часть
		UART_HandleTypeDef *huart_;
		GPIO_TypeDef * MAX485_enable_port_;
		uint16_t MAX485_enable_pin_;

		TIM_HandleTypeDef *etr_htim_;

		TIM_HandleTypeDef *capture_htim_;
		uint32_t capture_chan_;
		uint16_t capture_active_chan_;		//fucking system, cant use the same defines for timer chan and active chan!!!!


		//Служебнаые переменные и параметры
//		float capture_tick_to_hz_;			//Множитель для перевода тиков таймера захвата в секунды
		float cntr_tick_to_rpm_;			//Множитель для перевода тиков таймера (захвата или ETR) в обороты в минуту
		bool speed_positive_;				//Переменная для определения направления вращения в блоке get_rpm.
		float capture_cntr_it_;
		unsigned long prev_rpm_time_;
		uint16_t prev_capture_rpm_;
		rpm_type_t rpm_type_;
		Rolling_Avg_Statef avg_st_;
//		uint32_t *prev_captured_val;		//Указатель на динамический массив с предыдущими значениями захваченных значений таймера измерения оборотов в режиме capture


		//Константы и служебные функции
	    uint8_t hex_cmd[13] = {0};
	    uint8_t receive_hex[15] = {0};
	    uint8_t ID = 0x01;
	    const uint8_t READ = 0x03;							//Код функции чтения одного или нескольких регистров modbus
	    const uint8_t READ_LEN = 0x08;
	    const uint8_t MULTI_READ_LEN = 0x09;

	    const uint8_t WRITE = 0x06;							//Код функции записи одного регистра modbus
	    const uint8_t WRITE_LEN = 0x08;
	    const uint8_t MULTI_WRITE = 0x10;					//Код функции записи нескольких регистров modbus
	    const uint8_t MULTI_WRITE_LEN = 0x0D;

	    const uint8_t CONTROL_REG[2] = {0X20, 0X31};
	    const uint8_t ENABLE[2] = {0x00, 0X08};
	    const uint8_t DISABLE[2] = {0x00, 0X07};
	    const uint8_t OPERATING_MODE[2] = {0X20, 0X32};
	    const uint8_t VEL_MODE[2] = {0x00, 0X03};
	    const uint8_t SET_RPM[2] = {0x20, 0X3A};
	    const uint8_t GET_RPM[2] = {0x20, 0X2C};
	    const uint8_t SET_ACC_TIME[2] = {0x20, 0X81};
	    const uint8_t SET_DECC_TIME[2] = {0x20, 0X83};
	    const uint8_t SET_KP[2] = {0x20, 0X1D};
	    const uint8_t SET_KI[2] = {0x20, 0X1E};
	    const uint8_t INITIAL_SPEED[2] = {0X20, 0X08};
	    const uint8_t MAX_SPEED[2] = {0X20, 0X0A};
	    const uint8_t ACTUAL_POSITION_H[2] = {0X20, 0X2A};
	    const uint8_t ACTUAL_POSITION_L[2] = {0X20, 0X2B};

	    /**
	     * @brief calculates the crc and stores it in the hex_cmd array, so there is no return value
	     */
	    void calculate_crc(uint16_t len);

	    /**
	     * @brief reads from the serial port and saves the string into the receive_hex array
	     * @param num_bytes how many bytes to read from the buffer
	     * @return return 0 when OK, 1 if crc error
	     */
	    uint8_t read_hex(uint8_t num_bytes);

	    /**
	     * @brief print the hex command for debugging
	     */
	    void print_hex_cmd() const;

	    /**
	     * @brief print received hex for debugging
	     */
	    void print_rec_hex() const;

};



constexpr unsigned short crc16_table[] = {
    0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
    0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
    0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
    0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
    0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
    0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
    0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
    0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
    0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
    0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
    0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
    0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
    0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
    0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
    0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
    0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
    0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
    0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
    0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
    0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
    0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
    0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
    0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
    0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
    0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
    0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
    0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
    0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
    0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
    0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
    0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
    0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040};

static inline unsigned short crc16(const unsigned char *data, unsigned short len)
{
    unsigned char nTemp;
    unsigned short wCRCWord = 0xFFFF;

    while (len--)
    {
        nTemp = *data++ ^ wCRCWord;
        wCRCWord >>= 8;
        wCRCWord ^= crc16_table[nTemp];
    }
    return wCRCWord;
}

#endif /*__ZLAC8015D_H__*/
