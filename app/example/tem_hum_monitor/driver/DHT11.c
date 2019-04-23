#include "DHT11.h"

#include "gpio.h"
#include "espressif/esp_common.h"
#include "aos/kernel.h"
//#include "osal.h"

uint32_t humidity;
uint32_t temperature;

#define HIGH (1)
#define LOW (0) 
static void delay_ms();
static long long aos_now_us();

int32_t DHT11_read()
{
    uint8_t bits[5];
    uint8_t cnt = 7;
    uint8_t idx = 0;
    uint32_t loopCnt;
    int i;
    uint32_t ret=0;
    for( i = 0 ;i< 5 ;i++) bits[i] = 0;
    //ETS_INTR_LOCK();
    //krhino_intrpt_enter();
   // OS_EnterCritical();
    GPIO_OUTPUT_SET(4,HIGH);
    //  ret = GPIO_INPUT_GET(4);
    // printf("DHT11: %d\r\n",ret);
    // GPIO_OUTPUT_SET(4,LOW);
    // ret = GPIO_INPUT_GET(4);
    // printf("DHT11: %d\r\n",ret);
     delay_ms(20);

    GPIO_OUTPUT_SET(4,LOW);
     delay_ms(18);
    GPIO_OUTPUT_SET(4,HIGH);
    os_delay_us(40);
    GPIO_DIS_OUTPUT(4);
    os_delay_us(40);

    loopCnt = 10000;
    while(GPIO_INPUT_GET(4) == LOW){
        if(loopCnt-- == 0){
            printf("timeout %s",__FUNCTION__);
            return DHTLIB_ERROR_TIMEOUT;
        } 
        
    }
    loopCnt = 10000;
    while(GPIO_INPUT_GET(4) == HIGH){
       if(loopCnt-- == 0){
            printf("timeout %s %d\r\n",__FUNCTION__,__LINE__);
            return DHTLIB_ERROR_TIMEOUT;
        } 
    }
    for( i= 0;i < 40 ; i++){
        loopCnt = 10000;
		while(GPIO_INPUT_GET(4) == LOW){
            if(loopCnt-- == 0){
            printf("timeout %s %d",__FUNCTION__,__LINE__);
            return DHTLIB_ERROR_TIMEOUT;
            } 
        }	
        uint32_t t = system_get_time();
        loopCnt = 10000;
		while(GPIO_INPUT_GET(4) == HIGH){
             if(loopCnt-- == 0){
            printf("timeout %s %d",__FUNCTION__,__LINE__);
            return DHTLIB_ERROR_TIMEOUT;
            } 
        }
        
        if (((system_get_time() - t)) > 40) bits[idx] |= (1 << cnt); 
        if (cnt == 0)
        {
			cnt = 7;    // restart at MSB
			idx++;      // next byte!
		}
		else cnt--;
    }
   //OS_ExitCritical();
    //krhino_intrpt_exit();
    //ETS_INTR_LOCK();
    printf("[humidity] Integer:%d Decimal:%d\r\n [temperature] Integer:%d Decimal:%d\r\n",bits[0],bits[1],bits[2],bits[3],bits[4]);
    humidity    = bits[0];
   
	temperature = bits[2]&0x7f;
    uint8_t sum = (bits[0] + bits[1]+bits[2]+bits[3])&0xff;  
    if (bits[4] != sum) return DHTLIB_ERROR_CHECKSUM;

	return DHTLIB_OK;
} 
void DHT11_GPIO_init()
{
    GPIO_ConfigTypeDef dht11_gpio_cnf;

    //gpio_intr_handler_register(key_gpio_isr, NULL);

    dht11_gpio_cnf.GPIO_Pin = DHT11_GPIO_PIN;
    dht11_gpio_cnf.GPIO_Mode = DHT11_GPIO_MODE;
    dht11_gpio_cnf.GPIO_Pullup = DHT11_GPIO_PULLUP;
    dht11_gpio_cnf.GPIO_IntrType = DHT11_GPIO_INTRTYPE;
    gpio_config(&dht11_gpio_cnf);

   // key_gpio_enable_isr();
}

static inline void delay_ms(uint32_t ms)
{
    os_delay_us(1000*ms);
}
static inline long long  aos_now_us()
{
    return aos_now_ms()*1000;
}