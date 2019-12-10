#define _CAMEL_TASK_C_

#include "camelOS.h"
#include "camel_task.h"
//#include "camel_projdefs.h"

// 当前任务控制块
TCB_t * volatile pxCurrentTCB = NULL;

// 任务就绪列表
List_t pxReadyTasksLists[ configMAX_PRIORITIES ];  


static TaskHandle_t xIdleTaskHandle     = NULL ;

static volatile TickType_t xTickCount   = ( TickType_t ) 0U;





static void prvInitNewTask( TaskFunction_t pxTaskCode,          // 任务入口
                            const char * const pcName,          // 任务名称, 字符串形式
                            const uint32_t ulStackDepth,        // 任务栈大小，单位为字
                            void * const pvParameters,          // 任务形参
                            TaskHandle_t * const pxCreatedTask, // 任务句柄
                            TCB_t *pxNewTCB);                   // 任务控制块指针


// 创建静态任务函数
#if ( configSUPPORT_STATIC_ALLOCATION == 1)

TaskHandle_t xTaskCreateStatic( TaskFunction_t pxTaskCode,          // 任务入口 
                                const char * const pcName,          // 任务名称, 字符串形式
                                const uint32_t ulStackDepth,        // 任务栈大小，单位为字
                                void * const pvParameters,          // 任务形参
                                StackType_t * const puxStackBuffer, // 任务栈起始地址
                                TCB_t * const pxTaskBuffer )        // 任务控制块指针
{
    TCB_t *pxNewTCB;
    TaskHandle_t xReturn;
    
    // 判断 栈空间和 TCB 控制块指针是否正常, 如果正常就创建任务
    if(( pxTaskBuffer != NULL )&&( puxStackBuffer != NULL ))
    {
        pxNewTCB = ( TCB_t *) pxTaskBuffer;
        pxNewTCB->pxStack = ( StackType_t *) puxStackBuffer;
        
        // 创建新任务
        prvInitNewTask( pxTaskCode,     // 任务入口
                        pcName,         // 任务名称,字符串形式
                        ulStackDepth,   // 任务栈大小,单位为字
                        pvParameters,   // 任务形参
                        &xReturn,       // 任务句柄
                        pxNewTCB);      // 任务栈起始地址
    }
    else
    {
        xReturn = NULL;
    }
    
    // 返回任务句柄,如果任务创建成功,此时 xReturn 应该指向任务控制块
    return xReturn;
}
                                    
#endif // configSUPPORT_STATIC_ALLOCATION



// 初始化任务
static void prvInitNewTask( TaskFunction_t pxTaskCode,          // 任务入口
                            const char * const pcName,          // 任务名称, 字符串形式
                            const uint32_t ulStackDepth,        // 任务栈大小，单位为字
                            void * const pvParameters,          // 任务形参
                            TaskHandle_t * const pxCreatedTask, // 任务句柄
                            TCB_t *pxNewTCB)                    // 任务控制块指针
{
    StackType_t *pxTopOfStack;
    UBaseType_t x;
    
    // 获取栈顶指针
    pxTopOfStack = pxNewTCB->pxStack + ( ulStackDepth - (uint32_t) 1 );
    
    // 向下做 8 字节对齐
    pxTopOfStack = (StackType_t *) ((( uint32_t ) pxTopOfStack ) & ( ~(( uint32_t ) 0x0007 )));
    
    // 将任务的名字存储在 TCB 中
    for( x = (UBaseType_t) 0; x < (UBaseType_t)configMAX_TASK_NAME_LEN; x ++)
    {
        pxNewTCB->pcTaskName[ x ] = pcName[ x ];
        
        if( pcName[ x ] == 0x00 )
        {
            break;
        }
    }
    // 任务名字的长度不能超过 configMAX_TASK_NAME_LEN
    pxNewTCB->pcTaskName[ configMAX_TASK_NAME_LEN - 1] = '\0';
    
    // 初始化 TCB 中的 xStateListNode 节点
    vListInitNode( &( pxNewTCB->xStateListNode ) );
    // 设置 xStateListNode 节点拥有者
    listSET_LIST_NODE_OWNER( &( pxNewTCB->xStateListNode ), pxNewTCB );
    
    // 初始化任务栈
    pxNewTCB->pxTopOfStack = pxPortInitStack( pxTopOfStack, pxTaskCode, pvParameters );
        
    // 让任务句柄指向任务控制块
    if(( void *) pxCreatedTask != NULL )
    {
        *pxCreatedTask = ( TaskHandle_t )pxNewTCB;
    }
    
}

// 初始化任务相关列表
void prvInitTaskLists(void)
{
    UBaseType_t uxPriority;
    
    for( uxPriority = ( UBaseType_t )0U; uxPriority < (UBaseType_t) configMAX_PRIORITIES; uxPriority ++)
    {
        vListInit( &( pxReadyTasksLists[ uxPriority ]));
    }
}


extern TCB_t Task1TCB;
extern TCB_t Task2TCB;
extern TCB_t IdleTaskTCB;

// 空闲任务声明
extern void vApplicationGetIdleTaskMemory( TCB_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize );

static portTASK_FUNCTION( prvIdleTask, pvParameters )
{
    ( void )pvParameters;
    
    for(;;)
    {
        // 空闲任务 暂时什么都不做
    }
}


void vTaskStartScheduler(void)
{
    // ============创建空闲任务 start ===================//
    TCB_t *pxIdleTaskTCBBuffer = NULL;          // 用于指向空闲任务控制块
    StackType_t *pxIdleTaskStackBuffer = NULL;  // 用于空闲任务栈起始地址
    uint32_t ulIdleTaskStackSize;
    
    // 获取空闲任务的内存：任务栈和任务 TCB
    vApplicationGetIdleTaskMemory(  &pxIdleTaskTCBBuffer,
                                    &pxIdleTaskStackBuffer,
                                    &ulIdleTaskStackSize    );
    
    xIdleTaskHandle = xTaskCreateStatic((TaskFunction_t)prvIdleTask,   // 任务入口
                                        (char *)"IDLE",
                                        (uint32_t)ulIdleTaskStackSize,
                                        (void *) NULL,
                                        (StackType_t *)pxIdleTaskStackBuffer,
                                        (TCB_t *)pxIdleTaskTCBBuffer    );
    // 将任务添加到就绪列表
    vListInsertEnd( &( pxReadyTasksLists[0] ), &( ((TCB_t *)pxIdleTaskTCBBuffer)->xStateListNode ));
    // ==========创建空闲任务 End ===================== //
    
    
    // 手动指定第一个运行的任务
    pxCurrentTCB = &Task1TCB;
    
    // 启动调度器
    if( xPortStartScheduler() != pdFALSE )
    {
        // 调度器启动成功，则不会返回，即不会到这里
    }
    
}

//void vTaskSwitchContext(void)
//{
//    // 轮流切换两个任务
//    if( pxCurrentTCB == &Task1TCB)
//    {
//        pxCurrentTCB =& Task2TCB;
//    }
//    else
//    {
//        pxCurrentTCB =& Task1TCB;
//    }
//}


void vTaskSwitchContext( void )
{
    // 如果当前线程是空闲线程，那么就去尝试执行线程1和线程2，看其延时时间是否结束，如果均没有结束那么就去执行空闲任务
    if( pxCurrentTCB == &IdleTaskTCB)
    {
        if(Task1TCB.xTicksToDelay == 0)
        {
            pxCurrentTCB = &Task1TCB;
        }
        else if(Task2TCB.xTicksToDelay == 0)
        {
            pxCurrentTCB = &Task2TCB;
        }
        else
        {
            return ;    //线程均到期则返回,继续执行空闲线程
        }
    }
    else
    {
        if(pxCurrentTCB == &Task1TCB)
        {
            if(Task2TCB.xTicksToDelay == 0)
            {
                pxCurrentTCB = &Task2TCB;
            }
            else if(pxCurrentTCB->xTicksToDelay != 0)
            {
                pxCurrentTCB = &IdleTaskTCB;
            }
            else
            {
                return ; // 两个任务都在延时，不进行切换
            }
        }
        else if(pxCurrentTCB == &Task2TCB)
        {
            if(Task1TCB.xTicksToDelay == 0)
            {
                pxCurrentTCB = &Task1TCB;
            }
            else if(pxCurrentTCB->xTicksToDelay != 0)
            {
                pxCurrentTCB = &IdleTaskTCB;
            }
            else
            {
                return ; // 两个任务都在延时，不进行切换
            }
        }
    }
    
}


void vTaskDelay( const TickType_t xTicksToDelay)
{
    TCB_t *pxTCB = NULL;
    
    // 获取当前任务的 TCB
    pxTCB = pxCurrentTCB;
    
    // 设置延时时间
    pxTCB->xTicksToDelay = xTicksToDelay;
    
    // 任务切换
    taskYIELD();
}

void xTaskIncrementTick( void )
{
    TCB_t *pxTCB = NULL;
    BaseType_t i = 0;
    
    // 更新系统时基计数器 xTickCount, xTickCount 是在 camel_port.c 中定义的一个全局变量。
    const TickType_t xConstTickCount = xTickCount + 1;
    xTickCount = xConstTickCount;
    
    // 扫描就绪列表中所有线程的 xTicksToDelay, 如果不为 0,则减 1
    for( i = 0; i < configMAX_PRIORITIES; i ++)
    {
        pxTCB = ( TCB_t * ) listGET_OWNER_OF_HEAD_ENTRY(( &pxReadyTasksLists[i] ));
        if( pxTCB->xTicksToDelay > 0 )
        {
            pxTCB->xTicksToDelay -- ;
        }
    }
    
    //任务切换
    portYIELD();
}
