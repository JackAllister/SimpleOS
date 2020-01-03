#include <stdint.h>
#include <string.h>
#include "graphics.h"

/******************** Module Defines ********************/

#define SIZE_OF_PIXEL 4

/******************** Module Typedefs ********************/

/* Structure containing instance data internal to the graphics module.
 * Incomplete type is used to export this as a "handle" to other modules.
 * No external modules should be able to the internal types. */
typedef struct _graphics_internal_t
{
    EFI_GRAPHICS_OUTPUT_PROTOCOL* protocol;
} graphics_internal_t, *graphics_handle_t;

/* Defines a type for a raw color byte buffer.
 * An array is used rather than a uint32 to explicitly declare endianness. */
typedef uint8_t raw_color_t[SIZE_OF_PIXEL];

/******************** Module Constants ********************/

/* Defines the maximum pixel widths to use in the video mode. */
#define MAX_APPROPRIATE_WIDTH 1920
#define MAX_APPROPRIATE_HEIGHT 1080

/******************** Module Variables ********************/

/******************** Module Prototypes ********************/

static bool setPixel(os_handle_t handle, uint32_t x, uint32_t y, draw_color_t color);
static bool clearScreen(os_handle_t handle);
bool convertDrawColor(EFI_GRAPHICS_PIXEL_FORMAT format, draw_color_t drawColor, raw_color_t rawColor);
static EFI_STATUS findMostAppropriateMode(EFI_GRAPHICS_OUTPUT_PROTOCOL* protocol, uint32_t* foundMode);

/******************** Public Code ********************/

/* Initialises the graphics module, and returns the graphics information. */
EFI_STATUS graphics_init(EFI_SYSTEM_TABLE* systemTable,
                         os_handle_t* pointerHandle,
                         graphics_info_t* pointerInfo)
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
                Print(L"Video mode selected: %d\r\n", foundMode);

                 /* Allocate memory for the internal instance data/handle.
                  * TODO: Remove the fixed 1 page allocation, mathematically calculate number of pages. */
                status = uefi_call_wrapper(systemTable->BootServices->AllocatePages,
                                           4,
                                           AllocateAnyPages,
                                           EfiLoaderData,
                                           1,
                                           pointerHandle);

                if (FALSE == EFI_ERROR(status))
                {
                    /* De-reference the abstract pointer to its actual type. */
                    graphics_handle_t graphicsHandle = (graphics_handle_t)*pointerHandle;

                    /* Store internal state data. */
                    graphicsHandle->protocol = protocol;

                    /* Set the resulting information. */
                    pointerInfo->horizontal = protocol->Mode->Info->HorizontalResolution;
                    pointerInfo->vertical = protocol->Mode->Info->VerticalResolution;
                    pointerInfo->functions.setPixelFunc = setPixel;
                    pointerInfo->functions.clearScreenFunc = clearScreen;

                    Print(L"Resolution: %dx%d.\r\n", pointerInfo->horizontal, pointerInfo->vertical);
                }
            }
        }
    }

    return status;
}

/******************** Module Code ********************/

/* Used for setting a pixel to a specific color. */
static bool setPixel(os_handle_t handle, uint32_t x, uint32_t y, draw_color_t color)
{
    bool result = false;

    /* TODO: Implement proper return type for status rather than just true/false. */
    if (NULL != handle)
    {
        graphics_handle_t graphicsHandle = (graphics_handle_t)handle;

        /* Get the EFI graphics mode and info. */
        EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE* graphicsMode = graphicsHandle->protocol->Mode;

        /* Ensure the specified pixel is within the display resolution. */
        if ((x < graphicsMode->Info->HorizontalResolution) &&
            (y < graphicsMode->Info->VerticalResolution))
        {
            /* Calculate the address of where the pixel is. */
            uint8_t* addrOfPixel = (uint8_t*)(graphicsMode->FrameBufferBase +
                                                (x * SIZE_OF_PIXEL) +
                                                (y * SIZE_OF_PIXEL) *
                                                graphicsMode->Info->PixelsPerScanLine);

            /* Convert the draw color to the correct format for EFI. 
             * raw_color_t is of array type, so therefor we don't need to ampersand (&) in the call. */
            raw_color_t rawColor;

            if (true == convertDrawColor(graphicsMode->Info->PixelFormat, color, rawColor))
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
                result = true;
            }
        }
    }

    return result;
}

/* Used for clearing the screen completely. */
static bool clearScreen(os_handle_t handle)
{
    Print(L"Clearing screen.\r\n");
    return false;
}

/* Converts the a draw color to the appropriate EFI color bytes. */
bool convertDrawColor(EFI_GRAPHICS_PIXEL_FORMAT format, draw_color_t drawColor, raw_color_t rawColor)
{
    bool result;

    /* Depending on pixel format the order of the bytes for color changes. */
    if (PixelRedGreenBlueReserved8BitPerColor == format)
    {
        /* Explicitly define the endianness. */
        rawColor[0] = drawColor.red;
        rawColor[1] = drawColor.green;
        rawColor[2] = drawColor.blue;
        rawColor[3] = 0; /* Reserved. */
        result = true;

    }
    else if (PixelBlueGreenRedReserved8BitPerColor == format)
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

/* Searches through all the graphics modes available and find the most appropriate for
 * the requirements of the bootloader. */
static EFI_STATUS findMostAppropriateMode(EFI_GRAPHICS_OUTPUT_PROTOCOL* protocol, uint32_t* foundMode)
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
