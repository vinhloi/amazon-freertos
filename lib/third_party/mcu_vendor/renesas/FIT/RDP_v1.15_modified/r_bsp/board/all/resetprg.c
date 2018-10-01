/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No 
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS 
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
* this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer 
*
* Copyright (C) 2011 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : resetprg.c
* Device(s)    : RX600v2/RX200v2,RX600,RX200
* Description  : Defines post-reset routines that are used to configure the MCU prior to the main program starting. 
*                This is were the program counter starts on power-up or reset.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 26.10.2011 1.00     First Release
*         : 13.03.2012 1.10     Stack sizes are now defined in r_bsp_config.h. Because of this the #include for 
*                               stacksct.h was removed. Settings for SCKCR are now set in r_bsp_config.h and used here
*                               to setup clocks based on user settings.
*         : 16.07.2012 1.20     Added ability to enable FPU Exceptions based on the BSP_CFG_EXCEP_FPU_ISR_CALLBACK macro
*                               and the ability to enable NMI interrupts based on the BSP_CFG_NMI_ISR_CALLBACK. Also 
*                               added code to enable BCLK and SDCLK output based on settings in r_bsp_config.h.
*         : 09.08.2012 1.30     Added checking of BSP_CFG_IO_LIB_ENABLE macro for calling I/O Lib functions.
*         : 09.10.2012 1.40     Added support for all clock options specified by BSP_CFG_CLOCK_SOURCE in r_bsp_config.h.
*                               The code now handles starting the clocks and the required software delays for the clocks
*                               to stabilize. Created clock_source_select() function.
*         : 19.11.2012 1.50     Updated code to use 'BSP_' and 'BSP_CFG_' prefix for macros.
*         : 26.06.2013 1.60     Removed auto-enabling of NMI pin interrupt handling. Fixed a couple of typos. Fixed
*                               a warning that was being produced by '#pragma inline_asm' directive. Added a call
*                               to bsp_interrupt_open() to init callbacks. Added ability to use 1 or both stacks.
*         : 07.01.2014 0.02     Added initialization of EXTB register
*         : 31.03.2014 1.70     Added the ability for user defined 'warm start' callback functions to be made from
*                               PowerON_Reset_PC() when defined in r_bsp_config.h.
*         : 31.03.2014 0.10     Added the ability for user defined 'warm start' callback functions to be made from
*                               PowerON_Reset_PC() when defined in r_bsp_config.h.
6         : 15.12.2014 1.80     Replaced R_BSP_SoftwareDelay() calls with loops because clock not set yet.
*         : 09.12.2015 1.90     Change the comment of Peripheral Clock Frequency.
*         : 15.05.2017 1.10     Deleted unnecessary comments.
*         :(15.05.2017 2.00     Deleted unnecessary comments.)
*                               Added the bsp startup module disable function.
*         :(01.11.2017 2.00     Added the bsp startup module disable function.)
*         : 01.07.2018 2.01     Include RTOS /timer preprocessing.
*         : 27.07.2018 2.02     Added the comment to for statement.
*                               Added the comment to while statement.
*         : xx.xx.xxxx 2.03     Added bsp_ram_initialize function call.
*                               Removed RTOS header files because they are included in the platform.h.
*                               Added support for GNUC and ICCRX.
*                               Splitted resetprg.c between resetprg.c and clkconf.c.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#if defined(__CCRX__)
/* Defines MCU configuration functions used in this file */
#include    <_h_c_lib.h>
#endif /* defined(__CCRX__) */

/* This macro is here so that the stack will be declared here. This is used to prevent multiplication of stack size. */
#define     BSP_DECLARE_STACK
/* Define the target platform */
#include    "platform.h"

/* When using the user startup program, disable the following code. */
#if (BSP_CFG_STARTUP_DISABLE == 0)

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* If the user chooses only 1 stack then the 'U' bit will not be set and the CPU will always use the interrupt stack. */
#if (BSP_CFG_USER_STACK_ENABLE == 1)
    #define PSW_init  (0x00030000)
#else
    #define PSW_init  (0x00010000)
#endif
#define FPSW_init (0x00000000)  /* Currently nothing set by default. */

/***********************************************************************************************************************
Pre-processor Directives
***********************************************************************************************************************/
/* Set this as the entry point from a power-on reset */
#if defined(__CCRX__)
#pragma entry PowerON_Reset_PC
#endif /* defined(__CCRX__) */

/***********************************************************************************************************************
External function Prototypes
***********************************************************************************************************************/
extern void _INITSCT(void);
extern void bsp_ram_initialize(void);
#if defined(__ICCRX__)
extern void __iar_data_init2(void);
extern void _iar_main_call(void);
#endif /* defined(__ICCRX__) */

#if BSP_CFG_USER_WARM_START_CALLBACK_PRE_INITC_ENABLED != 0
/* If user is requesting warm start callback functions then these are the prototypes. */
void BSP_CFG_USER_WARM_START_PRE_C_FUNCTION(void);
#endif

#if BSP_CFG_USER_WARM_START_CALLBACK_POST_INITC_ENABLED != 0
/* If user is requesting warm start callback functions then these are the prototypes. */
void BSP_CFG_USER_WARM_START_POST_C_FUNCTION(void);
#endif

#if BSP_CFG_RTOS_USED == 1  //FreeRTOS
/* A function is used to create a main task, rtos's objects required to be available in advance. */
extern void Processing_Before_Start_Kernel(void);
#endif

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
/* Power-on reset function declaration */
#if defined(__CCRX__)
void PowerON_Reset_PC(void);
#elif defined(__ICCRX__)
int __low_level_init ( void );
#endif /* defined(__CCRX__), defined(__ICCRX__) */

/* Main program function declaration */
#if BSP_CFG_RTOS_USED == 0	// Non-OS
void main(void);
#endif

/***********************************************************************************************************************
* Function name: PowerON_Reset_PC
* Description  : This function is the MCU's entry point from a power-on reset.
*                The following steps are taken in the startup code:
*                1. The User Stack Pointer (USP) and Interrupt Stack Pointer (ISP) are both set immediately after entry 
*                   to this function. The USP and ISP stack sizes are set in the file bsp_config.h.
*                   Default sizes are USP=4K and ISP=1K.
*                2. The interrupt vector base register is set to point to the beginning of the relocatable interrupt 
*                   vector table.
*                3. The MCU is setup for floating point operations by setting the initial value of the Floating Point 
*                   Status Word (FPSW).
*                4. The MCU operating frequency is set by configuring the Clock Generation Circuit (CGC) in
*                   operating_frequency_set.
*                5. Calls are made to functions to setup the C runtime environment which involves initializing all 
*                   initialed data, zeroing all uninitialized variables, and configuring STDIO if used
*                   (calls to _INITSCT and _INIT_IOLIB).
*                6. Board-specific hardware setup, including configuring I/O pins on the MCU, in hardware_setup.
*                7. Global interrupts are enabled by setting the I bit in the Program Status Word (PSW), and the stack 
*                   is switched from the ISP to the USP.  The initial Interrupt Priority Level is set to zero, enabling 
*                   any interrupts with a priority greater than zero to be serviced.
*                8. The processor is optionally switched to user mode.  To run in user mode, set the macro 
*                   BSP_CFG_RUN_IN_USER_MODE above to a 1.
*                9. The bus error interrupt is enabled to catch any accesses to invalid or reserved areas of memory.
*
*                Once this initialization is complete, the user's main() function is called.  It should not return.
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
#if defined(__CCRX__) || defined(__GNUC__)
void PowerON_Reset_PC(void)
#elif defined(__ICCRX__)
int __low_level_init ( void )
#endif /* defined(__CCRX__), defined(__GNUC__), defined(__ICCRX__) */
{
#if defined(__CCRX__)
    /* Stack pointers are setup prior to calling this function - see comments above */    
    
    /* Initialize the Interrupt Table Register */
    set_intb((void *)__sectop("C$VECT"));

#ifdef __RXV2
    /* Initialize the Exception Table Register */
    set_extb((void *)__sectop("EXCEPTVECT"));
#endif
#endif /* defined(__CCRX__) */

#if defined(__CCRX__)
    /* Initialize FPSW for floating-point operations */
#ifdef __ROZ
#define FPU_ROUND 0x00000001  /* Let FPSW RMbits=01 (round to zero) */
#else 
#define FPU_ROUND 0x00000000  /* Let FPSW RMbits=00 (round to nearest) */
#endif 
#ifdef __DOFF 
#define FPU_DENOM 0x00000100  /* Let FPSW DNbit=1 (denormal as zero) */
#else 
#define FPU_DENOM 0x00000000  /* Let FPSW DNbit=0 (denormal as is) */
#endif 

    R_SET_FPSW((R_SET_FPSW_CAST_ARGS1)(FPSW_init | FPU_ROUND | FPU_DENOM));
#endif /* defined(__CCRX__) */

    /* Switch to high-speed operation */
    operating_frequency_set();

    /* If the warm start Pre C runtime callback is enabled, then call it. */
#if BSP_CFG_USER_WARM_START_CALLBACK_PRE_INITC_ENABLED == 1
     BSP_CFG_USER_WARM_START_PRE_C_FUNCTION();
#endif

    /* Initialize C runtime environment */
#if defined(__CCRX__) || defined(__GNUC__)
    _INITSCT();
#elif defined(__ICCRX__)
    __iar_data_init2();
#endif /* defined(__CCRX__), defined(__GNUC__), defined(__ICCRX__) */

    /* Initialize RAM */
    bsp_ram_initialize();

    /* If the warm start Post C runtime callback is enabled, then call it. */
#if BSP_CFG_USER_WARM_START_CALLBACK_POST_INITC_ENABLED == 1
     BSP_CFG_USER_WARM_START_POST_C_FUNCTION();
#endif

#if BSP_CFG_IO_LIB_ENABLE == 1
    /* Comment this out if not using I/O lib */
#if defined(__CCRX__)
    _INIT_IOLIB();
#endif /* defined(__CCRX__) */
#endif

    /* Initialize MCU interrupt callbacks. */
    bsp_interrupt_open();

    /* Initialize register protection functionality. */
    bsp_register_protect_open();

    /* Configure the MCU and board hardware */
    hardware_setup();

    /* Change the MCU's user mode from supervisor to user */
#if defined(__CCRX__) || defined(__GNUC__)
    R_NOP();
    R_SET_PSW((R_SET_PSW_CAST_ARGS1)PSW_init);
#elif defined(__ICCRX__)
    asm("POP R15");
    R_SET_PSW((R_SET_PSW_CAST_ARGS1)PSW_init);
    asm("PUSH.L R15");
#endif /* defined(__CCRX__), defined(__GNUC__), defined(__ICCRX__) */

#if BSP_CFG_RUN_IN_USER_MODE==1
    R_CHG_PMUSR();
#endif

    /* Enable the bus error interrupt to catch accesses to illegal/reserved areas of memory */
    R_BSP_InterruptControl(BSP_INT_SRC_BUS_ERROR, BSP_INT_CMD_INTERRUPT_ENABLE, FIT_NO_PTR);

#if BSP_CFG_RTOS_USED == 0		// Non-OS
    /* Call the main program function (should not return) */
#if defined(__CCRX__) || defined(__GNUC__)
    main();
#elif defined(__ICCRX__)
    _iar_main_call();
#endif /* defined(__CCRX__), defined(__GNUC__), defined(__ICCRX__) */

#elif BSP_CFG_RTOS_USED == 1    // FreeRTOS
    /* Lock the channel that system timer of RTOS is using. */
    #if (((BSP_CFG_RTOS_SYSTEM_TIMER) >=0) && ((BSP_CFG_RTOS_SYSTEM_TIMER) <= 3))
        if (R_BSP_HardwareLock((mcu_lock_t)(BSP_LOCK_CMT0 + BSP_CFG_RTOS_SYSTEM_TIMER)) == false)
        {
            /* WAIT_LOOP */
            while(1);
        }
    #else
        #error "Setting BSP_CFG_RTOS_SYSTEM_TIMER is invalid."
    #endif

    /* Prepare the necessary tasks, FreeRTOS's resources... required to be executed at the beginning
     * after vTaskStarScheduler() is called. Other tasks can also be created after starting scheduler at any time */
    Processing_Before_Start_Kernel();

    /* Call the kernel startup (should not return) */
    vTaskStartScheduler();
#elif BSP_CFG_RTOS_USED == 2    // SEGGER embOS
#elif BSP_CFG_RTOS_USED == 3    // Micrium MicroC/OS
#elif BSP_CFG_RTOS_USED == 4    // Renesas RI600V4 & RI600PX
#endif
    
#if BSP_CFG_IO_LIB_ENABLE == 1
    /* Comment this out if not using I/O lib - cleans up open files */
#if defined(__CCRX__)
    _CLOSEALL();
#endif /* defined(__CCRX__) */
#endif

    /* Infinite loop is intended here. */
    /* WAIT_LOOP */
    while(1)
    {
        /* Infinite loop. Put a breakpoint here if you want to catch an exit of main(). */
    }
}

#endif /* BSP_CFG_STARTUP_DISABLE == 0 */
