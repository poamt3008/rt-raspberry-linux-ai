/* drivers/gpio.h
 *
 * Fully encapsulates libgpiod 2.2. No other module in the project
 * should include <gpiod.h> directly.
 */
#ifndef GPIO_H
#define GPIO_H

#include <stdbool.h>

typedef enum {
    GPIO_DIR_INPUT,
    GPIO_DIR_OUTPUT
} gpio_direction_t;

/* Opaque handle: the caller never sees or manipulates libgpiod structs. */
typedef struct gpio_handle gpio_handle_t;

/**
 * gpio_init - Reserve and configure a GPIO line.
 *
 * @chip_path:      path to the chip, e.g. "/dev/gpiochip0" 
 * @line_offset:    line number within the chip
 * @direction:      GPIO_DIR_INPUT or GPIO_DIR_OUTPUT
 * @initial_value:  initial value if direction == GPIO_DIR_OUTPUT (0 or 1)
 * @consumer:       identifying name for lsof/gpioinfo (e.g. "led_home")
 *
 * Return: pointer to gpio_handle_t on success, NULL on error (check stderr/perror).
 */
gpio_handle_t *gpio_init(const char *chip_path,
                          unsigned int line_offset,
                          gpio_direction_t direction,
                          int initial_value,
                          const char *consumer);

/* The following will be implemented in the next steps */
int  gpio_set(gpio_handle_t *h, int value);
int  gpio_get(gpio_handle_t *h);
int  gpio_toggle(gpio_handle_t *h);

/**
 * gpio_close - Release a GPIO line and its associated resources.
 *
 * @h: handle returned by gpio_init(). Safe to call with NULL (no-op).
 *
 * After this call, @h must not be used again (it is freed internally).
 */
void gpio_close(gpio_handle_t *h);

#endif /* GPIO_H */