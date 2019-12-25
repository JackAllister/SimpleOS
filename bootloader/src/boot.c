#include <efi.h>
#include <efilib.h>
 
EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);
    EFI_STATUS status = uefi_call_wrapper(SystemTable->ConOut->OutputString,
                                            2,
                                            SystemTable->ConOut,
                                            L"Hello, World!\n");
    for(;;) __asm__("hlt");
 
    return EFI_SUCCESS;
}