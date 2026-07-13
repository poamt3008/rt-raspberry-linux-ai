/******************************************************************************
 * File: task_network.c
 *
 * Periodic task that classifies the current Wi-Fi network as
 * HOME / PHONE / OTHER and blinks the corresponding LED(s):
 *   HOME  -> green LED blinks, red LED off
 *   PHONE -> red LED blinks, green LED off
 *   OTHER -> both LEDs blink
 *
 * Owns both GPIO handles: gpio_init()/gpio_close() are called here,
 * not in main(), per project convention (tasks own their drivers).
 ******************************************************************************/

/******************************************************************************
 * Librerias
 ******************************************************************************/
#include "task_network.h"
#include "network_info.h"
#include "gpio.h"
#include "config.h"
#include "time_utils.h"

#include <pthread.h>
#include <stdio.h>
#include <time.h>

#define GPIO_CHIP_PATH "/dev/gpiochip0"

/******************************************************************************
 * Task implementation
 ******************************************************************************/
void *task_network(void *arg)
{
    /**************************************************************************
     * Local variables
     **************************************************************************/
    struct timespec next_activation;
    gpio_handle_t *led_green;
    gpio_handle_t *led_red;
    int blink_phase = 0;   /* 0 = off, 1 = on -- software-driven, not read from hardware */
    (void)arg;

    /**************************************************************************
     * Initialization
     **************************************************************************/
    led_green = gpio_init(GPIO_CHIP_PATH, GPIO_LED_GREEN,
                           GPIO_DIR_OUTPUT, 0, "task_network_green");
    if (!led_green)
    {
        printf("[NETWORK] Failed to initialize green LED.\n");
        return NULL;
    }

    led_red = gpio_init(GPIO_CHIP_PATH, GPIO_LED_RED,
                         GPIO_DIR_OUTPUT, 0, "task_network_red");
    if (!led_red)
    {
        printf("[NETWORK] Failed to initialize red LED.\n");
        gpio_close(led_green);
        return NULL;
    }

    clock_gettime(CLOCK_MONOTONIC, &next_activation);

    printf("[NETWORK] Period = %d ms\n", NETWORK_PERIOD_MS);

    printf("[NETWORK] Task Started. \n");

    while(1)
    {
        timespec_add_ms(&next_activation,
                        NETWORK_PERIOD_MS);

        int ret = clock_nanosleep(
            CLOCK_MONOTONIC,
            TIMER_ABSTIME,
            &next_activation,
            NULL);

        if (ret != 0)
        {
            printf("[NETWORK] clock_nanosleep() failed.\n");
        }

        /*Tarea periodica*/
        network_status_t status = network_info_get_status();
        blink_phase = !blink_phase;

        switch (status)
        {
        case NETWORK_STATUS_HOME:
            gpio_set(led_green, blink_phase);
            gpio_set(led_red, 0);
            //printf("[NETWORK]  Home-Green \n");
            break;

        case NETWORK_STATUS_PHONE:
            gpio_set(led_green, 0);
            gpio_set(led_green, blink_phase);
            //printf("[NETWORK]  Phone-Red \n");
            break;

        case NETWORK_STATUS_OTHER:
        default:
            gpio_set(led_green, blink_phase);
            gpio_set(led_red, blink_phase);
            //printf("[NETWORK]  No conection_blink \n");
            break;
        }
    }

    /* Unreachable in current design (infinite loop) -- kept here as
     * documentation of the intended cleanup path if a stop condition
     * is added later. */
    gpio_close(led_green);
    gpio_close(led_red);

    return NULL;
}