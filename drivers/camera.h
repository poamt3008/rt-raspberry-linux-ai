/******************************************************************************
 * File: camera.h
 * Author: Ander
 * Date: 24/06/2026
 *
 * Description:
 * Driver para la captura de imágenes mediante una cámara USB.
 ******************************************************************************/

#ifndef CAMERA_H
#define CAMERA_H

/**
 * @brief Inicializa el driver de la cámara.
 *
 * @return 0 si todo fue correcto.
 */
int camera_init(void);

/**
 * @brief Captura una imagen y la guarda en un archivo.
 *
 * @param filename Ruta donde guardar la imagen.
 *
 * @return 0 si fue exitoso.
 */
int camera_capture(const char *filename);

/**
 * @brief Libera recursos del driver.
 */
void camera_close(void);

#endif