#include <efi.h>
#include <efilib.h>
#include "graphics.h"
 
EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);

    /* Clear all previous information on the screen. */
    uefi_call_wrapper(SystemTable->ConOut->ClearScreen, 1, SystemTable->ConOut);

    EFI_STATUS status = uefi_call_wrapper(SystemTable->ConOut->OutputString,
                                            2,
                                            SystemTable->ConOut,
                                            L"Initialising all required modules.\r\n");

    if (FALSE == EFI_ERROR(status))
    {
        graphics_info_t graphicsInfo;

        /* Initialise all required modules. */
        status = graphics_init(SystemTable, &graphicsInfo);
    }

    for(;;) __asm__("hlt");
 
    return status;
}