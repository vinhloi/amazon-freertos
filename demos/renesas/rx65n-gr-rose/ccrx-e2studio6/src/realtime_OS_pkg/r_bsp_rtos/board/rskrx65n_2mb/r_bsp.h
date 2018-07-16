#ifndef REALTIME_OS_PKG_R_BSP_H
#define REALTIME_OS_PKG_R_BSP_H

#include    "mcu/all/r_bsp_common.h"
#include    "r_bsp_config.h"
#include    "r_bsp_interrupt_config.h"
#include    "mcu/rx65n/register_access/iodefine.h"
#include    "mcu/rx65n/mcu_info.h"
#include    "mcu/rx65n/mcu_locks.h"
#include    "mcu/rx65n/locking.h"
#include    "mcu/rx65n/cpu.h"
#include    "mcu/rx65n/mcu_init.h"
#include    "mcu/rx65n/mcu_interrupts.h"
#include    "mcu/rx65n/mcu_mapped_interrupts_private.h"
#include    "mcu/rx65n/mcu_mapped_interrupts.h"
#include    "mcu/rx65n/mcu_startup.h"
#include    "board/rskrx65n_2mb/rskrx65n_2mb.h"
#include    "board/rskrx65n_2mb/hwsetup.h"
#include    "board/rskrx65n_2mb/lowsrc.h"
#include    "board/rskrx65n_2mb/vecttbl.h"
#if BSP_CFG_RTOS_USED == 0      // Non-OS
#elif BSP_CFG_RTOS_USED == 1    // FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
/*#include "croutine.h"*/
#include "timers.h"
#include "event_groups.h"
#elif BSP_CFG_RTOS_USED == 2    // SEGGER embOS
#elif BSP_CFG_RTOS_USED == 3    // Micrium MicroC/OS
#elif BSP_CFG_RTOS_USED == 4    // Renesas RI600V4 & RI600PX
#endif

#endif /* REALTIME_OS_PKG_R_BSP_H */

