/**
 * @file lv_port_disp.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include <stdbool.h>
#include <stdio.h>

#include "RTE_Components.h"
#include CMSIS_device_header
#include <RTE_Device.h>
#include "Driver_CDC200.h" // Display driver

#include "dave_cfg.h"
#include "dave_d0lib.h"
#include "dave_driver.h"
#include "lv_draw_dave2d_utils.h"

#define USE_EVT_GROUP

#define USE_VSYNC

#if (LV_USE_OS == LV_OS_FREERTOS) && defined(USE_EVT_GROUP)
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "event_groups.h"
#endif

/*********************
 *      DEFINES
 *********************/
#ifndef MY_DISP_HOR_RES
    // Replace the macro MY_DISP_HOR_RES with the actual screen width.
    #define MY_DISP_HOR_RES    (RTE_PANEL_HACTIVE_TIME)
#endif

#ifndef MY_DISP_VER_RES
    // Replace the macro MY_DISP_HOR_RES with the actual screen height.
    #define MY_DISP_VER_RES    (RTE_PANEL_VACTIVE_LINE)
#endif

#if ((LV_COLOR_DEPTH == 16) && (RTE_CDC200_PIXEL_FORMAT != 2)) || \
    ((LV_COLOR_DEPTH == 24) && (RTE_CDC200_PIXEL_FORMAT != 1)) || \
    ((LV_COLOR_DEPTH == 32) && (RTE_CDC200_PIXEL_FORMAT != 0))
#error "The LV_COLOR_DEPTH and RTE_CDC200_PIXEL_FORMAT must match."
#endif

#if (LV_USE_OS == LV_OS_FREERTOS) && defined(USE_EVT_GROUP)
#define EVENT_DISP_BUFFER_READY     ( 1 << 0 )
#define EVENT_DISP_BUFFER_CHANGED   ( 1 << 1 )
#endif

#if (D1_MEM_ALLOC == D1_MALLOC_D0LIB)
// D/AVE D0 heap address and size
#define D1_HEAP_SIZE	0x80000
#endif

/**********************
 *      TYPEDEFS
 **********************/

#pragma pack(1)
#if RTE_CDC200_PIXEL_FORMAT == 0    // ARGB8888
typedef lv_color32_t Pixel;
#elif RTE_CDC200_PIXEL_FORMAT == 1  // RGB888
typedef lv_color_t Pixel;
#elif RTE_CDC200_PIXEL_FORMAT == 2  // RGB565
typedef lv_color16_t Pixel;
#else
#error "CDC200 Unsupported color format"
#endif
#pragma pack()

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);

static void disp_flush_wait(lv_display_t * disp);

static void disp_callback(uint32_t event);

/**********************
 *  STATIC VARIABLES
 **********************/

#if (D1_MEM_ALLOC == D1_MALLOC_D0LIB)
static uint8_t __attribute__((section(".bss.at_sram1"))) d0_heap[D1_HEAP_SIZE];
#endif

static Pixel lcd_buffer_1[MY_DISP_VER_RES][MY_DISP_HOR_RES]
            __attribute__((section(".bss.lcd_frame_buf"))) = {0};
static Pixel lcd_buffer_2[MY_DISP_VER_RES][MY_DISP_HOR_RES]
            __attribute__((section(".bss.lcd_frame_buf"))) = {0};

extern ARM_DRIVER_CDC200 Driver_CDC200;
static ARM_DRIVER_CDC200 *CDCdrv = &Driver_CDC200;

static volatile bool disp_flush_enabled = true;

#if (LV_USE_OS == LV_OS_FREERTOS) && defined(USE_EVT_GROUP)
static EventGroupHandle_t dispEventGroupHandle = NULL;
#else
static volatile bool disp_buf_ready = false;
static volatile bool disp_buf_changed = false;
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    #if (LV_USE_OS == LV_OS_FREERTOS) && defined(USE_EVT_GROUP)
    // Create event group to sync display states
    dispEventGroupHandle = xEventGroupCreate();
    #endif

    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*-------------------------
     * Set display interrupt priority to FreeRTOS kernel level
     * -----------------------*/
    NVIC_SetPriority(CDC_SCANLINE0_IRQ_IRQn, configKERNEL_INTERRUPT_PRIORITY);

#if (D1_MEM_ALLOC == D1_MALLOC_D0LIB)
    /*-------------------------
     * Initialize D/AVE D0 heap
     * -----------------------*/
    if (!d0_initheapmanager(d0_heap, sizeof(d0_heap), d0_mm_fixed_range,
                            NULL, 0, 0, 0, d0_ma_unified))
    {
        printf("\r\nError: Heap manager initialization failed\n");
        return;
    }
#endif

    lv_init();

    /*------------------------------------
     * Create a display and set a flush_cb
     * -----------------------------------*/
    lv_display_t * disp = lv_display_create(MY_DISP_HOR_RES, MY_DISP_VER_RES);
    lv_display_set_flush_cb(disp, disp_flush);
    lv_display_set_flush_wait_cb(disp, disp_flush_wait);

    /* Two buffers screen sized buffer for double buffering.
     * Both LV_DISPLAY_RENDER_MODE_DIRECT and LV_DISPLAY_RENDER_MODE_FULL works, see their comments*/
    lv_display_set_buffers(disp, lcd_buffer_1, lcd_buffer_2,
                           sizeof(lcd_buffer_1),
                           LV_DISPLAY_RENDER_MODE_DIRECT);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    /* Initialize CDC driver */
    int ret = CDCdrv->Initialize(disp_callback);
    if(ret != ARM_DRIVER_OK){
        printf("\r\n Error: CDC init failed\n");
        __BKPT(0);
        return;
    }

    /* Power control CDC */
    ret = CDCdrv->PowerControl(ARM_POWER_FULL);
    if(ret != ARM_DRIVER_OK){
        printf("\r\n Error: CDC Power up failed\n");
        __BKPT(0);
        return;
    }

    /* configure CDC controller */
    ret = CDCdrv->Control(CDC200_CONFIGURE_DISPLAY, (uint32_t)lcd_buffer_1);
    if(ret != ARM_DRIVER_OK){
        printf("\r\n Error: CDC controller configuration failed\n");
        __BKPT(0);
        return;
    }

    /* Enable CDC SCANLINE0 event */
    ret = CDCdrv->Control(CDC200_SCANLINE0_EVENT, ENABLE);
    if(ret != ARM_DRIVER_OK){
        printf("\r\n Error: CDC200_SCANLINE0_EVENT enable failed\n");
        __BKPT(0);
        return;
    }

    /* Start CDC */
    ret = CDCdrv->Start();
    if(ret != ARM_DRIVER_OK){
        printf("\r\n Error: CDC Start failed\n");
        __BKPT(0);
        return;
    }
}

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

/*Flush the content of the internal buffer the specific area on the display.
 *`px_map` contains the rendered image as raw pixel map and it should be copied to `area` on the display.
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_display_flush_ready()' has to be called when it's finished.*/
static void disp_flush(lv_display_t * disp_drv, const lv_area_t * area, uint8_t * px_map)
{
    if (disp_flush_enabled && lv_disp_flush_is_last(disp_drv)) {
        d2_finish_rendering();

        //uint32_t size = lv_area_get_width(area) * lv_area_get_height(area)
        //                * lv_color_format_get_size(lv_display_get_color_format(disp_drv));
        //SCB_CleanInvalidateDCache_by_Addr(px_map, size);

#ifdef USE_VSYNC
        #if (LV_USE_OS == LV_OS_FREERTOS) && defined(USE_EVT_GROUP)
        xEventGroupClearBits(dispEventGroupHandle, EVENT_DISP_BUFFER_READY);
        #else
        disp_buf_ready = false;
        #endif

        CDCdrv->Control(CDC200_FRAMEBUF_UPDATE_VSYNC, (uint32_t)px_map);

        #if (LV_USE_OS == LV_OS_FREERTOS) && defined(USE_EVT_GROUP)
        xEventGroupSetBits(dispEventGroupHandle, EVENT_DISP_BUFFER_CHANGED);
        #else
        disp_buf_changed = true;
        #endif
#else
        CDCdrv->Control(CDC200_FRAMEBUF_UPDATE, (uint32_t)px_map);
#endif
    }
}

/* Display buffer flush waiting callback
 */
static void disp_flush_wait(lv_display_t * disp)
{
#ifdef USE_VSYNC
    #if (LV_USE_OS == LV_OS_FREERTOS) && defined(USE_EVT_GROUP)
    xEventGroupWaitBits(dispEventGroupHandle, EVENT_DISP_BUFFER_READY,
                        pdFALSE, pdFALSE, (100/portTICK_PERIOD_MS));
    #else
    while (!disp_buf_ready)
        sys_busy_loop_us(33);
    #endif
#endif
}

/* Display event handler
 */
static void disp_callback(uint32_t event)
{
    if (event & ARM_CDC_SCANLINE0_EVENT) {
        #if (LV_USE_OS == LV_OS_FREERTOS) && defined(USE_EVT_GROUP)
        if (xEventGroupGetBitsFromISR(dispEventGroupHandle)
           & EVENT_DISP_BUFFER_CHANGED) {
            xEventGroupClearBitsFromISR(
                dispEventGroupHandle,
                EVENT_DISP_BUFFER_CHANGED);

            BaseType_t context_switch = pdFALSE;
            xEventGroupSetBitsFromISR(
                dispEventGroupHandle,
                EVENT_DISP_BUFFER_READY,
                &context_switch);
            portYIELD_FROM_ISR(context_switch);
        }
        #else
        if (disp_buf_changed) {
            disp_buf_changed = false;
            disp_buf_ready = true;
        }
        #endif
    }

    if (event & ARM_CDC_DSI_ERROR_EVENT) {
        // Transfer Error: Received Hardware error.
        __BKPT(0);
    }
}

