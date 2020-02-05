#include <efi.h>
#include <efilib.h>
#include "graphics.h"
#include "draw.h"
 
EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS status;
    EFI_LOADED_IMAGE* loadedImage = NULL;

    /* Initialise the UEFI library. */
    InitializeLib(ImageHandle, SystemTable);

    /* Clear all previous information on the screen. */
    uefi_call_wrapper(SystemTable->ConOut->ClearScreen, 1, SystemTable->ConOut);

    /* Get the image information for this application. */
    status = uefi_call_wrapper(SystemTable->BootServices->HandleProtocol, 
                               3,
                               ImageHandle,
                               &LoadedImageProtocol,
                               &loadedImage);

    if (FALSE == EFI_ERROR(status))
    {
        Print(L"Bootloader Image Base: 0x%lX\r\n", loadedImage->ImageBase);

        // int waitForDebug = TRUE;

        // while (TRUE == waitForDebug)
        // {
        //     __asm__ __volatile__("pause");
        // }

        graphics_info_t graphicsInfo;

        /* Initialise all required modules. */
        status = graphics_init(SystemTable, &graphicsInfo);

        if (FALSE == EFI_ERROR(status))
        {
            /* TODO: Do the memory map here. */

            /* Initial the draw module, for some cool drawing. */
            os_handle_t drawHandle;

            os_status_t drawStatus = draw_init(SystemTable, &graphicsInfo, &drawHandle);

            if (ESUCCESS == drawStatus)
            {
                /* Print some cool art for debugging. */
                draw_color_t color = {0};
                    
                for (uint32_t x = 0; x < graphicsInfo.horizontal; x++)
                {
                    for (uint32_t y = 0; y < graphicsInfo.vertical; y++)
                    {
                        color.red = 0xFF;
                        //color.red = x % 0xFF;
                        //color.green = y % 0xFF;
                        //color.blue = (x + y) % 255;

                        /* TODO: Actually care about draw status. */
                        (void)draw_setPixel(drawHandle, x, y, color);
                    }
                }
            }
        }
    }


    for(;;) __asm__("hlt");
 
    return status;
}
