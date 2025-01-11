

/*============================ INCLUDES ======================================*/
#ifdef _RTE_
#   include "RTE_Components.h"
#endif

#ifdef RTE_CMSIS_View_EventRecorder
#   include "EventRecorder.h"
#endif

#include <stdint.h>
#include <stdio.h>
#include <perf_counter.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

void SysTick_Handler(void)
{
    __NOP();
}



#if __IS_COMPILER_IAR__
__attribute__((used, constructor))
#else
__attribute__((used, constructor(101)))
#endif
void app_platform_init(void)
{
#ifdef RTE_CMSIS_View_EventRecorder
    EventRecorderInitialize(0, 1);
#endif
}

int main(void)
{

    init_cycle_counter(false);

    printf("Hello world! \r\n");

    while(1) {
    }

}