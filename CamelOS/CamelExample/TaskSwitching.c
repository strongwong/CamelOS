#define _TASKSWITCHING_C_

#include "camelOS.h"
#include "camel_task.h"


// 全局变量
portCHAR flag1;
portCHAR flag2;

extern List_t pxReadyTasksLists[ configMAX_PRIORITIES ];  

// 定义任务栈
#define TASK1_STACK_SIZE        128
StackType_t Task1Stack[TASK1_STACK_SIZE];

#define TASK2_STACK_SIZE        128
StackType_t Task2Stack[TASK2_STACK_SIZE];

// 定义任务控制块
TCB_t Task1TCB;
TCB_t Task2TCB;

// 定义任务句柄
TaskHandle_t Task1_Handle;
TaskHandle_t Task2_Handle;


// 函数声明
void daley(uint32_t count);
void Task1_Entry( void *p_arg );
void Task2_Entry( void *p_arg );

void TaskSwitching_example(void)
{
    prvInitTaskLists();
    
    Task1_Handle = xTaskCreateStatic(  Task1_Entry,
                                       "Task1_Entry",
                                       TASK1_STACK_SIZE,
                                       NULL,
                                       Task1Stack,
                                       &Task1TCB );
    
    // 核心就是插入函数 vListInsert, 将任务插入到就绪列表中
    vListInsert(&pxReadyTasksLists[1], &Task1TCB.xStateListNode);
    
    Task2_Handle = xTaskCreateStatic(  Task2_Entry,
                                       "Task2_Entry",
                                       TASK2_STACK_SIZE,
                                       NULL,
                                       Task2Stack,
                                       &Task2TCB );
    vListInsert(&pxReadyTasksLists[2], &Task2TCB.xStateListNode);
    
    vTaskStartScheduler();
}

// 软件延时
void delay( uint32_t count)
{
    for(; count != 0; count --);
}

// 定义一个任务函数 Task1 入口函数
void Task1_Entry(void *p_arg)
{
    for(;;)
    {
        flag1 = 1;
//        delay( 100 );
        vTaskDelay( 2 );
        flag1 = 0;
//        delay( 100 );
        vTaskDelay( 2 );
//        taskYIELD();        // 注意，这里是手动切换任务
    }
}

// 定义一个任务函数 Task2 入口函数
void Task2_Entry(void *p_arg)
{
    for(;;)
    {
        flag2 = 1;
//        delay( 100 );
        vTaskDelay( 2 );
        flag2 = 0;
//        delay( 100 );
        vTaskDelay( 2 );
//        taskYIELD();
    }
}

