#include <stdint.h>
#include "graphics.h"

/******************** Module Typedefs ********************/

/* Structure containing instance data internal to the graphics module.
 * Incomplete type is used to export this as a "handle" to other modules.
 * No external modules should be able to the internal types. */
typedef struct _graphics_internal_t
{
    EFI_GRAPHICS_PIXEL_FORMAT pixelFormat;
} graphics_internal_t;

/******************** Module Constants ********************/

/* Defines the maximum pixel widths to use in the video mode. */
#define MAX_APPROPRIATE_WIDTH 1920
#define MAX_APPROPRIATE_HEIGHT 1080

/******************** Module Variables ********************/

/******************** Module Prototypes ********************/

EFI_STATUS findMostAppropriateMode(EFI_GRAPHICS_OUTPUT_PROTOCOL* protocol, uint32_t* foundMode);

/******************** Public Code ********************/

/* Initialises the graphics module, and returns the graphics information. */
EFI_STATUS graphics_init(EFI_SYSTEM_TABLE* systemTable, graphics_info_t* graphicsInfo)
{
    static const EFI_GUID PROTOCOL_GUID = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

    EFI_GRAPHICS_OUTPUT_PROTOCOL* protocol;

    Print(L"\tInitialising graphics module.\r\n");

    EFI_STATUS status = uefi_call_wrapper(systemTable->BootServices->LocateProtocol, 3, &PROTOCOL_GUID, NULL, &protocol);

    if (FALSE == EFI_ERROR(status))
    {
        uint32_t foundMode;

        status = findMostAppropriateMode(protocol, &foundMode);

        if (FALSE == EFI_ERROR(status))
        {
            /* Appropriate mode has been selected, so set the mode. */
            status = uefi_call_wrapper(protocol->SetMode, 2, protocol, foundMode);

            if (FALSE == EFI_ERROR(status))
            {
                 /* Allocate memory for the internal instance data. */
                graphics_internal_t* allocatedInternal;

                status = uefi_call_wrapper(systemTable->BootServices->AllocatePages,
                                           4,
                                           AllocateAnyPages,
                                           EfiLoaderData,
                                           1,
                                           &allocatedInternal);

                if (FALSE == EFI_ERROR(status))
                {
                    allocatedInternal->pixelFormat = protocol->Mode->Info->PixelFormat;

                    graphicsInfo->handle = allocatedInternal;

                    /* Successfully set the graphics mode, store buffer info. */
                    graphicsInfo->horizontal = protocol->Mode->Info->HorizontalResolution;
                    graphicsInfo->vertical = protocol->Mode->Info->VerticalResolution;
                    graphicsInfo->bufferBase = (void*)protocol->Mode->FrameBufferBase;
                    graphicsInfo->bufferSize = protocol->Mode->FrameBufferSize;

                    Print(L"Resolution: %dx%d.\r\n", graphicsInfo->horizontal, graphicsInfo->vertical);
                    Print(L"Frame buffer base: 0x%lX.\r\n", graphicsInfo->bufferBase);
                    Print(L"Frame buffer size: %d.\r\n", graphicsInfo->bufferSize);
                }
            }
        }
    }

    return status;
}

/******************** Module Code ********************/

/* Searches through all the graphics modes available and find the most appropriate for
 * the requirements of the bootloader. */
EFI_STATUS findMostAppropriateMode(EFI_GRAPHICS_OUTPUT_PROTOCOL* protocol, uint32_t* foundMode)
{
    EFI_STATUS status;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* pCurrentModeInfo;
    UINTN size;

    uint32_t maxFoundVerticalResolution = 0;
    uint32_t maxFoundHorizontalResolution = 0;

    Print(L"\t\tFinding best video mode.\r\n");

    /* Initialise the info to the current mode, if we can't find a better one this
     * will be the one that is defaulted. */
    status = uefi_call_wrapper(protocol->QueryMode, 4, 
                               protocol, 
                               protocol->Mode->Mode, 
                               &size, 
                               &pCurrentModeInfo);
    
    if (FALSE == EFI_ERROR(status))
    {
        /* Set the best to the first one we have successfully found. 
         * This is the default if no other suitable ones are found. */
        *foundMode = protocol->Mode->Mode;

        Print(L"\t\tGraphics modes available: %d\r\n", protocol->Mode->MaxMode);

        /* Loop through all available modes and see if they match the criteria. */
        for (uint32_t i = 0; i < protocol->Mode->MaxMode; i++)
        {
            status = uefi_call_wrapper(protocol->QueryMode, 4, protocol, i, &size, &pCurrentModeInfo);
            if (FALSE == EFI_ERROR(status))
            {
                // Print(L"\t\tGraphics mode %d Horizontal: %d Vertical: %d\r\n", 
                //       i, pCurrentModeInfo->HorizontalResolution, pCurrentModeInfo->VerticalResolution);

                /* Check to see if the pixel formats are appropriate. */
                if ((PixelRedGreenBlueReserved8BitPerColor == pCurrentModeInfo->PixelFormat) ||
                    (PixelBlueGreenRedReserved8BitPerColor == pCurrentModeInfo->PixelFormat))
                {
                    
                    /* Ensure the resolutions are not too high. */
                    if ((pCurrentModeInfo->HorizontalResolution <= MAX_APPROPRIATE_WIDTH) &&
                        (pCurrentModeInfo->VerticalResolution <= MAX_APPROPRIATE_HEIGHT))
                    {
                        
                        /* Try find the highest resolution that is within bounds. */
                        if ((pCurrentModeInfo->HorizontalResolution >= maxFoundHorizontalResolution) &&
                            (pCurrentModeInfo->VerticalResolution >= maxFoundVerticalResolution))
                        {
                            /* Find a better resolution mode, so store it and all other information required. */
                            *foundMode = i;

                            maxFoundHorizontalResolution = pCurrentModeInfo->HorizontalResolution;
                            maxFoundVerticalResolution = pCurrentModeInfo->VerticalResolution;
                        }
                    }
                }
            }
        }
    }

    return status;
}
