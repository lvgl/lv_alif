/* Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/******************************************************************************
 * @file     cpu_usage.h
 * @brief    CPU usage calculation hooks 
 *
 ******************************************************************************/
#ifndef CPU_USAGE_H
#define CPU_USAGE_H

void freertos_task_switch_in(const char *name);
void freertos_task_switch_out();
uint32_t freertos_get_idle_percent(void);

#endif // CPU_USAGE_H
