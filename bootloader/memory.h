#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stdbool.h>
#include <efi.h>
#include <efilib.h>
#include "memoryInfo.h"

/******************** Public Defines ********************/

/******************** Public Typedefs ********************/

/******************** Public Constants ********************/

/******************** Public Variables ********************/

/******************** Public Prototypes ********************/

EFI_STATUS memory_init(EFI_SYSTEM_TABLE* systemTable, memory_info_t* pointerInfo);

#endif /* MEMORY_H */
