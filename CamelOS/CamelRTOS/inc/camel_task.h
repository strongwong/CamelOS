#ifndef _CAMEL_TASK_H_
#define _CAMEL_TASK_H_

//#include "camel_config.h"
#include "camel_list.h"

#define taskYIELD()   portYIELD()

// 定义任务控制块
typedef struct tskTaskControlBlock
{
    volatile StackType_t    *pxTopOfStack;  // 栈顶
    ListNode_t              xStateListNode; // 任务节点
    StackType_t             *pxStack;       // 任务栈起始地址
    char                    pcTaskName[configMAX_TASK_NAME_LEN]; 
    
    TickType_t              xTicksToDelay;  // 用于延时
} tskTCB;

typedef tskTCB TCB_t;


// 定义任务句柄
typedef void * TaskHandle_t;


// 函数声明
#if( configSUPPORT_STATIC_ALLOCATION == 1)
TaskHandle_t xTaskCreateStatic( TaskFunction_t pxTaskCode,
                                const char * const pcName,
                                const uint32_t ulStackDepth,
                                void * const pvParameters,
                                StackType_t * const puxStackBuffer,
                                TCB_t * const pxTaskBuffer );
#endif  // configSUPPORT_STATIC_ALLOCATION

void vTaskStartScheduler(void);
void prvInitTaskLists(void);
void vTaskSwitchContext(void);
                                
void vTaskDelay(const TickType_t xTicksToDelay);
void xTaskIncrementTick(void);
                                
#endif // _CAMEL_TASK_H_

