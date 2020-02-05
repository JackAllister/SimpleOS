#ifndef GRAPHICS_INFO_H
#define GRAPHICS_INFO_H

#include <stdint.h>
#include <stdbool.h>

/******************** Public Defines ********************/

/******************** Public Typedefs ********************/

typedef enum
{
    GRAPHICS_COLOR_UNKNOWN,
    GRAPHICS_COLOR_RGB,
    GRAPHICS_COLOR_BGR
} graphics_color_t;

typedef struct
{
    void* buffer;
    uint32_t bufferSize;
    uint32_t horizontal;
    uint32_t vertical;
    uint32_t pixelsPerScanLine;
    graphics_color_t colorMode;
} graphics_info_t;

/******************** Public Constants ********************/

/******************** Public Variables ********************/

/******************** Public Prototypes ********************/

#endif /* GRAPHICS_INFO_H */
