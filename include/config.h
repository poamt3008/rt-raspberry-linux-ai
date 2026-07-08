/******************************************************************************
 * File: config.h
 * Author: Ander
 *
 * Project:
 * Real-Time AI System on Raspberry Pi
 *
 * Description:
 * Global configuration parameters.
 ******************************************************************************/
#ifndef CONFIG_H
#define CONFIG_H

/*Camara configuration*/
#define CAMERA_DEVICE "/dev/video0"
#define IMAGE_DIRECTORY "images"

/******************************************************************************
 * Capture Task
 ******************************************************************************/
#define CAPTURE_PERIOD_MS 5000
/* Number of images for generation*/
#define NUMBER_IMAGES 10

/******************************************************************************
 * Inference Task
 ******************************************************************************/
#define INFERENCE_PERIOD_MS   1000

/******************************************************************************
 * wifi Network Detection Task
 ******************************************************************************/
#define HOME_WIFI_SSID   "NaruKira_2163"
#define PHONE_WIFI_SSID  "iPhone de Anderson"
/* Blink period for task_network, in milliseconds */
#define NETWORK_PERIOD_MS 1000

/******************************************************************************
 * Image processing
 ******************************************************************************/

/* Grayscale threshold (0-255) below which a pixel is considered "ink"
 * against the white paper background. */
#define DIGIT_THRESHOLD 128

/******************************************************************************
 * Dataset
 ******************************************************************************/
#define DATASET_PATH        "datasets/test_data/input"
#define NUMBER_TEST_IMAGES  10

/******************************************************************************
 * GPIO
 ******************************************************************************/
#define GPIO_LED_GREEN     27
#define GPIO_LED_RED       17

#endif