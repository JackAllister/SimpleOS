#include <stdint.h>
#include <string.h>
#include <efi.h>
#include <efilib.h>
#include "draw.h"

/******************** Module Defines ********************/

#define SIZE_OF_PIXEL 4

/******************** Module Typedefs *******************/

/* Structure containing instance data internal to the draw module.
 * Incomplete type is used to export this as a "handle" to other modules.
 * No external modules should be able to the internal types. */
typedef struct _draw_internal_t
{
    graphics_info_t info;
} draw_internal_t, *draw_handle_t;

/* Defines a type for a raw color byte buffer.
 * An array is used rather than a uint32 to explicitly declare endianness. */
typedef uint8_t raw_color_t[SIZE_OF_PIXEL];

/******************** Module Constants ******************/


/******************** Module Variables ******************/


/******************** Module Prototypes *****************/

static bool convertDrawColor(graphics_color_t format, draw_color_t drawColor, raw_color_t rawColor);

/******************** Public Code ***********************/

/* TODO: Remove system table from here, it is supposed to be usable by both kernel and bootloader. */
os_status_t draw_init(EFI_SYSTEM_TABLE* systemTable, 
                      const graphics_info_t* graphicsInfo, 
                      os_handle_t* handle)
{
    os_status_t status = ENOMEM;
    EFI_STATUS efiStatus;

    Print(L"\tInitialising draw module.\r\n");

    /* Allocate memory for the handle structure.
     * TODO: Remove the fixed 1 page allocation, mathematically calculate the number of pages.
     * TODO: Switch from using UEFI allocation, in the kernel we won't have this. */
    efiStatus = uefi_call_wrapper(systemTable->BootServices->AllocatePages,
                                4,
                                AllocateAnyPages,
                                EfiLoaderData,
                                1,
                                handle);

    if (FALSE == EFI_ERROR(efiStatus))
    {
        draw_handle_t drawHandle = (draw_handle_t)*handle;

        /* Store the graphics handle in the draw handle,
         * so we can access this information later. */
        memcpy(&drawHandle->info, graphicsInfo, sizeof(graphics_info_t));

        Print(L"Draw canvas: %dx%d.\r\n", drawHandle->info.horizontal, drawHandle->info.vertical);

        status = ESUCCESS;
    }

    return status;
}

os_status_t draw_uninit(EFI_SYSTEM_TABLE* systemTable, os_handle_t handle)
{
    os_status_t status = ENOENT;

    if (NULL != handle)
    {
        EFI_STATUS efiStatus;

        /* TODO: Remove the fixed page size de-allocation. */
        /* TODO: Switch from using EFI API, in the kernel we won't have this. */
        efiStatus = uefi_call_wrapper(systemTable->BootServices->FreePages,
                                      2,
                                      (EFI_PHYSICAL_ADDRESS)handle,
                                      1);

        if (FALSE == EFI_ERROR(efiStatus))
        {
            status = ESUCCESS;
        }
    }

    return status;
}

os_status_t draw_setPixel(os_handle_t handle, uint32_t x, uint32_t y, draw_color_t color)
{
    os_status_t status;

    if (NULL != handle)
    {
        draw_handle_t drawHandle = (draw_handle_t)handle;

        /* Ensure the specified pixel is within the display resolution. */
        if ((x < drawHandle->info.horizontal) &&
            (y < drawHandle->info.vertical))
        {
            
            /* Calculate the address of where the pixel is. */
            uint8_t* addrOfPixel = (uint8_t*)(drawHandle->info.buffer +
                                                (x * SIZE_OF_PIXEL) +
                                                (y * SIZE_OF_PIXEL) *
                                                drawHandle->info.pixelsPerScanLine);

            /* Convert the draw color to the correct format for EFI. 
             * raw_color_t is of array type, so therefor we don't need to ampersand (&) in the call. */
            raw_color_t rawColor;

            if (true == convertDrawColor(drawHandle->info.colorMode, color, rawColor))
            {
                /* DEBUG. */
                // Print(L"Frame buffer base: 0x%Xl\r\n", graphicsMode->FrameBufferBase);
                // Print(L"X: %d\r\n", x);
                // Print(L"Y: %d\r\n", y);
                // Print(L"PixelsPerScanLine: %d\r\n", graphicsMode->Info->PixelsPerScanLine);

                // Print(L"\r\n");
                // Print(L"Address of pixel: 0x%Xl\r\n", addrOfPixel);

                // Print(L"\r\n");
                // Print(L"Draw Color: 0x%X Red: %d Green: %d: Blue: %d\r\n", color.flags, color.red, color.green, color.blue);
                // Print(L"Raw Color: 0x%X\r\n", rawColor);

                /* Explicitly copy raw pixel buffer to the address of the pixel. */
                memcpy(addrOfPixel, rawColor, sizeof(rawColor));
                
                status = ESUCCESS;
            }
            else
            {
                /* An invalid color mode is in use. */
                status = EDOM;
            }
        }
        else
        {
            /* An invalid X || Y parameter was given. */
            status = ERANGE;
        }
    }
    else
    {
        /* An invalid handle was specified. */
        status = ENOENT;
    }

    return status;
}

/* Used for clearing the screen completely. */
os_status_t draw_clearScreen(os_handle_t handle)
{
    /* TODO: Implement. */
    return EPERM;
}

/******************** Module Code ***********************/



/* Converts the a draw color to the appropriate EFI color bytes. */
static bool convertDrawColor(graphics_color_t format, draw_color_t drawColor, raw_color_t rawColor)
{
    bool result;

    /* Depending on pixel format the order of the bytes for color changes. */
    if (GRAPHICS_COLOR_RGB == format)
    {
        /* Explicitly define the endianness. */
        rawColor[0] = drawColor.red;
        rawColor[1] = drawColor.green;
        rawColor[2] = drawColor.blue;
        rawColor[3] = 0; /* Reserved. */
        result = true;

    }
    else if (GRAPHICS_COLOR_BGR == format)
    {
        /* Explicitly define the endianness. */
        rawColor[0] = drawColor.blue;
        rawColor[1] = drawColor.green;
        rawColor[2] = drawColor.red;
        rawColor[3] = 0; /* Reserved. */
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}
