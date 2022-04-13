/*
 * RTOSDemo.c
 *
 * Created: 2022-04-11 10:31:46 PM
 * Author : Mark
 */

#include "FreeRTOS.h"
#include "task.h"

#define mainREGION_1_SIZE	8201
#define mainREGION_2_SIZE	29905
#define mainREGION_3_SIZE	7807

static void  prvInitialiseHeap(void);

void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char* pcTaskName);
void vApplicationTickHook(void);
void vApplicationGetIdleTaskMemory(StaticTask_t** ppxIdleTaskTCBBuffer, StackType_t** ppxIdleTaskStackBuffer, uint32_t* pulIdleTaskStackSize);
void vApplicationGetTimerTaskMemory(StaticTask_t** ppxTimerTaskTCBBuffer, StackType_t** ppxTimerTaskStackBuffer, uint32_t* pulTimerTaskStackSize);
static void prvSaveTraceFile(void);

StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];
static BaseType_t xTraceRunning = pdTRUE;


/*--------------------------------------------------------------------------------------------------*/
/* My Code starts here                                                                              */
/*--------------------------------------------------------------------------------------------------*/

TaskHandle_t myTask1Handle = NULL;
TaskHandle_t myTask2Handle = NULL;
int array[256];

void myTask1(void* p) {
    int count;
    count = 0;
    while (count <= 255) {
        array[count] = count++;
    }
    printf("A sorted array containing all ascii characters has been created\n");
    return;
}
void myTask2(void* p) {
    char ch;
    int first, last, middle, search;
    while (1) {
        printf("Which characters do you wish to locate using the binary search?\n");
        scanf("%c", &ch);
        search = ch;
        first = 0;
        last = 255;
        middle = 128;
        while (first <= last) {
            if (array[middle] < search)
                first = middle + 1;
            else if (array[middle] == search) {
                printf("%c is located at index %d.\n", ch, middle);
                break;
            }
            else
                last = middle - 1;
            middle = (first + last) / 2;
        }
        if (first > last)
            printf("Not found! %c is not present in the list.\n", (char)search);
    }
    return;
}

int main(void)
{
    prvInitialiseHeap();
    vTraceEnable(TRC_START);

    xTaskCreate(myTask1, "task1", 200, (void*)0, tskIDLE_PRIORITY, &myTask1Handle);
    xTaskCreate(myTask2, "task2", 200, (void*)0, tskIDLE_PRIORITY, &myTask2Handle);
    vTaskStartScheduler();
    return 1;
}

/*--------------------------------------------------------------------------------------------------*/

void vApplicationMallocFailedHook(void) {}
void vApplicationIdleHook(void) {}
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char* pcTaskName) {}
void vApplicationTickHook(void) {}
void vApplicationDaemonTaskStartupHook(void) {}
void vAssertCalled(unsigned long ulLine, const char* const pcFileName) {}
static void prvSaveTraceFile(void) {}

static void  prvInitialiseHeap(void)
{
    static uint8_t ucHeap[configTOTAL_HEAP_SIZE];
    volatile uint32_t ulAdditionalOffset = 19;
    const HeapRegion_t xHeapRegions[] =
    {
        { ucHeap + 1,											mainREGION_1_SIZE },
        { ucHeap + 15 + mainREGION_1_SIZE,						mainREGION_2_SIZE },
        { ucHeap + 19 + mainREGION_1_SIZE + mainREGION_2_SIZE,	mainREGION_3_SIZE },
        { NULL, 0 }
    };
    configASSERT((ulAdditionalOffset + mainREGION_1_SIZE + mainREGION_2_SIZE + mainREGION_3_SIZE) < configTOTAL_HEAP_SIZE);
    (void)ulAdditionalOffset;
    vPortDefineHeapRegions(xHeapRegions);
}

void vApplicationGetIdleTaskMemory(StaticTask_t** ppxIdleTaskTCBBuffer, StackType_t** ppxIdleTaskStackBuffer, uint32_t* pulIdleTaskStackSize)
{
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory(StaticTask_t** ppxTimerTaskTCBBuffer, StackType_t** ppxTimerTaskStackBuffer, uint32_t* pulTimerTaskStackSize)
{
    static StaticTask_t xTimerTaskTCB;

    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

