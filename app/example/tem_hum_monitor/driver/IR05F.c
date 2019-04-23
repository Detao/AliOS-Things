/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include "aos/kernel.h"

//#include "hal/uart.h"

#include "driver/uart.h"
#include "aos/yloop.h"
#include "aos/hal/gpio.h"
#include <stdio.h>
#include "ulog/ulog.h"

gpio_dev_t  IR05F_gpio;
uart_dev_t uart;
#define HAL_WAIT_FOREVER 0xFFFFFFFFU

#define IR05_LOG(format,...) \
do{printf("\r\n%d[%s]"format"\r\n",__LINE__,__FUNCTION__,##__VA_ARGS__);}while(0);


char *key_char[3] = {"learn_flag","air-condition_on","air-condition_off"};

enum key{
    FLAG,
    ON,
    OFF
};

static void remote_contorl_learn();
static uint32_t IR05F_Learn();

uint32_t IR05F_Learn(char * key)
{
    uint32_t ret;
    uint8_t receive_bytes[236];
    uint32_t  i,recv_size = 0;
    uint32_t value;
    uint64_t   elink_time = 0;
    uint64_t diff;
    uint8_t is_timeout;
   while(1){

        hal_gpio_output_low(&IR05F_gpio);
        hal_gpio_input_get(&IR05F_gpio,&value);
        printf("vlaue:%d\r\n",value);
        aos_msleep(500);
        hal_gpio_output_high(&IR05F_gpio);
        hal_gpio_input_get(&IR05F_gpio,&value);
        printf("vlaue:%d\r\n",value);
        elink_time = aos_now_ms();
        while(1){
            diff = aos_now_ms() - elink_time;
            if(diff > 20000){
                is_timeout = 1;
                break;
            }
             ret = hal_uart_recv_II(&uart, &receive_bytes, 236, &recv_size, HAL_WAIT_FOREVER);
            if ((ret == 0)&&(recv_size == 236)){
                is_timeout =0;
                aos_kv_set(key,receive_bytes,recv_size,1);
                break;
            }
        }
        if(!is_timeout) break;
   }
   
   printf("learn success recv_size: %d\r\n" ,recv_size);
    for(i=0;i<recv_size;i++)
    {
        printf("%d ",receive_bytes[i]);
    }
    return 0;
}
void IR05F_Send()
{
    printf("IR05F_Send\r\n");
    uint8_t receive_bytes[236];
    uint32_t  i,recv_size = 236;
    aos_kv_get(key_char[1],receive_bytes,&recv_size);
    printf("recv_size %d\r\n",recv_size);
    for(i=0;i<recv_size;i++)
    {
        printf("%d ",receive_bytes[i]);
    }
     hal_uart_send(&uart,receive_bytes,recv_size,1000); 
    
}
void delete_key_active()
{
    uint8_t write_flag[1];
    uint8_t write_size=1;
    printf("delete_key_active \n");
    write_flag[0] = 0 & 0xff;
    aos_kv_set(key_char[0],write_flag,write_size,1);
    //aos_kv_del(key_char[0]);
    aos_kv_del(key_char[1]);
    aos_kv_del(key_char[2]);
    HAL_Reboot();
}
 uint8_t check_study()
 {
     LOG("check_study");
     uint8_t read_flag[1];
     uint8_t write_flag[1]={0};
     uint32_t get_size=1;
     uint32_t ret ;
     ret = aos_kv_get(key_char[0],read_flag,&get_size);
     printf("aos_kv_get: ret = %d  value =%d",ret,read_flag[0]);
     if( ret != 0)
     {
         aos_kv_set(key_char[0],write_flag,1,1);
     }else{
         if(read_flag[0] == 1) return 1;
         else return 0;
     }
     return 2;
 }
int IR05F_init()
{
    uint8_t write_flag[1];
    uint8_t write_size=1;
    IR05F_gpio.port = 14;
    IR05F_gpio.config = OUTPUT_PUSH_PULL;
    hal_gpio_init(&IR05F_gpio);
    hal_gpio_output_high(&IR05F_gpio);

    uart_config_t uartConfig;
    uartConfig.baud_rate = 9600; 
    
    uart.port = 1; //for esp8266 , when the port set is 1 ,then the uart1 ande uart2 is the same baud_rate , but the uart1 is for log 
    uart.config = uartConfig;
    hal_uart_init(&uart);

    printf("IR05F_init\r\n");
    int32_t ret = -1;
    uint32_t i, recv_size = 0;
    aos_kv_init();
    LOG("uart sample application started...");
    if(1 == check_study()){
        printf("alread study");
    }else if(0 == check_study()){
        printf("learning....");
        write_flag[0]=1 & 0xff;
        aos_kv_set(key_char[0],write_flag,write_size);
        remote_contorl_learn();
    }else{
        printf("error");
    }
  
    //aos_register_event_filter(EV_KEY, IR05F_key_process, NULL);
}
static void remote_contorl_learn()
{
    printf("remote_contorl start learning\n");
    printf("please ensure: \n \
        Temperature: 16℃ \n \
        Workmode: Refrigeration \n \
        WindSpeed: top speed\n");
    printf("learning open PowerSwitch");
    IR05F_Learn(key_char[1]);
    printf("open PowerSwitch learning success\n");
    
    printf("learning close PowerSwitch");
    IR05F_Learn(key_char[2]);
    printf("close PowerSwitch learning success\n");
    
}