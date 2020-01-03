#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <stdbool.h>
#include <efi.h>
#include <efilib.h>
#include "drawInterface.h"

/******************** Public Defines ********************/

/******************** Public Typedefs ********************/

typedef struct
{
    uint32_t horizontal;
    uint32_t vertical;
    display_draw_functions_t functions;
} graphics_info_t;

/******************** Public Constants ********************/

/******************** Public Variables ********************/

/******************** Public Prototypes ********************/

EFI_STATUS graphics_init(EFI_SYSTEM_TABLE* systemTable, 
                         os_handle_t* pointerHandle,
                         graphics_info_t* pointerInfo);

#endif /* GRAPHICS_H */
