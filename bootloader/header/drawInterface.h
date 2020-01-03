#ifndef DRAW_INTERFACE_H
#define DRAW_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>
#include "osTypes.h"

/* A generic drawing module.
 *
 * The aim is to abstract away from the graphics driver and give
 * standard functions for drawing items such as rectangles etc. 
 * 
 * This reduces coupling of the application with a specific driver. */

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

/*** Functions that are required to be provided to the underlying driver. ***/

typedef bool (*fn_set_pixel_t)(os_handle_t handle, uint32_t x, uint32_t y, draw_color_t color);
typedef bool (*fn_clear_screen_t)(os_handle_t handle);

/* Structure holding the required draw functions all in one place. */
typedef struct
{
    fn_set_pixel_t setPixelFunc;
    fn_clear_screen_t clearScreenFunc;
} display_draw_functions_t;

/******************** Public Constants ********************/

/******************** Public Variables ********************/

/******************** Public Prototypes ********************/

#endif /* DRAW_INTERFACE_H */
