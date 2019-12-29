#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <efi.h>

/******************** Public Defines ********************/

/******************** Public Typedefs ********************/

typedef struct
{
    EFI_GRAPHICS_OUTPUT_PROTOCOL protocol;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION modeInfo;
    void* bufferBase;
    UINT32 bufferSize;
} graphics_info_t;

/******************** Public Constants ********************/

/******************** Public Variables ********************/

/******************** Public Prototypes ********************/

EFI_STATUS graphics_init(EFI_SYSTEM_TABLE* systemTable, graphics_info_t* graphicsInfo);

#endif /* GRAPHICS_H */