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
 * Copyright (C) 2017 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_fw_up_rx.c
 * Version      : 1.00
 * Description  : Firmware update 
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 02.10.2017 1.00     First Release
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 Includes <System Includes> , "Project Includes"
 ***********************************************************************************************************************/
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

/* Access to peripherals and board defines. */
#include "platform.h"

/* Access to Flash API */
#include "r_flash_rx_if.h"

/* Defines for Firmware update support */
#include "r_fw_up_rx_if.h"
#include "r_fw_up_buf.h"
#include "r_fw_up_rx_private.h"

/***********************************************************************************************************************
 Private variables and functions
 ***********************************************************************************************************************/
static bool is_opened = false;

static fw_up_return_t write_firmware(fw_up_fl_data_t *p_fl_data);
static fw_up_return_t fw_up_put_data(fw_up_fl_data_t *p_fl_data);
static fw_up_return_t fw_up_get_data(fw_up_fl_data_t *p_fl_data);

/***********************************************************************************************************************
 * Function Name: fw_up_open_flash
 * Description  : Initialize Flash function.
 * Arguments    : none
 * Return Value : FW_UP_SUCCESS -
 *                   Processing completed successfully.
 *                FW_UP_ERR_INITIALIZE -
 *                   Processing is failed.
 ***********************************************************************************************************************/
fw_up_return_t fw_up_open_flash(void)
{
    flash_err_t ret_flash;
    fw_up_return_t ret;

    ret_flash = R_FLASH_Open();
    if (FLASH_SUCCESS == ret_flash)
    {
        ret = FW_UP_SUCCESS;
    }
    else
    {
        ret = FW_UP_ERR_INITIALIZE;
    }

    return ret;
} /* End of function fw_up_open_flash() */

/***********************************************************************************************************************
 * Function Name: fw_up_open
 * Description  : Initialize firmware update function.
 * Arguments    : none
 * Return Value : FW_UP_SUCCESS -
 *                   Processing completed successfully.
 *                FW_UP_ERR_OPENED -
 *                   fw_up_open has already been run.
 ***********************************************************************************************************************/
fw_up_return_t fw_up_open(void)
{
    fw_up_return_t ret = FW_UP_SUCCESS;

    /* Check that the fw_up_open() has been executed. */
    if (true == is_opened)
    {
        return FW_UP_ERR_OPENED;
    }

    /* Initialize variable for store address of allocated memory */
    fw_up_memory_init();

    /* Initialize ring buffer */
    fw_up_buf_init();

    /* Set initialize complete flag */
    is_opened = true;

    return ret;
} /* End of function fw_up_open() */

/***********************************************************************************************************************
 * Function Name: fw_up_close
 * Description  : Initialize firmware update function.
 * Arguments    : none
 * Return Value : FW_UP_SUCCESS -
 *                   Processing completed successfully.
 *                FW_UP_ERR_NOT_OPEN -
 *                   fw_up_open has not been run.
 ***********************************************************************************************************************/
fw_up_return_t fw_up_close(void)
{
    fw_up_return_t ret = FW_UP_SUCCESS;

    /* Check that the fw_up_open() has been executed. */
    if (false == is_opened)
    {
        return FW_UP_ERR_NOT_OPEN;
    }

    /* Initialize ring buffer */
    fw_up_buf_init();

    /* Clear initialize complete flag */
    is_opened = false;

    return ret;
} /* End of function fw_up_close() */

/***********************************************************************************************************************
 * Function Name: erase_another_bank
 * Description  : Erase memory another bank.
 * Arguments    : none
 * Return Value : FW_UP_SUCCESS -
 *                   Processing completed successfully.
 *                FW_UP_ERR_ERASE -
 *                   Flash API(erase) error.
 *                FW_UP_ERR_NOT_BLANK -
 *                   erased area is not blank.
 ***********************************************************************************************************************/
fw_up_return_t erase_another_bank(void)
{
    fw_up_return_t ret = FW_UP_SUCCESS;
    flash_err_t ret_flash = FLASH_SUCCESS;

    uint32_t block_num;

    /* Calculate erase block number */
    /* Casting is valid because the width is a constant expressible with 32 bits. */
    block_num = ((((uint32_t)(FLASH_CF_BLOCK_38) - (uint32_t)FLASH_CF_BLOCK_45) / (uint32_t)FLASH_CF_SMALL_BLOCK_SIZE) + 1u);
    block_num += ((((uint32_t)FLASH_CF_BLOCK_46 - (uint32_t)FLASH_CF_LO_BANK_LO_ADDR) / (uint32_t)FLASH_CF_MEDIUM_BLOCK_SIZE) + 1u);

    /* Erase another bank */
    clrpsw_i();
    ret_flash = R_FLASH_Erase(FLASH_CF_BLOCK_38, block_num);
    setpsw_i();

    if (FLASH_SUCCESS != ret_flash)
    {
        ret = FW_UP_ERR_ERASE;
    }

    return ret;

} /* End of function erase_another_bank() */

/***********************************************************************************************************************
 * Function Name: analyze_and_write_data
 * Description  : Analyze mot s format and program data.
 * Arguments    : *p_recv_data -
 *                   mot file data.
 *                data_size
 *                   mot file data size.
 * Return Value : FW_UP_SUCCESS -
 *                   Processing completed successfully.
 *                FW_UP_ERR_WRITE -
 *                   Flash API(erase) error.
 ***********************************************************************************************************************/
fw_up_return_t analyze_and_write_data(const uint8_t *p_recv_data, uint32_t data_size)
{
    fw_up_return_t  ret1;
    fw_up_return_t  ret2;
    fw_up_return_t  ret3;
    fw_up_fl_data_t fl_data;
    fw_up_fl_data_t bin_data;
    int32_t status;

    /* Put received data in buffer */
    /* Casting is valid because the pointer value is 32 bits and the reference destination of the buffer is never changed */
    fl_data.src_addr = (uint32_t)p_recv_data;
    fl_data.len = data_size;
    fl_data.count = 0;

    do
    {
        /* Put received data in buffer */
        ret1 = fw_up_put_data(&fl_data);
        if (FW_UP_ERR_INVALID_RECORD == ret1)
        {
            return ret1;
        }
        ret3 = FW_UP_SUCCESS;
        do
        {
            /* Get Pointer to firmware data and data size, write address. */
            ret2 = fw_up_get_data(&bin_data);
            if (FW_UP_SUCCESS == ret2)
            {
                /* Casting is valid because the width is a constant expressible with 32 bits. */
                if (bin_data.dst_addr >= (uint32_t)FLASH_CF_HI_BANK_LO_ADDR)
                {

                    bin_data.dst_addr -= 0x100000u;

                    /* Write firmware to another bank */
                    ret3 = write_firmware(&bin_data);
                    if (FW_UP_SUCCESS == ret3)
                    {
                        /* Casting is valid because address values are converted to pointers */
                        status = memcmp((int8_t*)bin_data.dst_addr, (int8_t*)bin_data.src_addr, bin_data.len);
                        if (0 != status)
                        {
                            ret3 = FW_UP_ERR_WRITE;
                        }
                    }
                    else
                    {
                        ret3 = FW_UP_ERR_WRITE;
                    }
                }
            }
        }
        while ((FW_UP_SUCCESS == ret3) && (FW_UP_SUCCESS == ret2));
    }
    while (FW_UP_SUCCESS != ret1);

    return ret3;
} /* End of function analyze_and_write_data() */

/***********************************************************************************************************************
 * Function Name: bank_toggle
 * Description  : Changing Startup Bank.
 * Arguments    : none
 * Return Value : FW_UP_SUCCESS -
 *                   Successfully.
 *                FW_UP_ERR_WRITE -
 *                   Flash API(R_FLASH_Control, FLASH_CMD_BANK_TOGGLE) error.
 ***********************************************************************************************************************/
fw_up_return_t bank_toggle(void)
{
    fw_up_return_t ret;
    flash_err_t status_flash;

    clrpsw_i();
    status_flash = R_FLASH_Control(FLASH_CMD_BANK_TOGGLE, NULL);
    setpsw_i();
    if (FLASH_SUCCESS == status_flash)
    {
        ret = FW_UP_SUCCESS;
    }
    else
    {
        ret = FW_UP_ERR_WRITE;
    }
    return ret;
} /* End of function bank_toggle() */


/***********************************************************************************************************************
 * Function Name: fw_up_soft_reset
 * Description  : execute software reset.
 * Arguments    : none
 * Return Values: none
 ***********************************************************************************************************************/
void fw_up_soft_reset(void)
{
    /* Disable register write protect of SWRR */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);

    /* execute software reset by write SWRR */
    SYSTEM.SWRR = 0xA501;

    /* Infinite loop */
    while (true)
    {
        /* do nothing */
    }

} /* End of function fw_up_soft_reset() */

/***********************************************************************************************************************
 * Function Name: fw_up_check_reset_vector
 * Description  : Reset vector is checked valid.
 * Arguments    : none
 * Return Value : true -
 *                   Valid.
 *                false -
 *                   Invalid.
 ***********************************************************************************************************************/
bool fw_up_check_reset_vector(void)
{
    bool ret;

    /* Casting is valid because  address values are converted to pointers */
    const uint32_t *preset_vector_addr = (uint32_t*)((uint32_t)FLASH_CF_LO_BANK_HI_ADDR - (sizeof(uint32_t*) - 1u));
    uint32_t reset_vector_value = *preset_vector_addr;

    /* Casting is valid because the width is a constant expressible with 32 bits. */
    if ((FW_UP_BLANK_VALUE == reset_vector_value) || ((uint32_t)FLASH_CF_HI_BANK_LO_ADDR > reset_vector_value))
    {
        ret = false;
    }
    else
    {
        ret = true;
    }
    return ret;

} /* End of function fw_up_check_reset_vector */


/***********************************************************************************************************************
 * Function Name: write_firmware
 * Description  : Write firmware data.
 * Arguments    : p_fl_data -
 *                   Pointer to binary data of initial address, write address, data size.
 * Return Value : FW_UP_SUCCESS -
 *                   Processing completed successfully.
 *                FW_UP_ERR_NOT_OPEN -
 *                   R_FW_UP_Open has not been run.
 *                FW_UP_ERR_NULL_PTR -
 *                   The argument p_fl_data is null pointer.
 *                FW_UP_ERR_INTERNAL -
 *                   Flash API error.
 ***********************************************************************************************************************/
static fw_up_return_t write_firmware(fw_up_fl_data_t *p_fl_data)
{
    fw_up_return_t ret = FW_UP_SUCCESS;
    flash_err_t ret_flash;


    /* Check that the fw_up_open has been executed. */
    if (false == is_opened)
    {
        return FW_UP_ERR_NOT_OPEN;
    }

    /* Make sure that the argument is correct. */
    /* To cast in order to compare the address. There is no problem because the information is not lost even if the
     *  cast. */
    if (NULL == p_fl_data)
    {
        return FW_UP_ERR_NULL_PTR;
    }

    /* Write firmware data */
    clrpsw_i();
    ret_flash = R_FLASH_Write(p_fl_data->src_addr, p_fl_data->dst_addr, p_fl_data->len);
    setpsw_i();

    if (FLASH_SUCCESS != ret_flash)
    {
        ret = FW_UP_ERR_INTERNAL;
    }

    return ret;

} /* End of function write_firmware() */


/***********************************************************************************************************************
 * Function Name: fw_up_put_data
 * Description  : Put in firmware data to ring buffer
 * Arguments    : *p_fl_data -
 *                    Pointer to initial address and data size of firmware data.
 * Return Value : FW_UP_SUCCESS -
 *                    Processing completed successfully.
 *                FW_UP_ERR_NOT_OPEN -
 *                    fw_up_open has not been run.
 *                FW_UP_ERR_NULL_PTR -
 *                    The argument p_fl_data is null pointer.
 *                FW_UP_ERR_RING_BUF_FULL -
 *                    Ring buffer is not empty.
 *                FW_UP_ERR_INVALID_RECORD -
 *                    Motorola S record format data is invalid.
 *                FW_UP_ERR_INTERNAL -
 *                    Memory allocation is failed.
 ***********************************************************************************************************************/
static fw_up_return_t fw_up_put_data(fw_up_fl_data_t *p_fl_data)
{
    fw_up_return_t ret = FW_UP_SUCCESS;
    uint16_t cnt;
    uint8_t *pdata_tmp;

    /* Check that the fw_up_open() has been executed. */
    if (false == is_opened)
    {
        return FW_UP_ERR_NOT_OPEN;
    }

    /* Make sure that the argument is correct. */
    /* To cast in order to compare the address. There is no problem because the information is not lost even if the
     *  cast. */
    if (NULL == p_fl_data)
    {
        return FW_UP_ERR_NULL_PTR;
    }

    /* Set initial address of firmware data to be put in ring buffer */
    pdata_tmp = ((uint8_t *)p_fl_data->src_addr) + p_fl_data->count;

    for (cnt = p_fl_data->count; cnt < p_fl_data->len; cnt++)
    {
        /* Put firmware data in ring buffer */
        ret = fw_up_put_mot_s(*pdata_tmp);

        if (FW_UP_SUCCESS != ret)
        {
            /* Set values of data size to be put in ring buffer */
            p_fl_data->count = cnt;

            return ret;
        }

        pdata_tmp++;
    }

    return ret;

} /* End of function fw_up_put_data() */


/***********************************************************************************************************************
 * Function Name: fw_up_get_data
 * Description  : Stores firmware update data to ring buffer
 * Arguments    : *p_fl_data -
 *                   Pointer to binary data of initial address.
 * Return Value : FW_UP_SUCCESS -
 *                   Processing completed successfully.
 *                FW_UP_ERR_NOT_OPEN -
 *                   fw_up_open has not been run.
 *                FW_UP_ERR_NULL_PTR -
 *                   The argument p_fl_data is null pointer.
 *                FW_UP_ERR_RING_BUF_EMPTY -
 *                   Ring buffer is empty.
 ***********************************************************************************************************************/
static fw_up_return_t fw_up_get_data(fw_up_fl_data_t *p_fl_data)
{
    fw_up_return_t ret = FW_UP_SUCCESS;

    /* Check that the R_FW_UP_Open has been executed. */
    if (false == is_opened)
    {
        return FW_UP_ERR_NOT_OPEN;
    }

    /* Make sure that the argument is correct. */
    /* To cast in order to compare the address. There is no problem because the information is not lost even if the
     *  cast. */
    if (NULL == p_fl_data)
    {
        return FW_UP_ERR_NULL_PTR;
    }

    /* Get initial address and write address, data size of firmware data */
    ret = fw_up_get_binary(p_fl_data);

    return ret;
} /* End of function fw_up_get_data() */

