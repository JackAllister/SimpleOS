#include <stdint.h>
#include "graphics.h"

/******************** Module Typedefs ********************/

/******************** Module Constants ********************/

/* Defines the maximum pixel widths to use in the video mode. */
#define MAX_APPROPRIATE_WIDTH 1920
#define MAX_APPROPRIATE_HEIGHT 1080

/******************** Module Variables ********************/

/******************** Module Prototypes ********************/

EFI_STATUS findMostAppropriateMode(graphics_info_t* graphicsInfo);

/******************** Public Code ********************/

/* Initialises the graphics module, and returns the graphics information. */
EFI_STATUS graphics_init(EFI_SYSTEM_TABLE* systemTable, graphics_info_t* graphicsInfo)
{
    static const EFI_GUID PROTOCOL_GUID = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

    Print(L"\tInitialising graphics module.\r\n");

    EFI_STATUS status = uefi_call_wrapper(systemTable->BootServices->LocateProtocol, 3, &PROTOCOL_GUID, NULL, &graphicsInfo->protocol);

    if (FALSE == EFI_ERROR(status))
    {
        status = findMostAppropriateMode(graphicsInfo);

        if (FALSE == EFI_ERROR(status))
        {
            /* Appropriate mode has been selected, so set the mode. */
            status = uefi_call_wrapper(graphicsInfo->protocol->SetMode, 2, graphicsInfo->protocol, graphicsInfo->mode);

            if (FALSE == EFI_ERROR(status))
            {
                /* Successfully set the graphics mode, store buffer info. */
                graphicsInfo->bufferBase = (void*)graphicsInfo->protocol->Mode->FrameBufferBase;
                graphicsInfo->bufferSize = graphicsInfo->protocol->Mode->FrameBufferSize;

                Print(L"Frame buffer base: 0x%lX.\r\n", graphicsInfo->bufferBase);
                Print(L"Frame buffer size: %d.\r\n", graphicsInfo->bufferSize);
            }
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
    UINTN size;

    uint32_t maxFoundVerticalResolution = 0;
    uint32_t maxFoundHorizontalResolution = 0;

    Print(L"\t\tFinding best video mode.\r\n");

    /* Initialise the info to the current mode, if we can't find a better one this
     * will be the one that is defaulted. */
    status = uefi_call_wrapper(graphicsInfo->protocol->QueryMode, 4, 
                                graphicsInfo->protocol, 
                                graphicsInfo->protocol->Mode->Mode, 
                                &size, 
                                &pCurrentModeInfo);
    
    if (FALSE == EFI_ERROR(status))
    {
        /* Set the best to the first one we have successfully found. 
         * This is the default if no other suitable ones are found. */
        graphicsInfo->modeInfo = *pCurrentModeInfo;
        graphicsInfo->mode = graphicsInfo->protocol->Mode->Mode;

        Print(L"\t\tGraphics modes available: %d\r\n", graphicsInfo->protocol->Mode->MaxMode);

        /* Loop through all available modes and see if they match the criteria. */
        for (uint32_t i = 0; i < graphicsInfo->protocol->Mode->MaxMode; i++)
        {
            status = uefi_call_wrapper(graphicsInfo->protocol->QueryMode, 4, graphicsInfo->protocol, i, &size, &pCurrentModeInfo);
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
                            graphicsInfo->modeInfo = *pCurrentModeInfo;
                            graphicsInfo->mode = i;

                            maxFoundHorizontalResolution = pCurrentModeInfo->HorizontalResolution;
                            maxFoundVerticalResolution = pCurrentModeInfo->VerticalResolution;
                        }
                    }
                }
            }
        }
    }

    Print(L"\t\tGraphics mode %d selected: %d x %d.\r\n", 
          graphicsInfo->mode,
          graphicsInfo->modeInfo.HorizontalResolution, 
          graphicsInfo->modeInfo.VerticalResolution);

    return status;
}
