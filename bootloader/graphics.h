#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <stdbool.h>
#include <efi.h>
#include <efilib.h>
#include "graphicsInfo.h"

/******************** Public Defines ********************/

/******************** Public Typedefs ********************/

/******************** Public Constants ********************/

/******************** Public Variables ********************/

/******************** Public Prototypes ********************/

EFI_STATUS graphics_init(EFI_SYSTEM_TABLE* systemTable, 
                         graphics_info_t* pointerInfo);

#endif /* GRAPHICS_H */
