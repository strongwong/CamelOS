#ifndef	_CAMEL_PORTABLE_H_
#define _CAMEL_PORTABLE_H_


#include "camel_portmacro.h"
//#include "camel_projdefs.h"

StackType_t *pxPortInitStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters );
BaseType_t xPortStartScheduler( void );

#endif

