#ifndef OS_TYPES_H
#define OS_TYPES_H

#include <stdint.h>
#include <errno.h>

/* Standard types that the system relies on. */

/******************** Public Defines ********************/

#define ESUCCESS 0

/******************** Public Typedefs ********************/

/* Definition of a handle, this is effectively an opaque pointer
 * that will be used for identifying handles to drivers. */
typedef void* os_handle_t;

/* Definition of a status of the operating system. */
typedef uint32_t os_status_t;

/******************** Public Constants ********************/

/******************** Public Variables ********************/

/******************** Public Prototypes ********************/


#endif /* OS_TYPES_H */
