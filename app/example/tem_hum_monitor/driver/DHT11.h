#ifndef _DHT11_H_
#define _DHT11_H_

#include "aos/kernel.h"
#include "gpio.h"


enum ret_stauts{
    DHTLIB_OK,
    DHTLIB_ERROR_TIMEOUT,
    DHTLIB_ERROR_CHECKSUM
};

#define DHT11_GPIO_PIN GPIO_Pin_4
#define DHT11_GPIO_MODE GPIO_Mode_Output
#define DHT11_GPIO_PULLUP GPIO_PullUp_EN
#define DHT11_GPIO_INTRTYPE GPIO_PIN_INTR_DISABLE


extern uint32_t humidity;
extern uint32_t temperature;

void DHT11_GPIO_init();
int32_t DHT11_read();


#endif/*_DHT11_H_*/