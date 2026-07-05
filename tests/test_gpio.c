/* quick smoke test, not part of tests/ yet */
#include "gpio.h"
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    gpio_handle_t *led = gpio_init("/dev/gpiochip0", 17,
                                    GPIO_DIR_OUTPUT, 0, "test_gpio_init");
    if (!led) {
        fprintf(stderr, "gpio_init failed\n");
        return 1;
    }
    printf("gpio_init OK, handle=%p\n", (void *)led);

    gpio_set(led, 1);
    printf("gpio_get() = %d\n", gpio_get(led));
    sleep(2);

    printf("Toggling 5 times, 1s apart...\n");
    for (int i = 0; i < 5; i++) {
        gpio_toggle(led);
        printf("gpio_get() = %d\n", gpio_get(led));
        sleep(1);
    }

    printf("Closing GPIO handle...\n");
    gpio_close(led);

    /* Confirm the kernel released the line: after this point,
     * gpioinfo should no longer show a consumer on line 17. */
    printf("Done. Check with: gpioinfo -c gpiochip0 17\n");

    return 0;
}