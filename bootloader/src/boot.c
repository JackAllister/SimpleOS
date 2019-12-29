#include <efi.h>
#include <efilib.h>
#include "graphics.h"
 
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
    }




    for(;;) __asm__("hlt");
 
    return status;
}