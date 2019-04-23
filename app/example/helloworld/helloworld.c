/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include <aos/kernel.h>
#include "driver/uart.h"

int application_start(int argc, char *argv[])
{
    int count = 0;
    uint32_t recv_size;
    uint16_t data[236];
    uart_config_t uartConfig;
    printf("%d %s/r/n",__LINE__,__FUNCTION__);
    uartConfig.baud_rate = 921600;
    uart_dev_t uart;
    uart.port = 1;
    uart.config = uartConfig;
    hal_uart_init(&uart);

    printf("nano entry here!\r\n");

    while(1) {
        //printf("hello world! count %d \r\n", count++);
       hal_uart_recv_II(&uart, data, 236,
                      &recv_size, 0);
        hal_uart_send(&uart,)
        aos_msleep(1000);
    };
}
