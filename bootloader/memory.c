#include <stdint.h>
#include "memory.h"

/******************** Module Defines ********************/


/******************** Module Typedefs ********************/


/******************** Module Constants ********************/


/******************** Module Variables ********************/


/******************** Module Prototypes ********************/


/******************** Public Code ********************/

EFI_STATUS memory_init(EFI_SYSTEM_TABLE* systemTable, memory_info_t* pointerInfo)
{
    /* To determine the size of the memory map buffer needed,
     * we call the GetMemoryMap function with a null buffer. */
    UINTN requiredSize = 0;
    EFI_STATUS status = systemTable->BootServices->GetMemoryMap(&requiredSize, NULL, NULL, NULL, NULL);
    if (EFI_BUFFER_TOO_SMALL == status)
    {
        /* Allocate the actual buffer size needed. */
        EFI_MEMORY_DESCRIPTOR* allocatedDescriptor;
        status = systemTable->BootServices->AllocatePool(EfiBootServicesData, requiredSize, &allocatedDescriptor);
        if (EFI_SUCCESS == status)
        {
            UINTN mapSize = requiredSize;
            UINTN mapKey;
            UINTN descriptorSize;
            UINT32 descriptorVersion;
            status = systemTable->BootServices->GetMemoryMap(&mapSize, allocatedDescriptor, &mapKey, &descriptorSize, &descriptorVersion);
            if (EFI_SUCCESS == status)
            {
                /* TODO: Return the memory information. */
            }
        }
    }

    return status;
}

/******************** Module Code ********************/
