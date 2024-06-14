/* Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
#include "RTE_Components.h"
#include CMSIS_device_header

#include "Driver_GPIO.h"
#include "board.h"
#include "power.h"
#include "se_services_port.h"

#include "lv_port_disp.h"
#include "demos/benchmark/lv_demo_benchmark.h"

// Check if UART trace is disabled
#if !defined(DISABLE_UART_TRACE)
#include <stdio.h>
#include "uart_tracelib.h"
#else
#define printf(fmt, ...) (0)
#endif

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "timers.h"

// LVGL tick timer period
const uint32_t LVGL_TICK_TIME_MS = 1;
// LVGL tick timer handle
TimerHandle_t lvgl_tick_timer_handle = NULL;
// LVGL task handle
TaskHandle_t lvgl_task_handle = NULL;

void clock_init()
{
    uint32_t service_error_code = 0;
    /* Enable Clocks */
    uint32_t error_code = SERVICES_clocks_enable_clock(se_services_s_handle, CLKEN_CLK_100M, true, &service_error_code);
    if(error_code || service_error_code){
        printf("SE: 100MHz clock enable error_code=%u se_error_code=%u\n", error_code, service_error_code);
        return;
    }

    error_code = SERVICES_clocks_enable_clock(se_services_s_handle, CLKEN_HFOSC, true, &service_error_code);
    if(error_code || service_error_code){
        printf("SE: HFOSC enable error_code=%u se_error_code=%u\n", error_code, service_error_code);
        return;
    }
}

void lvgl_tick_timer_callback(TimerHandle_t xTimer)
{
    (void) xTimer;

    lv_tick_inc(LVGL_TICK_TIME_MS);
}

void lvgl_thread(void *pvParam)
{
    (void) pvParam;

    // Enable MIPI power
    enable_mipi_dphy_power();
    disable_mipi_dphy_isolation();

    // Initialize the SE services
    se_services_port_init();
    clock_init();

    // Initialize display
    lv_port_disp_init();
    // Launch LVGL benchmark
    lv_demo_benchmark();

    while (true) {
        uint32_t ms_till_next_run = lv_timer_handler();
        const TickType_t xDelay = ms_till_next_run/portTICK_PERIOD_MS;
        vTaskDelay(xDelay);
        BOARD_LED1_Control(BOARD_LED_STATE_TOGGLE);
    }
}

void main (void)
{
    // System Initialization
    SystemCoreClockUpdate();
    BOARD_Pinmux_Init();
#if !defined(DISABLE_UART_TRACE)
    tracelib_init(NULL, NULL);
#endif

    // Create application main thread
    if (xTaskCreate(lvgl_thread, "LVGL", 2048, NULL,
                    tskIDLE_PRIORITY+1, &lvgl_task_handle) != pdPASS) {
        vTaskDelete(lvgl_task_handle);
        __BKPT();
    }

    // Create LVGL tick timer
    lvgl_tick_timer_handle = xTimerCreate("lvgl_tick",
                            LVGL_TICK_TIME_MS/portTICK_PERIOD_MS,
                            pdTRUE, NULL,
                            lvgl_tick_timer_callback);
    if (lvgl_tick_timer_handle == NULL) {
        __BKPT();
    }
    xTimerStart(lvgl_tick_timer_handle, 0);

    // Start thread execution
    vTaskStartScheduler();
}
