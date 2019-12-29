#include <stdint.h>
#include "graphics.h"

/******************** Module Typedefs ********************/

/******************** Module Constants ********************/

/* Defines the maximum pixel widths to use in the video mode. */
#define MAX_APPROPRIATE_WIDTH 1080
#define MAX_APPROPRIATE_HEIGHT 720

/******************** Module Variables ********************/

/******************** Module Prototypes ********************/

EFI_STATUS findMostAppropriateMode(graphics_info_t* graphicsInfo);

/******************** Public Code ********************/

/* Initialises the graphics module, and returns the graphics information. */
EFI_STATUS graphics_init(EFI_SYSTEM_TABLE* systemTable, graphics_info_t* graphicsInfo)
{
    static const EFI_GUID PROTOCOL_GUID = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

    EFI_STATUS status = uefi_call_wrapper(systemTable->ConOut->OutputString,
                                        2,
                                        systemTable->ConOut,
                                        L"Initialising graphics module.\r\n");

    if (FALSE == EFI_ERROR(status))
    {
        status = uefi_call_wrapper(systemTable->BootServices->LocateProtocol, 3, &PROTOCOL_GUID, NULL, &graphicsInfo->protocol);

        if (FALSE == EFI_ERROR(status))
        {
            status = findMostAppropriateMode(graphicsInfo);
        }
    }

    return status;
}

/******************** Module Code ********************/

/* Searches through all the graphics modes available and find the most appropriate for
 * the requirements of the bootloader. */
EFI_STATUS findMostAppropriateMode(graphics_info_t* graphicsInfo)
{
    EFI_STATUS status;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* pCurrentModeInfo;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION bestModeInfo;
    UINTN size;

    uint32_t maxFoundVerticalResolution = 0;
    uint32_t maxFoundHorizontalResolution = 0;

    /* Initialise the info to the current mode, if we can't find a better one this
     * will be the one that is defaulted. */
    status = uefi_call_wrapper(graphicsInfo->protocol.QueryMode, 4, 
                                &graphicsInfo->protocol, 
                                graphicsInfo->protocol.Mode->Mode, 
                                &size, 
                                &pCurrentModeInfo);
    
    if (FALSE == EFI_ERROR(status))
    {
        /* Set the best to the first one we have successfully found. */
        bestModeInfo = *pCurrentModeInfo;

        /* Loop through all available modes and see if they match the criteria. */
        for (uint32_t i = 0; i < graphicsInfo->protocol.Mode->MaxMode; i++)
        {
            status = uefi_call_wrapper(graphicsInfo->protocol.QueryMode, 4, &graphicsInfo->protocol, i, &size, &pCurrentModeInfo);
            if (FALSE == EFI_ERROR(status))
            {
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
                            bestModeInfo = *pCurrentModeInfo;

                            maxFoundHorizontalResolution = pCurrentModeInfo->HorizontalResolution;
                            maxFoundVerticalResolution = pCurrentModeInfo->VerticalResolution;
                        }
                    }
                }
            }
        }
    }

    /* Set the graphics output mode, to the best found one. */
    graphicsInfo->modeInfo = bestModeInfo;

    return status;
}
