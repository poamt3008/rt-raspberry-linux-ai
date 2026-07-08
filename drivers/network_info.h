/******************************************************************************
 * File: network_info.h
 * Author: Ander
 *
 * Description:
 * Encapsulates Wi-Fi SSID detection. No other module should call
 * iwgetid, popen, or any system networking API directly.
 ******************************************************************************/

#ifndef NETWORK_INFO_H
#define NETWORK_INFO_H

typedef enum {
    NETWORK_STATUS_HOME,     /* connected to HOME_WIFI_SSID  */
    NETWORK_STATUS_PHONE,    /* connected to PHONE_WIFI_SSID */
    NETWORK_STATUS_OTHER,    /* connected to any other network, or disconnected */
} network_status_t;
/**
 * network_info_get_status - Query the currently connected SSID and
 * classify it as HOME, PHONE, or OTHER.
 *
 * Internally runs `iwgetid -r` and compares the result against
 * HOME_WIFI_SSID / PHONE_WIFI_SSID from config.h.
 *
 * Return: the classified status. On any internal failure (iwgetid
 * not available, no interface, etc.) returns NETWORK_STATUS_OTHER,
 * treating "no known network" the same as "unknown network".
 */
network_status_t network_info_get_status(void);

#endif