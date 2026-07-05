/* drivers/gpio.c */
#include "gpio.h"
#include <gpiod.h>
#include <stdio.h>
#include <stdlib.h>

struct gpio_handle {
    struct gpiod_chip         *chip;
    struct gpiod_line_request *request;
    unsigned int               offset;
};

gpio_handle_t *gpio_init(const char *chip_path,
                          unsigned int line_offset,
                          gpio_direction_t direction,
                          int initial_value,
                          const char *consumer)
{
    struct gpiod_chip *chip = NULL;
    struct gpiod_line_settings *settings = NULL;
    struct gpiod_line_config *line_cfg = NULL;
    struct gpiod_request_config *req_cfg = NULL;
    struct gpiod_line_request *request = NULL;
    gpio_handle_t *handle = NULL;

    chip = gpiod_chip_open(chip_path);
    if (!chip) {
        perror("gpio_init: gpiod_chip_open");
        return NULL;
    }

    settings = gpiod_line_settings_new();
    if (!settings) {
        perror("gpio_init: gpiod_line_settings_new");
        goto err_close_chip;
    }

    gpiod_line_settings_set_direction(
        settings,
        direction == GPIO_DIR_OUTPUT ? GPIOD_LINE_DIRECTION_OUTPUT
                                     : GPIOD_LINE_DIRECTION_INPUT);

    if (direction == GPIO_DIR_OUTPUT) {
        gpiod_line_settings_set_output_value(
            settings,
            initial_value ? GPIOD_LINE_VALUE_ACTIVE
                          : GPIOD_LINE_VALUE_INACTIVE);
    }

    line_cfg = gpiod_line_config_new();
    if (!line_cfg) {
        perror("gpio_init: gpiod_line_config_new");
        goto err_free_settings;
    }

    if (gpiod_line_config_add_line_settings(line_cfg, &line_offset, 1, settings) != 0) {
        perror("gpio_init: gpiod_line_config_add_line_settings");
        goto err_free_line_cfg;
    }

    if (consumer) {
        req_cfg = gpiod_request_config_new();
        if (!req_cfg) {
            perror("gpio_init: gpiod_request_config_new");
            goto err_free_line_cfg;
        }
        gpiod_request_config_set_consumer(req_cfg, consumer);
    }

    request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);
    if (!request) {
        perror("gpio_init: gpiod_chip_request_lines");
        goto err_free_req_cfg;
    }

    handle = calloc(1, sizeof(*handle));
    if (!handle) {
        perror("gpio_init: calloc");
        goto err_release_request;
    }

    handle->chip    = chip;
    handle->request = request;
    handle->offset  = line_offset;

    /* Free intermediate configuration objects; no longer needed once
     * the request has been created. The chip and request ARE kept
     * (they are released in gpio_close). */
    gpiod_request_config_free(req_cfg);
    gpiod_line_config_free(line_cfg);
    gpiod_line_settings_free(settings);

    return handle;

err_release_request:
    gpiod_line_request_release(request);
err_free_req_cfg:
    gpiod_request_config_free(req_cfg);
err_free_line_cfg:
    gpiod_line_config_free(line_cfg);
err_free_settings:
    gpiod_line_settings_free(settings);
err_close_chip:
    gpiod_chip_close(chip);
    return NULL;
}

int gpio_set(gpio_handle_t *h, int value)
{
    int ret;

    if (!h) {
        fprintf(stderr, "gpio_set: NULL handle\n");
        return -1;
    }

    ret = gpiod_line_request_set_value(
        h->request,
        h->offset,
        value ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE);

    if (ret != 0) {
        perror("gpio_set: gpiod_line_request_set_value");
        return -1;
    }

    return 0;
}
int gpio_get(gpio_handle_t *h)
{
    enum gpiod_line_value value;

    if (!h) {
        fprintf(stderr, "gpio_get: NULL handle\n");
        return -1;
    }

    value = gpiod_line_request_get_value(h->request, h->offset);

    if (value == GPIOD_LINE_VALUE_ERROR) {
        perror("gpio_get: gpiod_line_request_get_value");
        return -1;
    }

    return (value == GPIOD_LINE_VALUE_ACTIVE) ? 1 : 0;
}

int gpio_toggle(gpio_handle_t *h)
{
    int current;

    if (!h) {
        fprintf(stderr, "gpio_toggle: NULL handle\n");
        return -1;
    }

    current = gpio_get(h);
    if (current < 0) {
        /* gpio_get already printed the error */
        return -1;
    }

    return gpio_set(h, !current);
}

void gpio_close(gpio_handle_t *h)
{
    if (!h) {
        return;
    }

    gpiod_line_request_release(h->request);
    gpiod_chip_close(h->chip);

    free(h);
}