#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <efi.h>
#include <efilib.h>

/******************** Public Defines ********************/

/******************** Public Typedefs ********************/

/* Forward declaration used to hide internal datatype from external modules. */
typedef struct _graphics_internal_t* graphics_handle_t;

typedef struct
{
    graphics_handle_t handle;
    uint32_t horizontal;
    uint32_t vertical;
    void* bufferBase;
    uint32_t bufferSize;
} graphics_info_t;

/******************** Public Constants ********************/

/******************** Public Variables ********************/

/******************** Public Prototypes ********************/

EFI_STATUS graphics_init(EFI_SYSTEM_TABLE* systemTable, graphics_info_t* graphicsInfo);

#endif /* GRAPHICS_H */