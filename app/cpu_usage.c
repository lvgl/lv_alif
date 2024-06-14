/* Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include <stdbool.h>
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

static volatile uint32_t idle_time_sum = 0;
static volatile uint32_t non_idle_time_sum = 0;
static volatile uint32_t task_switch_timestamp = 0;
static volatile bool idle_task_running;

void freertos_task_switch_in(const char *name)
{
    if(strcmp(name, "IDLE") == 0)
        idle_task_running = true;
    else
        idle_task_running = false;

    task_switch_timestamp = xTaskGetTickCount();
}

void freertos_task_switch_out()
{
    uint32_t now = xTaskGetTickCount();
    int32_t elapsed = now - task_switch_timestamp;
    if(idle_task_running)
        idle_time_sum += elapsed;
    else
        non_idle_time_sum += elapsed;
}

uint32_t freertos_get_idle_percent(void)
{
    if(non_idle_time_sum + idle_time_sum == 0) {
        // Not enough time elapsed to provide idle percentage
        return 0;
    }

    uint32_t pct = (idle_time_sum * 100) / (idle_time_sum + non_idle_time_sum);

    non_idle_time_sum = 0;
    idle_time_sum = 0;
    return pct;
}

#if configUSE_IDLE_HOOK
void vApplicationIdleHook(void)
{
   for (;;) {
        __WFI();
   }
}
#endif
