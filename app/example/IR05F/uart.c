/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 * 
 * 
 *   this is a uart sample from hal uart for esp8266, and the same as other chips;
 *   for esp8266 , when the port set is 1 ,then the uart1 ande uart2 is the same baud_rate , but the uart1（gpio2） is for log ;
 * 
 *   这sample是一个从hal层适配esp8266的，也许同样适配其他芯片
 *   对于esp8266，如果设置串口为1，则串口一和串口零都是这个波特率，如果需要不一样，请自行修改driver ，注意串口一（gpio2）是作为日志打印。
 * 
 *   Contributor:  https://github.com/xuhongv
 * 
 * 
 */


#include "aos/kernel.h"

//#include "hal/uart.h"

#include "driver/uart.h"
#include "aos/yloop.h"
#include "aos/hal/gpio.h"
#include <stdio.h>

gpio_dev_t  IR05F_gpio;
uart_dev_t uart;
#define HAL_WAIT_FOREVER 0xFFFFFFFFU
#define IR05_LOG(format,...) \
do{printf("\r\n%d[%s]"format"\r\n",__LINE__,__FUNCTION__,##__VA_ARGS__);}while(0);

char *key_char[3] = {"flag","on","off"};
enum key{
    on,
    off
};

uint32_t IR05F_Learn();
void IR05F_Send();
static void IR05F_key_process(input_event_t *eventinfo, void *priv_data)
{   
     if (eventinfo->type != EV_KEY) {
        return;
    }
    //LOG("awss config press %u\n", eventinfo->value);
    printf("awss config press %u\n", eventinfo->value);
    if (eventinfo->code == CODE_BOOT) {
        if (eventinfo->value == VALUE_KEY_CLICK) {
            IR05F_Send();
        } else if (eventinfo->value == VALUE_KEY_LTCLICK) {
            IR05F_Learn();
        }
    }
}
uint32_t IR05F_Learn(char * key)
{
    uint32_t ret;
    uint8_t receive_bytes[236];
    uint32_t  i,recv_size = 0;
    uint32_t value;
    printf("learn success recv_size: %d\r\n" ,recv_size);
    hal_gpio_output_low(&IR05F_gpio);
    hal_gpio_input_get(&IR05F_gpio,&value);
    printf("vlaue:%d\r\n",value);
    aos_msleep(500);
    hal_gpio_output_high(&IR05F_gpio);
    hal_gpio_input_get(&IR05F_gpio,&value);
    printf("vlaue:%d\r\n",value);
    while(1){
         ret = hal_uart_recv_II(&uart, &receive_bytes, 236, &recv_size, HAL_WAIT_FOREVER);
        if ((ret == 0)&&recv_size == 236){
            aos_kv_set(key,receive_bytes,recv_size,1);
            break;
        }
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
    aos_kv_get("switch",receive_bytes,&recv_size);
    printf("recv_size %d\r\n",recv_size);
    for(i=0;i<recv_size;i++)
    {
        printf("%d ",receive_bytes[i]);
    }
     hal_uart_send(&uart,receive_bytes,recv_size,1000); 
    
}
static void write_kv(void)
{
    char key[10]="test";
    uint32_t value[1]={1};
    int len = 1;
    printf (">>>>aos_kv_set:%d\r\n",aos_kv_set(key,value,len,1));
}
static void write_kv_2()
{
     char key[10]="test";
    char value[1]={0};
    int len = 1;
    printf (">>>>aos_kv_set2:%d\r\n",aos_kv_set(key,value,len,1));
}
static void del_kv (void)
{ 
    printf (">>>>aos_kv_del:%d\r\n",aos_kv_del("test"));
}
static void read_kv(void)
{
    char key[10] = {0};
    char value[10] = {0};
    int len = 1;
    int i =0;
    printf(">>>>>>>aos_kv_get:%d\r\n",aos_kv_get("test",value,&len));
    printf(">>>>>>value:");
    for(i = 0;i<len;i++)
    {
        printf("%d",value[i]);

    }
}
 uint8_t check_study()
 {
     IR05_LOG("check_study");
     uint8_t read_flag[1];
     uint8_t write_flag[1]={0};
     uint32_t get_size=1;
     uint32_t ret ;
     ret = aos_kv_get(key_char[0],read_flag,&get_size);
     printf("aos_kv_get: %d",ret);
     if( ret != 0)
     {
         aos_kv_set(key_char[0],write_flag,1,1);
     }else{
         if(read_flag[0] == 1) return 1;
         else return 0;
     }
     return 2;
 }
int application_start(int argc, char *argv[])
{

    IR05_LOG("uart sample application started...");
   printf("uart sample application started...");
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

    
    int32_t ret = -1;
    uint32_t i, recv_size = 0;
    aos_kv_init();
    IR05_LOG("uart sample application started...");
    if(1 == check_study()){
        IR05_LOG("alread study");
    }else if(0 == check_study()){
        IR05_LOG("learning....");
        write_flag[0]=1 & 0xff;
        aos_kv_set(key_char[0],write_flag,write_size);
        IR05_LOG("learning open switch");
        while(1){
            if(IR05F_Learn(key_char[1]) == 0)break;
        }
        IR05_LOG("learning close switch");
        while(1){
            if(IR05F_Learn(key_char[2]) == 0)break;
        }
    }else{
        IR05_LOG("error");
    }
    // while(1)
    // {
    //     write_kv();
    //     read_kv();
    //     del_kv();
    //     read_kv();
    //     write_kv_2();
    //      aos_msleep(200);
    //     read_kv();
    //     aos_msleep(200);
    // }
    // while (1)
    // {
    //     ret = hal_uart_recv_II(&uart, &receive_bytes, 236, &recv_size, HAL_WAIT_FOREVER);
    //     if ((ret == 0))
    //     {
    //         for (i = 0; i < recv_size; i++)
    //             printf("hal_uart_recv_II ch = %d ,recv_size= %d \n", receive_bytes[i], recv_size);
    //         // en:return by the way you came  ch: 原路返回数据
    //         hal_uart_send(&uart,receive_bytes,recv_size,1000); 

    //     }
    // }
    aos_register_event_filter(EV_KEY, IR05F_key_process, NULL);
    aos_loop_run();
    return 0;
}
