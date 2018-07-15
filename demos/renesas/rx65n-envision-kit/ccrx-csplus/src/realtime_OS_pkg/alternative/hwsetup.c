#include "platform.h"
#include "r_cg_macrodriver.h"

static void output_ports_configure(void);
static void interrupts_configure(void);
static void peripheral_modules_enable(void);
static void bsp_adc_initial_configure(void);

void hardware_setup(void)
{
    output_ports_configure();
    interrupts_configure();
    peripheral_modules_enable();
    bsp_non_existent_port_init();
    bsp_adc_initial_configure();
}

static void output_ports_configure(void)
{
}

static void interrupts_configure(void)
{
}

static void peripheral_modules_enable(void)
{
    R_Systeminit();
}

static void bsp_adc_initial_configure(void)
{
    volatile uint8_t tmp_mstp;

    /* Protect off. */
    SYSTEM.PRCR.WORD = 0xA502;

    /* Store the value of the MSTPCR. */
    tmp_mstp = MSTP(S12AD1);

    /* Release from the module-stop state */
    MSTP(S12AD1) = 0;

    /* Writing to the A/D conversion time setting register is enabled. */
    S12AD1.ADSAMPR.BYTE = 0x03;

    /* Sets conversion time for middle-speed for S12AD unit 1. */
    S12AD1.ADSAM.WORD = 0x0020;

    /* Writing to the A/D conversion time setting register is disabled. */
    S12AD1.ADSAMPR.BYTE = 0x02;

    /* Restore the value of the MSTPCR. */
    MSTP(S12AD1) = tmp_mstp;

    /* Protect on. */
    SYSTEM.PRCR.WORD = 0xA500;
}
