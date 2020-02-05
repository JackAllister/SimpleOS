#ifndef DRAW_H
#define DRAW_H

#include "osTypes.h"
#include "graphicsInfo.h"

/******************** Public Defines ********************/

/******************** Public Typedefs ********************/

/* Definition of what a color is. */
typedef union
{
    struct {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    };
    uint32_t flags; /* Alternative to using struct. */
} draw_color_t;

/******************** Public Constants ********************/

/******************** Public Variables ********************/

/******************** Public Prototypes ********************/

os_status_t draw_init(EFI_SYSTEM_TABLE* systemTable, 
                      const graphics_info_t* graphicsInfo, 
                      os_handle_t* handle);
os_status_t draw_uninit(EFI_SYSTEM_TABLE* systemTable, os_handle_t handle);
os_status_t draw_setPixel(os_handle_t handle, uint32_t x, uint32_t y, draw_color_t color);
os_status_t draw_clearScreen(os_handle_t handle);

#endif /* DRAW_H */