#ifndef _CAMEL_PORTMACRO_H_
#define _CAMEL_PORTMACRO_H_

#include "stdint.h"
#include "stddef.h"


// 数据类型重定义
#define portCHAR 	char
#define portFLOAT	float
#define portDOUBLE	double
#define portLONG	long
#define portSHORT	short
#define portSTACK_TYPE	uint32_t
#define portBASE_TYPE	long

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

#if (configUSE_16_BIT_TICKS == 1)
	typedef uint16_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffff
#else
	typedef uint32_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xfffffffUL
#endif


// 多优先级宏定义
#ifndef configUSE_PORT_OPTIMISED_TASK_SELECTION
	#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#endif

#if configUSE_PORT_OPTIMISED_TASK_SELECTION == 1
	// 检测优先级配置
	#if( configMAX_PRIORITIES > 32 )
		#error configUSE_PORT_OPTIMISED_TASK_SELECTION can only be set to 1 when configMAX_PRIORITIES is less than or equal to 32. It is very rare that a system requiress 
	#endif
	
	// 根据优先级设置/清除优先级位图中相应的位
	#define portRECORD_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) |= ( 1UL << ( uxPriority ))
	#define portRESET_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) &= ~( 1UL << ( uxPriority ))

	// ------------------------------- //
	#define portGET_HIGHEST_PRIORITY( uxTopPriority, uxReadyPriorities ) uxTopPriority = ( 31UL - ( uint32_t ) __clz( ( uxReadyPriorities )))

#endif // taskRECORD_READY_PRIORITY 



//中断控制状态寄存器，0xe000ed04
// bit 28 PENDSVSET： PendSV 挂起
#define portNVIC_INT_CTRL_REG   (*(( volatile uint32_t *)0xe000ed04 ))
#define portNVIC_PENDSVSET_BIT  ( 1UL << 28UL )     // 第 28 位设置为 1

#define portSY_FULL_READ_WRITE  (15)

#define portYIELD()                                 \
{                                                   \
    /* 触发 PendSV，产生上下文切换   */             \
    portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT; \
    __dsb( portSY_FULL_READ_WRITE );                \
    __isb( portSY_FULL_READ_WRITE );                \
}


// 临界段的保护
extern void vPortEnterCritical( void );
extern void vPortExitCritical( void );




// 空闲任务
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )


#define portINLINE  __inline

#ifndef portFORCE_INLINE
    #define portFORCE_INLINE    __forceinline
#endif

#define portDISABLE_INTERRUPTS()    vPortRaiseBASEPRI()
#define portENABLE_INTERRUPTS()     vPortSetBASEPRI( 0 )

#define portENTER_CRITICAL()        vPortEnterCritical()
#define portEXIT_CRITICAL()         vPortExitCritical()

#define portSET_INTERRUPT_MASK_FROM_ISR()   ulPortRaiseBASEPRI()    // 在中断中使用
#define portCLEAR_INTERRUPT_MASK_FROM_ISR() vPortSetBASEPRI( x )



//  关中断
static portFORCE_INLINE void vPortRaiseBASEPRI( void )
{
    uint32_t ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;
    
    __asm
    {
        msr basepri , ulNewBASEPRI     // 11 大于11 的不能被响应
        dsb
        isb
    }
}

// 开中断
static portFORCE_INLINE void vPortClearBASEPRIFromISR( void )
{
    __asm
    {
        msr basepri, #0
    }
}


static portFORCE_INLINE uint32_t ulPortRaiseBASEPRI( void )
{
    uint32_t ulReturn, ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;
    
    __asm
    {
        mrs ulReturn, basepri       // 将原先 basepri 的值保存在 返回值 中
        msr basepri, ulNewBASEPRI   // 再设置 basepri
        dsb
        isb
    }
    
    return ulReturn;
}


static portFORCE_INLINE void vPortSetBASEPRI( uint32_t ulBASEPRI )
{
    __asm
    {
        msr basepri, ulBASEPRI
    }
}



#endif  // _CAMEL_PORTMACRO_H_

