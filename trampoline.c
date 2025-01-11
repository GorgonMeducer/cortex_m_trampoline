/*============================ INCLUDES ======================================*/
#ifdef _RTE_
#   include "RTE_Components.h"
#   include CMSIS_device_header
#endif

#include <cmsis_compiler.h>
#include "perf_counter.h"

#include "trampoline_cfg.h"


/*============================ MACROS ========================================*/

#ifndef __ROM_BASE_ADDRESS
#   define __ROM_BASE_ADDRESS       0x00000000
#endif
#ifndef __TRAMPOLINE_ENTRY_ADDRESS  
#   define __TRAMPOLINE_ENTRY_ADDRESS   0x00000200
#endif
/*============================ MACROFIED FUNCTIONS ===========================*/

#if __IS_COMPILER_IAR__
#   define __AT_ADDR(__ADDR)    @ __ADDR
#elif __IS_COMPILER_ARM_COMPILER_5__
#   define __AT_ADDR(__ADDR)    __attribute__((at(__ADDR)))
#elif __IS_COMPILER_ARM_COMPILER_6__
#   define __AT_ADDR(__ADDR)    __attribute__((section (".ARM.__at_" #__ADDR)))
#elif __IS_COMPILER_LLVM__
#   define __AT_ADDR(__ADDR)    __section(".ARM.__at_" #__ADDR) 
#else  /*__IS_COMPILER_GCC__: Using GCC as default for those GCC compliant compilers*/
#endif


#define AT_ADDR(__ADDR)     __AT_ADDR(__ADDR)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern const VECTOR_TABLE_Type __VECTOR_TABLE[];

extern uint32_t __INITIAL_SP;



/*============================ PROTOTYPES ====================================*/
extern
void __trampoline_entry(void);

extern
__NO_RETURN
void Reset_Handler(void);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

/*----------------------------------------------------------------------------*
 * Disable Semihosting                                                        *
 *----------------------------------------------------------------------------*/

#if __IS_COMPILER_ARM_COMPILER_6__
__asm(".global __use_no_semihosting\n\t");
#   ifndef __MICROLIB
__asm(".global __ARM_use_no_argv\n\t");
#   endif

__NO_RETURN
void _sys_exit(int ret)
{
    UNUSED_PARAM(ret);
    while(1) {}
}

#endif

#if defined(__MICROLIB)
_ARMABI_NORETURN 
ARM_NONNULL(1,2)
void __aeabi_assert(const char *chCond, const char *chLine, int wErrCode) 
{
    ARM_2D_UNUSED(chCond);
    ARM_2D_UNUSED(chLine);
    ARM_2D_UNUSED(wErrCode);
    while(1) {
        __NOP();
    }
}
#endif

#if !defined(__IS_COMPILER_GCC__)
void _ttywrch(int ch)
{
    UNUSED_PARAM(ch);
}
#endif



/*----------------------------------------------------------------------------*
 * Trampoline Entry                                                           *
 *----------------------------------------------------------------------------*/

#define DEFAULT_IRQ_HANDLER(handler_name)  \
void __NO_RETURN __WEAK handler_name(void); \
void handler_name(void) { \
    while(1) __NOP(); \
}


#define __BL_ISR(__ISR_NAME)                                                    \
extern                                                                          \
__NO_RETURN                                                                     \
void __ISR_NAME(void);                                                          \
__NO_RETURN                                                                     \
void __bl_##__ISR_NAME(void) {

#define __BL_ISR_END(__ISR_NAME)                                                \
    __ISR_NAME();                                                               \
}

#define BL_ISR(__ISR_NAME)      __BL_ISR(__ISR_NAME)
#define BL_ISR_END(__ISR_NAME)  __BL_ISR_END(__ISR_NAME)

/* Exceptions */
BL_ISR(NMI_Handler)
BL_ISR_END(NMI_Handler)


BL_ISR(HardFault_Handler)
BL_ISR_END(HardFault_Handler)

BL_ISR(MemManage_Handler)
BL_ISR_END(MemManage_Handler)

BL_ISR(BusFault_Handler)
BL_ISR_END(BusFault_Handler)

BL_ISR(UsageFault_Handler)
BL_ISR_END(UsageFault_Handler)

BL_ISR(SecureFault_Handler)
BL_ISR_END(SecureFault_Handler)

BL_ISR(SVC_Handler)
BL_ISR_END(SVC_Handler)

BL_ISR(DebugMon_Handler)
BL_ISR_END(DebugMon_Handler)

BL_ISR(PendSV_Handler)
BL_ISR_END(PendSV_Handler)



BL_ISR(SysTick_Handler)

#if 0
    /* some addtional code */
    if (is_bl_running()) {
        
        /* bootloader systick isr code */
        return ;
    }
#endif

BL_ISR_END(SysTick_Handler)


AT_ADDR(__ROM_BASE_ADDRESS)
__USED
const VECTOR_TABLE_Type __bl_VECTOR_TABLE[]  = {
  (VECTOR_TABLE_Type)(&__INITIAL_SP),  /*      Initial Stack Pointer */
    __trampoline_entry,                 /*      Reset Handler */
    __bl_NMI_Handler,                       /* -14: NMI Handler */
    __bl_HardFault_Handler,                 /* -13: Hard Fault Handler */
    __bl_MemManage_Handler,                 /* -12: MPU Fault Handler */
    __bl_BusFault_Handler,                  /* -11: Bus Fault Handler */
    __bl_UsageFault_Handler,                /* -10: Usage Fault Handler */
    __bl_SecureFault_Handler,               /*  -9: Secure Fault Handler */
    0,                                 /*      Reserved */
    0,                                 /*      Reserved */
    0,                                 /*      Reserved */
    __bl_SVC_Handler,                       /*  -5: SVCall Handler */
    __bl_DebugMon_Handler,                  /*  -4: Debug Monitor Handler */
    0,                                 /*      Reserved */
    __bl_PendSV_Handler,                    /*  -2: PendSV Handler */
    __bl_SysTick_Handler,                   /*  -1: SysTick Handler */
};

__NO_RETURN
__USED
AT_ADDR(__TRAMPOLINE_ENTRY_ADDRESS)
void __trampoline_entry(void)
{
    __disable_irq();
#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
    SCB->VTOR = (uint32_t)(&__VECTOR_TABLE[0]);
#endif

    Reset_Handler();
}