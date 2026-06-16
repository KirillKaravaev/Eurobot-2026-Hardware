#include "HardwareSerial.h"
#include <cstring>  // ????????? ??? strlen

// ?????????? Error_Handler, ???? ?? ?? ????????? ? main
extern "C" {
    void Error_Handler(void);
}

// ???? Error_Handler ?? ?????????, ??????? ??????? ??????????
__weak void Error_Handler(void)
{
    while(1) {
        // ???????? ? ??????????? ????? ??? ??????
    }
}

HardwareSerial::HardwareSerial(USART_TypeDef *uartInstance, uint32_t baudrate) 
    : uartInstance(uartInstance), baudrate(baudrate), txPin(0), rxPin(0) {
}

HardwareSerial::HardwareSerial(USART_TypeDef *uartInstance, uint32_t baudrate, uint32_t tx_pin, uint32_t rx_pin) 
    : uartInstance(uartInstance), baudrate(baudrate), txPin(tx_pin), rxPin(rx_pin) {
}

void HardwareSerial::begin() {
    if (initialized) return;
    
    huart.Instance = uartInstance;
    huart.Init.BaudRate = baudrate;
    huart.Init.WordLength = UART_WORDLENGTH_8B;
    huart.Init.StopBits = UART_STOPBITS_1;
    huart.Init.Parity = UART_PARITY_NONE;
    huart.Init.Mode = UART_MODE_TX_RX;
    huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart.Init.OverSampling = UART_OVERSAMPLING_16;
    huart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    
    if (HAL_UART_Init(&huart) != HAL_OK) {
        Error_Handler();
    }
    
    initialized = true;
}

int HardwareSerial::available() {
    if (!initialized) return 0;
    return (__HAL_UART_GET_FLAG(&huart, UART_FLAG_RXNE) != RESET) ? 1 : 0;
}

int HardwareSerial::read() {
    if (!initialized) return -1;
    
    if (available()) {
        uint8_t data;
        if (HAL_UART_Receive(&huart, &data, 1, 0) == HAL_OK) {
            return data;
        }
    }
    return -1;
}

int HardwareSerial::write(const uint8_t *data, size_t len) {
    if (!initialized) return 0;
    
    if (HAL_UART_Transmit(&huart, const_cast<uint8_t*>(data), len, HAL_MAX_DELAY) == HAL_OK) {
        return len;
    }
    return 0;
}

size_t HardwareSerial::write(uint8_t data) {
    return write(&data, 1);
}

size_t HardwareSerial::write(const char *str) {
    return write(reinterpret_cast<const uint8_t*>(str), strlen(str));
}

HardwareSerial::~HardwareSerial() {
    if (initialized) {
        HAL_UART_DeInit(&huart);
    }
}

void HardwareSerial::initGPIO() {
    // ?????????????? ????????? GPIO ???? ?????
}