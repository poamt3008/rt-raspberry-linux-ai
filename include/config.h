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
 * Task periods
 ******************************************************************************/
#define CAPTURE_PERIOD_MS 5000
#define INFERENCE_PERIOD_MS   1000

#define NUMBER_IMAGES 5

/******************************************************************************
 * Dataset
 ******************************************************************************/

#define DATASET_PATH        "datasets/test_data/input"

#define NUMBER_TEST_IMAGES  10

/******************************************************************************
 * GPIO
 ******************************************************************************/

#define GPIO_LED_GREEN     17
#define GPIO_LED_RED       27

#endif