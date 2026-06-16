/*-------------------------------------------------------------------------------------------------------------------
--
-- Проект      : EUROBOT_2026
-- Модуль      : STM32H723
-- Автор       : Караваев К.А.
-- Компания    : МИФИ
-- Файлы       :
--
---------------------------------------------------------------------------------------------------------------------
--
-- Описание    : Программа для управления и контроля движения на базе STM32H723
--
---------------------------------------------------------------------------------------------------------------------
--
--V0.0.0 (23.04.2025)  - Копирован и видоизменен старый проект. (eurobot_2024_embedded). Содержит код для двигателей
--                       JGB37-3650 (блок "kinematics" для преобразования оборотов в минуту в скорости вращения и др.,
--                       блок "motors" для аппаратного управления моторами(выдача шим-импульсов для контроля скорости,
--                       направление вращения), блок "rpm" для подсчета номинальных оборотов в минуту(дополняет блок "motors"),
--                       "rate_lim" и "pid" (не используются), блок "odometry" для расчета движения (путь и др.) и
--                       блок "imu" для вычисления скоростей и угла поворота, подключенный через i2c.
--V1.0.0 (28.04.2025)  - произведен рефакторинг кода, добавлен reboot, если микро-рос клиент перестает отвечать на запросы
--                       агента (например, после воздействия статического электричества), добавлены топики для передачи
--                       позиции робота в блок "odometry" (odom/reboot) при старте и 2 варианта функции
--                       void odom_init_subscriber_callback(const void *msgin) обработки посылки с начальными координатами
--                       робота при старте (необходимо для корректного накопления значений пути в одометрии) (функции не
--                       тестировались). Закомментирован блок "imu".
--V2.0.0 (02.05.2025)  - стабильная версия. Функции передачи начального положения робота при старте и соответствующий
--                       топик закомментированы за ненадобностью (для рассчета маршрута движения на pi4 из блока одометрии
--                       используются только скорости, а не путь)
--V3.0.0 (23.10.2025)  - значительно изменена структура проекта. Добавлены директории с математическими функциями (Software)
--                       и функциями для управления периферией (Hardware)
--V4.1.0 (25.12.2026)  - добавлены сонары, ADC для измерения напряжения
--V4.1.1 (05.02.2026)  - изменена кинематика
--V4.2.1 (08.02.2026)  - добавлена motor_driver_ZLAC8015D (Hardware)
--V5.2.2 (14.02.2026)  - изменена кинематика, добавлена функция _write() для логирования посредством printf(), добавлены
--                       переменные для измерения временных интервалов при помощи DWT
--V6.3.2 (01.03.2026)  - Исправлена проблема перезагрузки МК, увеличено число попыток пинга агента, изменена функция
--                       check_microros_agent_reset_watchdog(void), удалены лишние комментарии, изменено имя ноды.
--V7.3.2 (13.03.2026)  - Исправлены названия функций FS->HS в usb_cdc_transport.c, удалена папка .git из
--                       micro_ros_stm32cubemx_utils
--V8.5.2 (28.03.2026)  - Добавлены функции для считывания данных с оптического датчика одометрии и управления сервоприводами ST3215
--                       Добавлена FreeRtos задача для управления сервоприводами ST3215 (медленный процесс с задержками), под это
--                       выделен отдельный файл ServoTask.cpp и .h
--V9.6.3 (16.04.2026)  - Изменен источник данных в odom - теперь не с колес, а с OTOS. Добавлена функция в библиотеку odom
--                       для расчета одометрии при помощи OTOS, изменена библиотека для управления сервоприводами ST3215 и
--                       значительно переписана библиотека управления датчиком оптической одометрии PAA5160 (OTOS)
--V10.6.3 (23.05.2026) - RELEASE!! Изменен тип сообщения odom на vector3_stamped, чтобы снизить объем передаваемых данных. С целью
--                       гарантированной доставки сообщений на сервоприводы изменены типы топиков с стандартных на
--                       настраиваемые. (sensor - best effort, т.к пропуски не критичны, cmd - reliable )
---------------------------------------------------------------------------------------------------------------------
*/

#include <rcl/error_handling.h>
#include <rcl/rcl.h>
#include <rclc/executor.h>
#include <rclc/rclc.h>
#include <rmw_microros/rmw_microros.h>
#include <std_msgs/msg/int32.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "cmsis_os.h"
#include "iwdg.h"
#include "adc.h"
#include "i2c.h"
#include "gpio.h"

#include "usbd_cdc_if.h"
#include "usbd_cdc.h"
#include "usart.h"

#include "kinematics.h"
#include "motor_jgb37_bl.h"
#include "servo_mg90s.h"
#include "odometry.h"
//#include "rpm.h"
//#include "pid.h"
//#include "rate_lim.h"
#include "tim.h"
//#include "SCServo.h"  //Теперь все, связанное с управлением сервами, размещено в ServoTask, а здесь только получаем команды
						//на сервы и пишем в глобальную переменную servo_data, которая используется в ServoTask


#include <nav_msgs/msg/odometry.h>
//#include <geometry_msgs/msg/quaternion.h>
#include <geometry_msgs/msg/twist.h>
#include <std_msgs/msg/u_int32.h>
#include <std_msgs/msg/u_int8.h>
#include <std_msgs/msg/int32.h>
#include <std_msgs/msg/bool.h>

#include <std_msgs/msg/u_int32_multi_array.h>
#include <std_msgs/msg/int32_multi_array.h>
#include <geometry_msgs/msg/vector3_stamped.h>


#define PUBLISH_DELAY_MS 				20

#define EXECUTE_ERROR_MSG_MAX_LEN 		200

#define ROS_DOMAIN_ID                   0

//#define VOLTAGE_CORRECT_MUX             1.023	//Отношение реального напряжения к измеренному АЦП

//#define RPM_DIV									//Делитель для вычисления оборотов мотора. Учитывает редукционное соотношение и кол-во импульсов на оборот

#define MAX_PING_NO_RESPONSE_NUM 		3

//#define EXECUTE_LOG_MSG_LEN 			200

//bool inv1;
//bool inv2;
//bool inv3;
//

//uint32_t servo_angle = 0;
//uint32_t debug = 0;


uint8_t servo_data;				//Глобальная переменная для обмена данными с ServoTask
bool servo_spin_flag = false;

bool start_flag = false;

uint32_t ping_no_response_num = 0;
uint32_t ping_errors[MAX_PING_NO_RESPONSE_NUM+1];
uint32_t ping_err_max = 0;

uint32_t init_error;
char init_error_msg[][100] = {
		"All initialized correct",
		"You have selected an uninitialized timer during servo initialization",
		"Incorrect PWM channel is set during servo initialization",
		"You have selected an uninitialized timer during motor initialization",
		"Incorrect PWM channel is set during motor initialization",
		"You have selected an uninitialized timer during sonar initialization",
		"Incorrect Capture channel is set during sonar initialization",
		"Incorrect Capture channel is set during sonar initialization"};

//uint32_t execute_error;
//char execute_error_msg[EXECUTE_ERROR_MSG_MAX_LEN];

#ifdef EXECUTE_LOG_MSG_LEN
char execute_log_msg[EXECUTE_LOG_MSG_LEN] = {0};
uint8_t temp_buf[EXECUTE_LOG_MSG_LEN] = {0};
void write_log(char * execute_log){
	/*Объявляем статическую переменную для подсчета оставшегося в кольцевом буфере места*/
	static uint32_t written_characters = 0;

    // Проверяем валидность указателя
    if (execute_log == NULL) return;

	/*Считаем оставшееся в кольцевом буфере место (за минусом единицы, так как snprintf добавляет еще NULL
	 *в конце строки, но возвращает число символов без учета NULL).
	 */
	uint32_t free_space = EXECUTE_LOG_MSG_LEN - written_characters - 1;
	/*По тем же причинам здесь прибавляем единицу*/
	written_characters = written_characters + snprintf(&execute_log_msg[written_characters], \
															free_space, "%s", execute_log);
	/*Если записано больше EXECUTE_LOG_MSG_LEN - 10 символов (то есть осталось свободного места меньше 10
	 *символов, то отправляем массив по UART, очищаем его и сбрасываем значение written_characters)*/
	if(written_characters > EXECUTE_LOG_MSG_LEN - 20){

		for(int i = 0; i < EXECUTE_LOG_MSG_LEN; i++){
			temp_buf[i] = (uint8_t)execute_log_msg[i];
			execute_log_msg[i] = 0;
		}
		HAL_UART_Transmit_IT(&huart7, temp_buf, written_characters);
		written_characters = 0;

	}
}
#else
/*Пустой макрос, который позволяет не объявлять #ifdef EXECUTE_LOG_MSG_LEN везде при вызове функции write_log.
 *Если не объявлено, то будет подставлен пустой макрос вместо функции => нет затрат на выполнение.
 *То есть подставляется либо функция, если нужен отладочный вывод, либо пустой макрос, если нет.
 */
#define write_log(char_execute_log){};
#endif


//#define USE_IMU

#ifndef RCCHECK
#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){rclErrorLoop();}}
#endif

#ifndef RCSOFTCHECK
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){state = AGENT_DISCONNECTED;}}
#endif

#define EXECUTE_EVERY_N_MS(MS, X)  do { \
  static volatile int64_t init = -1; \
  if (init == -1) { init = uxr_millis();} \
  if (uxr_millis() - init > MS) { X; init = uxr_millis();} \
} while (0)

extern "C" {
#include "optic_odom_PAA5160E1.h"
/*Задаем прототипы специальных функций microros. Делаем так, чтобы не создавать хэдеры для .c файлов, содержащих эти функции*/
bool cubemx_transport_open(struct uxrCustomTransport * transport);
bool cubemx_transport_close(struct uxrCustomTransport * transport);
size_t cubemx_transport_write(struct uxrCustomTransport* transport, const uint8_t * buf, size_t len, uint8_t * err);
size_t cubemx_transport_read(struct uxrCustomTransport* transport, uint8_t* buf, size_t len, int timeout, uint8_t* err);

void * microros_allocate(size_t size, void * state);
void microros_deallocate(void * pointer, void * state);
void * microros_reallocate(void * pointer, size_t size, void * state);
void * microros_zero_allocate(size_t number_of_elements, size_t size_of_element, void * state);

//Позволяет использовать printf с выводом в UART
//int __io_putchar(int ch)
//{
//  /* Place your implementation of fputc here */
//  /* e.g. write a character to the USART4 and Loop until the end of transmission */
//  HAL_UART_Transmit(&huart7, (uint8_t *)&ch, 1, 0xFFFF);
//
//  return ch;
//}

int _write(int fd, char * ptr, int len)
{
  HAL_UART_Transmit(&huart7, (uint8_t *) ptr, len, HAL_MAX_DELAY);
  return len;
}

//Функция-заглушка, чтобы избежать ошибки компиляции.
#include <sys/time.h>
int _gettimeofday( struct timeval *tv, void *tzvp )
{
    // you can add code here there many example in google search.
    return 0;  // return non-zero for error
} // end _gettimeofday()
}

extern USBD_HandleTypeDef hUsbDeviceHS;

rcl_timer_t timer;
rcl_node_t node;
rcl_allocator_t allocator;
rclc_support_t support;
rclc_executor_t executor;

uint32_t e_cntr;

						rcl_ret_t ret_debug;


unsigned long long time_offset = 0;
unsigned long prev_cmd_time = 0;
unsigned long prev_odom_update = 0;
unsigned long prev_rpm_update = 0;

bool sync_time_flag = false;

Odometry odometry;
//rpm current_rpm;
//imp_num current_imp_num;
//struct repeating_timer timer;


float vel_y;
float angular_vel;

//const uint LED_PIN = 25;
int cnt = 1;
bool flag = true;

#ifdef USE_IMU
MPU9250IMU imu;

rcl_publisher_t imu_publisher;
sensor_msgs__msg__Imu imu_msg;
#endif

//rcl_publisher_t odom_publisher;
//nav_msgs__msg__Odometry odom_msg;

rcl_publisher_t simple_odom_publisher;
geometry_msgs__msg__Vector3Stamped simple_odom_msg;

//rcl_publisher_t rpm_publisher;
std_msgs__msg__Int32MultiArray rpm_msgs;

rcl_subscription_t twist_subscriber;
geometry_msgs__msg__Twist twist_msg;

//rcl_publisher_t voltage_publisher;
//std_msgs__msg__UInt32 voltage_msg;

rcl_subscription_t servo_subscriber;
std_msgs__msg__UInt8  servo_msg;
//std_msgs__msg__Int32 servo_msg;

rcl_publisher_t start_publisher;
std_msgs__msg__Bool start_msg;

//Переехали в файл ServoTask
//HardwareSerial SerialServo(UART4, 1000000);//115200);
//SMS_STS st;

OTOS_Handle otos;
OTOS_Data OTOS_pose;
OTOS_Data OTOS_vel;
//OTOS_Data OTOS_acc;
//OTOS_Data OTOS_std_pose;
//OTOS_Data OTOS_std_vel;
//OTOS_Data OTOS_std_acc;

//PID motor1_pid(RPM_MIN, RPM_MAX, K_P, K_I, K_D);
//PID motor2_pid(RPM_MIN, RPM_MAX, K_P, K_I, K_D);

JGB37BL motor1;
JGB37BL motor2;
JGB37BL motor3;

Kinematics kinematics(
    Kinematics::OMNI,
    MOTOR_MAX_RPM,
    MAX_RPM_RATIO,
#ifdef BRUSH_MOTORS
    MOTOR_OPERATING_VOLTAGE,
    MOTOR_POWER_MAX_VOLTAGE,
#endif
#ifdef BRUSHLESS_MOTORS
    MAX_RPM,
#endif
    WHEEL_DIAMETER,
    LR_WHEELS_DISTANCE);

//Kinematics::velocities cmd_vel;
//Kinematics::velocities current_vel;    //Скорости, полученные из одометрии

enum states
{
  WAITING_AGENT,
  AGENT_AVAILABLE,
  AGENT_CONNECTED,
  AGENT_DISCONNECTED
} state;


uint32_t DWT_cntr;
uint32_t DWT_cntr1;
float DWT_duration_mks[20] = {0};
float DWT_tick_to_mks; // = 1000000.f / HAL_RCC_GetSysClockFreq();
/**
  * @brief  Initialize DWT to measure short time intervals (mks, ps) (https://stackoverflow.com/questions/36378280/stm32-how-to-enable-dwt-cycle-counter)
  * @param  none
  * @retval none
  */
void DWT_init(void){
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}


//Функция проверяет состояние агента и сбрасывает watchdog. Если при пинге 3 раз подряд агент не отвечает на пару запросов, значит соединение разорвано
rmw_ret_t check_microros_agent_reset_watchdog(void){
	rmw_ret_t ret = rmw_uros_ping_agent(100, 2);
	HAL_IWDG_Refresh(&hiwdg1);
	if(ret != RMW_RET_OK){
		ping_errors[ping_no_response_num] = ret;
		ping_no_response_num++;

		if(ping_err_max <  ping_no_response_num){
			ping_err_max = ping_no_response_num;
		}
//		printf("No response from agent N = %d, error = %d \n", ping_no_response, ret);

	}
	else{
		ping_no_response_num = 0;
	}
	if(ping_no_response_num == MAX_PING_NO_RESPONSE_NUM){
//		printf("ERROR!!!! Agent ping failed %d times with errors %d, %d, %d, %d, %d and period %d mks. RESTART \n", MAX_PING_NO_RESPONSE_NUM,
//		ping_errors[0], ping_errors[1], ping_errors[2], ping_errors[3], ping_errors[4], (uint32_t)DWT_duration_mks[0]	);
		return ret;
	}
	return RMW_RET_OK;
//	return rmw_uros_ping_agent(100, 1);
}



void blink(void) {
	HAL_GPIO_TogglePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
}



void blink_n_times(int times_per_second, uint32_t delay_before_next_call_ms){
	//Множитель 2 возникает из-за того, что для генерации одного блинка нужно вызвать HAL_GPIO_TogglePin дважды
	for(int i = 0; i < 2*times_per_second; i++){
		HAL_GPIO_TogglePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
		//Т.к задержка в мс, то должны поделить 1000 на 2*times_per_second
		HAL_Delay( (uint32_t)(500 / times_per_second));
	}
	HAL_Delay(delay_before_next_call_ms);
}

//void blink_cont(int delay_ms, int num){
//    while( (num --) > 0){
//    	HAL_GPIO_TogglePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
//        sleep_ms(delay_ms);
//    }
//}

void rclErrorLoop()
{
    while(true)
    {
    	//Мигаем 10 раз за секунду, затем ждем 2 секунды и повторяем
    	blink_n_times(10, 2000);
//        blink();
        printf("Error %d %s \n", init_error, init_error_msg[init_error]);
//        HAL_Delay(2000);
        write_log("err_loop\n");

    }
}

float get_dt_s(unsigned long *prev_time){
    unsigned long now = uxr_millis();
    float dt = (now - *prev_time) / 1000.0;
    *prev_time = now;
    return dt;
}

struct timespec getTime()
{
    struct timespec tp = {0};
    // add time difference between uC time and ROS time to
    // synchronize time with ROS
    unsigned long long now = uxr_millis() + time_offset;
    tp.tv_sec = now / 1000;
    tp.tv_nsec = (now % 1000) * 1000000;

    return tp;
}

void publishData(){

//	rcl_ret_t ret = rcl_publish(&voltage_publisher, &voltage_msg, NULL);
//	ret = rcl_publish(&rpm_publisher, &rpm_msgs, NULL);

//    odom_msg = odometry.getData();

    //синхронизируем время
    struct timespec time_stamp = getTime();

    simple_odom_msg.header.stamp.sec = time_stamp.tv_sec;
    simple_odom_msg.header.stamp.nanosec = time_stamp.tv_nsec;

//    odom_msg.pose.pose.position.x = OTOS_pose.x;
//    odom_msg.pose.pose.position.y = OTOS_pose.y;
//    odom_msg.pose.pose.position.z = 0.0;
//
//    odom_msg.twist.twist.linear.x = OTOS_vel.x;
//    odom_msg.twist.twist.linear.y = OTOS_vel.y;
//    odom_msg.twist.twist.angular.z = OTOS_vel.theta;

    rcl_publish(&simple_odom_publisher, &simple_odom_msg, NULL);

    rcl_publish(&start_publisher, &start_msg, NULL);

}

void moveBase(){


	if(HAL_GPIO_ReadPin(START_GPIO_Port, START_Pin) == GPIO_PIN_RESET  ){
		servo_spin_flag = false;
		motor1.stop();
		motor2.stop();
		motor3.stop();

		start_msg.data = false;

		__HAL_TIM_SET_COUNTER(&htim23, 0);
		//Отправляем нули в одометрию

	    if(OTOS_IsAlive(&otos) == HAL_OK){
	    //Если ОТОС подключен, то калибруем
	    OTOS_CalibrateIMU(&otos, 64, 1);
	    OTOS_ResetTracking(&otos);
	    }
	    simple_odom_msg.vector.x = 0;
	    simple_odom_msg.vector.y = 0;
	    simple_odom_msg.vector.z = 0;
		return;
	}

	start_msg.data = true;

	uint32_t delay100s = __HAL_TIM_GET_COUNTER(&htim23);
	if(delay100s > 100000000){
		servo_spin_flag = false;
		motor1.stop();
		motor2.stop();
		motor3.stop();

		start_msg.data = false;

		__HAL_TIM_SET_COUNTER(&htim23, 110000000);
		return;
	}


//	servo1.spin(servo_msg.data);

//	HAL_ADC_Start(&hadc2);

// get the required rpm for each motor based on required velocities, and base used
    Kinematics::rpm req_rpm = kinematics.getRPM(
    	twist_msg.linear.x,
    	twist_msg.linear.y,
    	twist_msg.angular.z
    );

//    float rpm_dt = get_dt_s(&prev_rpm_update);
    rpm_msgs.data.data[0] = motor1.get_rpm_it();
    rpm_msgs.data.data[1] = motor2.get_rpm_it();
    rpm_msgs.data.data[2] = motor3.get_rpm_it();


//ограничиваем скорость
//    req_rpm.motor1 = rate_lim.compute(req_rpm.motor1,dt);
//    req_rpm.motor2 = rate_lim.compute(req_rpm.motor2,dt);

    motor1.spin((int)req_rpm.motor1);  // обращаемся к элементу motor1
//    motor1.spin((int)debug);
    motor2.spin((int)req_rpm.motor2);
    motor3.spin((int)req_rpm.motor3);

    Kinematics::velocities current_vel = kinematics.getVelocities(
    //Учитываем направления вращения  колес.
    	rpm_msgs.data.data[0],
		rpm_msgs.data.data[1],
		rpm_msgs.data.data[2],
        0
    );

    if(OTOS_IsAlive(&otos) == HAL_OK){
	OTOS_ReadPose(&otos, &OTOS_pose);
	OTOS_ReadVelocity(&otos, &OTOS_vel);

	//	OTOS_ReadAcceleration(&otos, &OTOS_acc);
	//	OTOS_ReadStdDevPosition(&otos, &OTOS_std_pose);
	//	OTOS_ReadStdDevVelocity(&otos, &OTOS_std_vel);
	//	OTOS_ReadStdDevAcceleration(&otos, &OTOS_std_acc);
    }
    simple_odom_msg.vector.x = OTOS_vel.x;
    simple_odom_msg.vector.y = OTOS_vel.y;
    simple_odom_msg.vector.z = OTOS_pose.theta;


//TODO Можно попробовать перемещения брать с otos, а угол - с колес, или как-то дрейф за счет колесного угла убирать
//Одометрия с OTOS
//    float vel_dt = get_dt_s(&prev_odom_update);
//	odometry.update(
//	    vel_dt,
//		OTOS_vel.x,
//		OTOS_vel.y,
//		OTOS_vel.theta,
//		OTOS_pose.x,
//		OTOS_pose.y,
//		OTOS_pose.theta
//	);

//Одометрия с колес
//    float vel_dt = get_dt_s(&prev_odom_update);
//	odometry.update(
//	    vel_dt,
//		current_vel.linear_x,
//		current_vel.linear_y,
//		current_vel.angular_z
//	);

//	voltage_msg.data = (uint32_t) (HAL_ADC_GetValue(&hadc2) * VOLTAGE_CORRECT_MUX * 16500/4095.);

//-----------------FOR SERVO ST3215 TEST-----------------
//	servo_angle += 5;
//	st.WritePosEx(2, servo_angle, 1500, 50); // 1900 - mid, 1250 - open
//	if(servo_angle > 500){
//		servo_angle = 0;
//	}
//----------------END FOR SERVO ST3215 TEST--------------

}


void syncTime()
{
    // get the current time from the agent
    unsigned long now = uxr_millis();
    RCCHECK(rmw_uros_sync_session(10));
    unsigned long long ros_time_ms = rmw_uros_epoch_millis();
    // now we can find the difference between ROS time and uC time
    time_offset = ros_time_ms - now;
}






void timer_callback(rcl_timer_t *timer, int64_t last_call_time) {
	DWT_cntr = DWT->CYCCNT;
	moveBase();
    publishData();
    DWT_duration_mks[0] = (DWT->CYCCNT - DWT_cntr)*DWT_tick_to_mks;

    DWT_duration_mks[1] = (DWT->CYCCNT - DWT_cntr1)*DWT_tick_to_mks;
	DWT_cntr1 = DWT->CYCCNT;
}



void twist_subscriber_callback(const void *msgin) {
////    const geometry_msgs__msg__Twist *msg = (const geometry_msgs__msg__Twist *)msgin;
//
    prev_cmd_time = uxr_millis();
    blink();

}

void servo_subscriber_callback(const void *msgin) {
//    const std_msgs__msg__Int32 *msg = (const std_msgs__msg__Int32 *)msgin;
	servo_data = ((const std_msgs__msg__UInt8*)msgin)->data;
    prev_cmd_time = uxr_millis();
    servo_spin_flag = true;
//    blink();

    HAL_GPIO_TogglePin(GPIO_RES9_GPIO_Port, GPIO_RES9_Pin);

}




bool createEntities(){

	rmw_qos_profile_t qos_cmd = rmw_qos_profile_default;
	qos_cmd.history = RMW_QOS_POLICY_HISTORY_KEEP_ALL;
	qos_cmd.depth = 5;
	qos_cmd.reliability = RMW_QOS_POLICY_RELIABILITY_RELIABLE;
	qos_cmd.lifespan.sec = 3;
	qos_cmd.lifespan.nsec = 0;


	rmw_qos_profile_t qos_sensor = rmw_qos_profile_default;
	qos_sensor.history = RMW_QOS_POLICY_HISTORY_KEEP_LAST;
	qos_sensor.depth = 5;
	qos_sensor.reliability = RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT;
	qos_sensor.lifespan.sec = 1;
	qos_sensor.lifespan.nsec = 0;


    allocator = rcl_get_default_allocator();
    rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();

    //Создаем кастомный узел(node), в котором определен ros domain id
    // https://github.com/micro-ROS/micro-ROS-Agent/issues/49#issuecomment-769208313
    // https://github.com/micro-ROS/micro_ros_stm32cubemx_utils/issues/23

    RCSOFTCHECK(rcl_init_options_init(&init_options, allocator));

    RCSOFTCHECK(rcl_init_options_set_domain_id(&init_options, ROS_DOMAIN_ID ));

	// create init_options
	RCSOFTCHECK(rclc_support_init_with_options(&support, 0, NULL, &init_options, &allocator));

    		write_log("init_pico_node\t");
    RCSOFTCHECK(rclc_node_init_default(&node, "stm32h7_node_9_6_3_stbl", "", &support));

    RCCHECK(rclc_subscription_init(
        &twist_subscriber,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
        "twist_mux/cmd_vel",
		&qos_cmd ));

    		write_log("init_servo_spin\t");
    RCSOFTCHECK(rclc_subscription_init(
        &servo_subscriber,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt8),
        "/grip/request",
    	&qos_cmd ));

//    		write_log("init_voltage_data\t");
//    RCSOFTCHECK(rclc_publisher_init_default(
//    	&voltage_publisher,
//		&node,
//		ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt32),
//		"voltage"));

//    RCCHECK(rclc_publisher_init_default(
//        &rpm_publisher,
//        &node,
//        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32MultiArray),
//        "rpm/data"));


    RCCHECK(rclc_publisher_init(
        &simple_odom_publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Vector3Stamped),
        "odom/unfiltered",
		&qos_sensor ));

    RCSOFTCHECK(rclc_publisher_init(
        &start_publisher,
		&node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool),
        "/bt_trigger_topic",
		&qos_sensor ));

    		write_log("init_timer\t");
    RCSOFTCHECK(rclc_timer_init_default(
        &timer,
        &support,
        RCL_MS_TO_NS(PUBLISH_DELAY_MS),
        timer_callback));



    		write_log("init_executor\t");
    RCSOFTCHECK(rclc_executor_init(&executor, &support.context, 3, &allocator));

    		write_log("add_timer\t");
    RCSOFTCHECK(rclc_executor_add_timer(&executor, &timer));
    RCSOFTCHECK(rclc_executor_add_subscription(&executor, &twist_subscriber, &twist_msg, &twist_subscriber_callback, ON_NEW_DATA));
    RCSOFTCHECK(rclc_executor_add_subscription(&executor, &servo_subscriber, &servo_msg, &servo_subscriber_callback, ON_NEW_DATA));
    		write_log("add_servo_sub\n");

    syncTime();

    return true;
}

bool destroyEntities(){
    rmw_context_t * rmw_context = rcl_context_get_rmw_context(&support.context);
    (void) rmw_uros_set_context_entity_destroy_session_timeout(rmw_context, 0);
    rcl_ret_t ret;
//Приравниваем ret только для того, чтобы компилятор не ругался.
    ret = rcl_timer_fini(&timer);
    ret = rcl_node_fini(&node);
    rclc_support_fini(&support);
    rclc_executor_fini(&executor);

    ret = rcl_subscription_fini(&servo_subscriber,&node);
    ret = rcl_subscription_fini(&twist_subscriber,&node);
//    ret = rcl_publisher_fini(&rpm_publisher,&node);
    ret = rcl_publisher_fini(&simple_odom_publisher,&node);
//    ret = rcl_publisher_fini(&voltage_publisher,&node);
    ret = rcl_publisher_fini(&start_publisher,&node);

#ifdef USE_IMU
    ret = rcl_publisher_fini(&imu_publisher,&node);
#endif



    return true;
}


void MicroRosInit(void){

	rpm_msgs.data.capacity = 3;
	rpm_msgs.data.data = (int32_t*) malloc(rpm_msgs.data.capacity * sizeof(int32_t));
	rpm_msgs.data.size = 3;
	for(int i = 0; i < (int)rpm_msgs.data.capacity; i++){
		rpm_msgs.data.data[i] = 0;
	}

	rmw_uros_set_custom_transport(
	    true,
	    (void *) &hUsbDeviceHS,
	    cubemx_transport_open,
	    cubemx_transport_close,
	    cubemx_transport_write,
	    cubemx_transport_read);

    rcl_allocator_t freeRTOS_allocator = rcutils_get_zero_initialized_allocator();
    freeRTOS_allocator.allocate = microros_allocate;
    freeRTOS_allocator.deallocate = microros_deallocate;
    freeRTOS_allocator.reallocate = microros_reallocate;
    freeRTOS_allocator.zero_allocate =  microros_zero_allocate;

    if (!rcutils_set_default_allocator(&freeRTOS_allocator)) {
        printf("Error on default allocators (line %d)\n", __LINE__);
    }

    DWT_init();
    DWT_tick_to_mks = 1000000.f / HAL_RCC_GetSysClockFreq();


    motor1.motor_init(true, &htim12, TIM_CHANNEL_1, JGB37_DIR1_GPIO_Port, JGB37_DIR1_Pin);
    motor2.motor_init(true,  &htim12, TIM_CHANNEL_2, JGB37_DIR2_GPIO_Port, JGB37_DIR2_Pin);
    motor3.motor_init(true, &htim1, TIM_CHANNEL_1, JGB37_DIR3_GPIO_Port, JGB37_DIR3_Pin);

    motor1.encoder_init_it(336*2, 6, &htim8, TIM_CHANNEL_1);
    motor2.encoder_init_it(336*2, 6, &htim8, TIM_CHANNEL_2);
    motor3.encoder_init_it(336*2, 6, &htim8, TIM_CHANNEL_3);



    //Калибруем АЦП
//    HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);

//Тоже переехали в ServoTask
//	SerialServo.begin();
//	st.pSerial = &SerialServo;


    OTOS_Init(&otos, &hi2c4);
    if(OTOS_IsAlive(&otos) == HAL_OK){
    //Если ОТОС подключен, то калибруем
    OTOS_CalibrateIMU(&otos, 64, 1);
    OTOS_ResetTracking(&otos);
    }

    HAL_TIM_Base_Start(&htim23);

#ifdef USE_IMU
    bool imu_ok = imu.init();
    if(!imu_ok)
    {
        while(1)
        {
            blink();
            sleep_ms(100);
        }
    }
#endif


}




void MicroRosExecute(){

	switch (state)
    {
        case WAITING_AGENT:
            EXECUTE_EVERY_N_MS(500, state = (RMW_RET_OK == rmw_uros_ping_agent(100, 1)) ? AGENT_AVAILABLE : WAITING_AGENT;);
//            blink_n_times(3, 1000);
//            		write_log("wait_agent\n");
            HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_SET);
            motor1.stop();
            motor2.stop();
            motor3.stop();

            break;
        case AGENT_AVAILABLE:
            state = (true == createEntities()) ? AGENT_CONNECTED : WAITING_AGENT;
            		write_log("agent_avlbl\t");
            HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_RESET);
            if (state == WAITING_AGENT)
            {
                destroyEntities();
            }

            break;
        case AGENT_CONNECTED:
        	EXECUTE_EVERY_N_MS(1000, state = (RMW_RET_OK == check_microros_agent_reset_watchdog()) ? AGENT_CONNECTED : AGENT_DISCONNECTED;);

        	if (state == AGENT_CONNECTED)
            {

        			write_log("executor_spin\t");

//            		DWT_cntr = DWT->CYCCNT;
//            		printf("cntr = %d\n", cnt++);
//            		DWT_duration_mks[0] = (DWT->CYCCNT - DWT_cntr)*DWT_tick_to_mks;

            	ret_debug = rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
            	if(ret_debug != RCL_RET_OK){
            		write_log("error during execution\t");
            	}
            }
            break;
        case AGENT_DISCONNECTED:
        			write_log("destroy_ent\t");

            motor1.stop();
            motor2.stop();
            motor3.stop();
            destroyEntities();
            state = WAITING_AGENT;
            break;
        default:
            break;
    }
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){

	motor1.rpm_capture_it(htim);
	motor2.rpm_capture_it(htim);
	motor3.rpm_capture_it(htim);
}



