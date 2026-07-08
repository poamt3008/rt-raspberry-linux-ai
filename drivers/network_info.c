/******************************************************************************
 * File: network_info.c
 ******************************************************************************/
#include "network_info.h"
#include "config.h"

#include <stdio.h>
#include <string.h>

network_status_t network_info_get_status(void)
{
    FILE *fp;
    char ssid[128];
    size_t len;

    fp = popen("iwgetid -r", "r");
    if (!fp) {
        perror("network_info_get_status: popen");
        return NETWORK_STATUS_OTHER;
    }

    if (!fgets(ssid, sizeof(ssid), fp)) {
        /* No output: not associated to any network */
        pclose(fp);
        return NETWORK_STATUS_OTHER;
    }

    pclose(fp);

    /* Strip trailing newline left by iwgetid's output */
    len = strlen(ssid);
    if (len > 0 && ssid[len - 1] == '\n') {
        ssid[len - 1] = '\0';
    }

    if (strcmp(ssid, HOME_WIFI_SSID) == 0) {
        return NETWORK_STATUS_HOME;
    }

    if (strcmp(ssid, PHONE_WIFI_SSID) == 0) {
        return NETWORK_STATUS_PHONE;
    }

    return NETWORK_STATUS_OTHER;
}