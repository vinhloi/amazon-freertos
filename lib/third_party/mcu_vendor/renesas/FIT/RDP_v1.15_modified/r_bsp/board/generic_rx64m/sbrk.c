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
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.    
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : sbrk.c
* Device(s)    : RX
* Description  : Configures the MCU heap memory.  The size of the heap is defined by the macro HEAPSIZE below.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 26.10.2011 1.00     First Release
*         : 12.03.2012 1.10     Heap size is now defined in r_bsp_config.h, not sbrk.h.
*         : 19.11.2012 1.20     Updated code to use 'BSP_' and 'BSP_CFG_' prefix for macros.
*         : 15.05.2017 1.30     Deleted unnecessary comments.
*         : 01.11.2017 2.00     Added the bsp startup module disable function.
*         : xx.xx.xxxx 2.01     Added support for GNUC and ICCRX.
*                               Deleted unnecessary stddef.h, stdio.h, and stdint.h includes.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_bsp_config.h"

/* Only use this file if heap is enabled in r_bsp_config. */
#if (BSP_CFG_HEAP_BYTES > 0)

/* Used for getting BSP_CFG_HEAP_BYTES macro. */
#include "platform.h"

/* When using the user startup program, disable the following code. */
#if (BSP_CFG_STARTUP_DISABLE == 0)

#if defined(__CCRX__) || defined(__GNUC__)

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Function Prototypes
***********************************************************************************************************************/
/* Memory allocation function prototype declaration */
int8_t  *sbrk(size_t size);

/***********************************************************************************************************************
Global Variables
***********************************************************************************************************************/
//const size_t _sbrk_size=      /* Specifies the minimum unit of */
/* the defined heap area */
extern int8_t *_s1ptr;

union HEAP_TYPE
{
    int32_t  dummy;             /* Dummy for 4-byte boundary */
    int8_t heap[BSP_CFG_HEAP_BYTES];    /* Declaration of the area managed by sbrk*/
};
/* Declare memory heap area */
static union HEAP_TYPE heap_area;
/* End address allocated by sbrk    */
static int8_t *brk=(int8_t *)&heap_area;

/***********************************************************************************************************************
* Function name: sbrk
* Description  : This function configures MCU memory area allocation.
* Arguments    : size - 
*                    assigned area size
* Return value : Start address of allocated area (pass)
*                -1 (failure)
***********************************************************************************************************************/
int8_t  *sbrk(size_t size)
{
    int8_t  *p;

    if ((brk + size) > (heap_area.heap + BSP_CFG_HEAP_BYTES))
    {
        /* Empty area size  */
        p = (int8_t *)-1;
    }
    else
    {
        /* Area assignment */
        p = brk;  

        /* End address update */
        brk += size;
    }

    /* Return result */
    return p;
}

#endif /* defined(__CCRX__), defined(__GNUC__) */

#endif /* BSP_CFG_STARTUP_DISABLE == 0 */

#endif /* BSP_CFG_HEAP_BYTES */