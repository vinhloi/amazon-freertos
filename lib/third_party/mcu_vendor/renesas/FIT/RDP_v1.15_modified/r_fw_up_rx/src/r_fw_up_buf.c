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
 * File Name    : r_fw_up_buf.c
 * Version      : 1.00
 * Description  : Firmware update buffer process file
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.07.2017 1.00     First Release
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 Includes <System Includes> , "Project Includes"
 ***********************************************************************************************************************/
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "r_fw_up_rx_if.h"
#include "r_fw_up_buf.h"
#include "r_fw_up_rx_private.h"

/***********************************************************************************************************************
 Private variables and functions
 ***********************************************************************************************************************/
static fw_up_mot_s_buf_t *papp_put_mot_s_buf = NULL;
static fw_up_mot_s_buf_t *papp_get_mot_s_buf = NULL;
static fw_up_mot_s_buf_t mot_s_buf[FW_UP_BUF_NUM];

static fw_up_write_data_t *papp_write_buf = NULL;
static fw_up_write_data_t write_buf[FW_UP_BINARY_BUF_NUM];

static fw_up_mot_s_cnt_t mot_s_data_state = STATE_MOT_S_RECORD_MARK;

static uint32_t write_current_address;

static bool detect_terminal_flag = false;

static bool fw_up_ascii_to_hexbyte(uint8_t in_upper, uint8_t in_lower, uint8_t *p_hex_buf);

/***********************************************************************************************************************
 * Function Name: fw_up_buf_init
 * Description  : Initialize buffer.
 * Arguments    : none
 * Return Value : none
 ***********************************************************************************************************************/
void fw_up_buf_init(void)
{
    fw_up_mot_s_buf_t *pmot_s_buf;
    fw_up_write_data_t *pwrite_buf;
    uint32_t i;

    for (i = 0; i < FW_UP_BINARY_BUF_NUM; i++)
    {
        pwrite_buf = (fw_up_write_data_t *) &write_buf[i];

        /* Clear write address */
        pwrite_buf->addr = 0x00000000;

        /* Clear data length in buffer */
        pwrite_buf->len = 0;

        /* Clear data buffer */
        (void)memset((pwrite_buf->data), (0xFF), (FW_UP_BINARY_BUF_SIZE));

        /* Set next buffer address */
        pwrite_buf->pnext = (fw_up_write_data_t *)&write_buf[i + 1u];

        /* Set previous buffer address */
        pwrite_buf->pprev = (fw_up_write_data_t *)&write_buf[i - 1u];
    }

    /* Set next buffer address of last buffer is first buffer */
    pwrite_buf->pnext = (fw_up_write_data_t *)&write_buf[0];

    /* Set previous buffer address of first buffer is last buffer */
    write_buf[0].pprev = pwrite_buf;

    /* Set currently buffer in use is first buffer */
    papp_write_buf = (fw_up_write_data_t *)&write_buf[0];

    /* Set currently write address is initial address of option setting memory */
    write_current_address = 0u;

    /* Set write address of currently buffer in use is "write_current_address" */
    papp_write_buf->addr = write_current_address;

    for (i = 0; i < FW_UP_BUF_NUM; i++)
    {
        pmot_s_buf = (fw_up_mot_s_buf_t *)&mot_s_buf[i];

        /* Clear record type */
        pmot_s_buf->type = 0;

        /* Clear data length */
        pmot_s_buf->data_length = 0;

        /* Clear address length */
        pmot_s_buf->addr_length = 0;

        /* Clear initial address of allocated memory for address */
        if (NULL != pmot_s_buf->paddress)
        {
        	vPortFree(pmot_s_buf->paddress);
            pmot_s_buf->paddress = NULL;
        }

        /* Clear initial address of allocated memory for data */
        if (NULL != pmot_s_buf->pdata)
        {
        	vPortFree(pmot_s_buf->pdata);
            pmot_s_buf->pdata = NULL;
        }

        /* Clear lock flag */
        pmot_s_buf->act = BUF_UNLOCK;

        pmot_s_buf->pnext = (fw_up_mot_s_buf_t *)&mot_s_buf[i + 1u];
    }

    /* Set next buffer address of last buffer is first buffer */
    pmot_s_buf->pnext = (fw_up_mot_s_buf_t *)&mot_s_buf[0];

    /* Set currently buffer in use is first buffer */
    papp_put_mot_s_buf = (fw_up_mot_s_buf_t *)&mot_s_buf[0];
    papp_get_mot_s_buf = (fw_up_mot_s_buf_t *)&mot_s_buf[0];

    /* Clear analysis state of Motorola s record format */
    mot_s_data_state = STATE_MOT_S_RECORD_MARK;

    /* Clear detect terminal record flag */
    detect_terminal_flag = false;

} /* End of function fw_up_buf_init() */

/***********************************************************************************************************************
 * Function Name: fw_up_memory_init
 * Description  : Initialize variable for store address of allocated memory.
 * Arguments    : none
 * Return Value : none
 ***********************************************************************************************************************/
void fw_up_memory_init(void)
{
    fw_up_mot_s_buf_t *pmot_s_buf;
    uint32_t i;

    for (i = 0; i < FW_UP_BUF_NUM; i++)
    {
        pmot_s_buf = (fw_up_mot_s_buf_t *) &mot_s_buf[i];

        /* Clear initial address of allocated memory for address */
        pmot_s_buf->paddress = NULL;

        /* Clear initial address of allocated memory for data */
        pmot_s_buf->pdata = NULL;

        pmot_s_buf->pnext = (fw_up_mot_s_buf_t *) &mot_s_buf[i + 1u];
    }

} /* End of function fw_up_memory_init() */

/***********************************************************************************************************************
 * Function Name: fw_up_put_mot_s
 * Description  : Put in ring buffer and analysis Motorola s record format data.
 * Arguments    : mot_s_data -
 *                  Motorola s record format data
 * Return Value : FW_UP_SUCCESS -
 *                  Processing completed successfully.
 *                FW_UP_ERR_BUF_FULL -
 *                  Ring buffer is not empty.
 *                FW_UP_ERR_INVALID_RECORD -
 *                  Motorola S record format data is invalid.
 *                FW_UP_ERR_INTERNAL -
 *                  Memory allocation is failed.
 ***********************************************************************************************************************/
fw_up_return_t fw_up_put_mot_s(uint8_t mot_s_data)
{
    fw_up_return_t ret = FW_UP_SUCCESS;

    static uint32_t cnt;
    static uint32_t sum_value;
    static uint8_t mot_buf;
    static uint8_t *pwork_addr;
    uint8_t len_buf;
    uint8_t chk_sum;
    bool chk_ascii;

    if (BUF_LOCK == papp_put_mot_s_buf->act)
    {
        return FW_UP_ERR_BUF_FULL;
    }

    if (true == detect_terminal_flag)
    {
        return FW_UP_SUCCESS;
    }

    switch (mot_s_data_state)
    {
        case STATE_MOT_S_RECORD_MARK:

            /* Header field (upper 4bits) */
            /* Casting is valid because the width is a constant expressible with 8 bits. */
            if ((uint8_t)'S' == mot_s_data)
            {
                /* Update analysis state */
                mot_s_data_state = STATE_MOT_S_RECORD_TYPE;
            }
            /* Casting is valid because the width is a constant expressible with 8 bits. */
            else if (((uint8_t)'\n' == mot_s_data) || ((uint8_t)'\r' == mot_s_data))
            {
                /* Do nothing, ignore CR+LF */
            }
            else
            {
                ret = FW_UP_ERR_INVALID_RECORD;
            }
        break;

        case STATE_MOT_S_RECORD_TYPE:

            /* Header field (lower 4bits) */
            /* Convert to hex byte from ASCII data */
            /* Casting is valid because the width is a constant expressible with 8 bits. */
            chk_ascii = fw_up_ascii_to_hexbyte((uint8_t)'0', mot_s_data, &(papp_put_mot_s_buf->type));
            if (false == chk_ascii)
            {
                return FW_UP_ERR_INVALID_RECORD;
            }

            /* Update analysis state */
            mot_s_data_state = STATE_MOT_S_LENGTH_1;
        break;

        case STATE_MOT_S_LENGTH_1:

            /* Length field (upper 4bits) */
            /* Put in buffer */
            mot_buf = mot_s_data;

            /* Clear check sum compare value */
            sum_value = 0;

            /* Update analysis state */
            mot_s_data_state = STATE_MOT_S_LENGTH_2;
        break;

        case STATE_MOT_S_LENGTH_2:

            /* Length field (lower 4bits) */
            chk_ascii = fw_up_ascii_to_hexbyte(mot_buf, mot_s_data, &len_buf);
            if (false == chk_ascii)
            {
                return FW_UP_ERR_INVALID_RECORD;
            }

            /* Check address length */
            /* Casting is valid because the width is a constant expressible with 8 bits. */
            if ((((uint8_t)0 == papp_put_mot_s_buf->type) || ((uint8_t)1 == papp_put_mot_s_buf->type))
                    || ((uint8_t)9 == papp_put_mot_s_buf->type))
            {
                papp_put_mot_s_buf->addr_length = (ADDRESS_LENGTH_S1 / 2);
            }
            /* Casting is valid because the width is a constant expressible with 8 bits. */
            else if (((uint8_t)2 == papp_put_mot_s_buf->type) || ((uint8_t)8 == papp_put_mot_s_buf->type))
            {
                papp_put_mot_s_buf->addr_length = (ADDRESS_LENGTH_S2 / 2);
            }
            /* Casting is valid because the width is a constant expressible with 8 bits. */
            else if (((uint8_t)3 == papp_put_mot_s_buf->type) || ((uint8_t)7 == papp_put_mot_s_buf->type))
            {
                papp_put_mot_s_buf->addr_length = (ADDRESS_LENGTH_S3 / 2);
            }
            else
            {
                return FW_UP_ERR_INVALID_RECORD;
            }

            /* Allocate memory of the address data */
            /* Casting is valid because conversion of void pointer to concrete type pointer. */
            papp_put_mot_s_buf->paddress = pvPortMalloc(papp_put_mot_s_buf->addr_length);
            if (NULL == papp_put_mot_s_buf->paddress)
            {
                return FW_UP_ERR_INTERNAL;
            }

            /* Calculate data length */
            /* Casting is valid because the width is a constant expressible with 8 bits. */
            papp_put_mot_s_buf->data_length = (len_buf
                    - (papp_put_mot_s_buf->addr_length + (uint8_t)(MOT_S_CHECK_SUM_FIELD / 2)));

            /* Allocate memory of the data */
            if ((uint8_t)0 != papp_put_mot_s_buf->data_length)
            {
                /* Casting is valid because conversion of void pointer to concrete type pointer. */
                papp_put_mot_s_buf->pdata = pvPortMalloc(papp_put_mot_s_buf->data_length);
                if (NULL == papp_put_mot_s_buf->pdata)
                {
                    return FW_UP_ERR_INTERNAL;
                }
            }

            /* Copy initial address of address data */
            pwork_addr = papp_put_mot_s_buf->paddress;

            /* Add length value to check sum value */
            sum_value += len_buf;

            /* Clear 2bytes convert counter */
            cnt = 0;

            /* Update analysis state */
            mot_s_data_state = STATE_MOT_S_ADDRESS;
        break;

        case STATE_MOT_S_ADDRESS:

            /* Length field */
            if (1u == (cnt % 2u))
            {
                chk_ascii = fw_up_ascii_to_hexbyte(mot_buf, mot_s_data, pwork_addr);
                if (false == chk_ascii)
                {
                    return FW_UP_ERR_INVALID_RECORD;
                }

                /* Add address data to check sum value */
                sum_value += (*pwork_addr);

                /* Increment address */
                pwork_addr++;
            }

            /* Increment 2bytes convert counter */
            cnt++;

            /* put in buffer */
            mot_buf = mot_s_data;

            /* Casting is valid because conversion to wide type. */
            if (((uint32_t)papp_put_mot_s_buf->addr_length * 2u) <= cnt)
            {
                /* Clear 2bytes convert counter */
                cnt = 0;

                /* Copy initial address of data */
                pwork_addr = papp_put_mot_s_buf->pdata;

                /* Update analysis state */
                if ((uint8_t)0 != papp_put_mot_s_buf->data_length)
                {
                    mot_s_data_state = STATE_MOT_S_DATA;
                }
                else
                {
                    mot_s_data_state = STATE_MOT_S_CHKSUM_1;
                }
            }
        break;

        case STATE_MOT_S_DATA:

            /* Data field */
            if (1u == (cnt % 2u))
            {
                chk_ascii = fw_up_ascii_to_hexbyte(mot_buf, mot_s_data, pwork_addr);
                if (false == chk_ascii)
                {
                    return FW_UP_ERR_INVALID_RECORD;
                }

                /* Add data to check sum value */
                sum_value += (*pwork_addr);

                /* Increment address */
                pwork_addr++;
            }

            /* Increment 2bytes convert counter */
            cnt++;

            /* Put in buffer */
            mot_buf = mot_s_data;

            /* Casting is valid because conversion to wide type. */
            if (((uint32_t)papp_put_mot_s_buf->data_length * 2u) <= cnt)
            {
                /* Clear 2bytes convert counter */
                cnt = 0;

                /* Update analysis state */
                mot_s_data_state = STATE_MOT_S_CHKSUM_1;
            }
        break;

        case STATE_MOT_S_CHKSUM_1:

            /* Check sum field (upper 4bits) */
            /* Put in buffer */
            mot_buf = mot_s_data;

            /* Update analysis state */
            mot_s_data_state = STATE_MOT_S_CHKSUM_2;
        break;

        case STATE_MOT_S_CHKSUM_2:
            chk_ascii = fw_up_ascii_to_hexbyte(mot_buf, mot_s_data, &chk_sum);
            if (false == chk_ascii)
            {
                return FW_UP_ERR_INVALID_RECORD;
            }

            /* Compare check sum value */
            /* Casting is valid because higher 24 bits is not used. */
            if ((uint8_t)((~sum_value) & 0x000000FFu) != chk_sum)
            {
                return FW_UP_ERR_INVALID_RECORD;
            }

            /* Lock currently buffer in use */
            papp_put_mot_s_buf->act = BUF_LOCK;

            /* Check address length */
            /* Casting is valid because the width is a constant expressible with 8 bits. */
            if ((((uint8_t)7 == papp_put_mot_s_buf->type) || ((uint8_t)8 == papp_put_mot_s_buf->type))
                    || ((uint8_t)9 == papp_put_mot_s_buf->type))
            {
                /* Decect terminal record */
                detect_terminal_flag = true;
            }

            /* Set currently buffer in use is next buffer */
            papp_put_mot_s_buf = papp_put_mot_s_buf->pnext;

            /* Clear analysis state */
            mot_s_data_state = STATE_MOT_S_RECORD_MARK;
        break;

        default:
            ;
        break;
    }

    return ret;

} /* End of function fw_up_put_mot_s() */

/***********************************************************************************************************************
 * Function Name: fw_up_get_binary
 * Description  : Put in ring buffer and analysis Motorola s record format data.
 * Arguments    : *p_fl_data -
 *                  Pointer to initial address, write address, data size
 * Return Value : FW_UP_SUCCESS -
 *                    Processing completed successfully.
 *                FW_UP_ERR_BUF_EMPTY -
 *                    Ring buffer is empty.
 ***********************************************************************************************************************/
fw_up_return_t fw_up_get_binary(fw_up_fl_data_t *p_fl_data)
{
    fw_up_return_t ret = FW_UP_SUCCESS;

    fw_up_write_data_t *pprev_write_buf;
    fw_up_write_data_t *plocal_write_buf;

    uint32_t addr_buf = 0;
    uint32_t put_number;
    uint32_t i;
    uint8_t *pwork_addr;

    pwork_addr = NULL;

    pprev_write_buf = papp_write_buf->pprev;

    pprev_write_buf->len = 0;
    (void)memset((pprev_write_buf->data), (0xFF), (FW_UP_BINARY_BUF_SIZE));

    while (FW_UP_BINARY_BUF_SIZE > papp_write_buf->len)
    {
        if (BUF_UNLOCK == papp_get_mot_s_buf->act)
        {
            if (true == detect_terminal_flag)
            {
                if (0u != papp_write_buf->len)
                {
                    papp_write_buf->len = FW_UP_BINARY_BUF_SIZE;
                    break;
                }
                else
                {
                    return FW_UP_ERR_BUF_EMPTY;
                }
            }
            else
            {
                return FW_UP_ERR_BUF_EMPTY;
            }
        }

        /* Check data record */
        /* Casting is valid because the width is a constant expressible with 8 bits. */
        if ((((uint8_t)1 == papp_get_mot_s_buf->type) || ((uint8_t)2 == papp_get_mot_s_buf->type)) || ((uint8_t)3 == papp_get_mot_s_buf->type))
        {

            pwork_addr = papp_get_mot_s_buf->paddress;

            addr_buf = 0;
            for (i = 0; i < papp_get_mot_s_buf->addr_length; i++)
            {
                addr_buf = (addr_buf << 8u);
                addr_buf |= (*pwork_addr);

                /* Increment address */
                pwork_addr++;
            }

            if (0u == papp_write_buf->len)
            {
                /* Calculate currently write address */
                write_current_address = (addr_buf / FW_UP_BINARY_BUF_SIZE) * FW_UP_BINARY_BUF_SIZE;

                /* Set currently write address to write address in buffer */
                papp_write_buf->addr = write_current_address;

                /* Set difference of less than write size to data length */
                papp_write_buf->len = addr_buf % FW_UP_BINARY_BUF_SIZE;
            }
            else
            {
                /* Set difference between currently write address and write address in buffer to data length */
                papp_write_buf->len = addr_buf - papp_write_buf->addr;
            }

            if (FW_UP_BINARY_BUF_SIZE <= papp_write_buf->len)
            {
                papp_write_buf->len = FW_UP_BINARY_BUF_SIZE;
                break;
            }

            put_number = FW_UP_BINARY_BUF_SIZE - papp_write_buf->len;

            if (put_number >= papp_get_mot_s_buf->data_length)
            {
                pwork_addr = papp_get_mot_s_buf->pdata;

                for (i = 0; i < papp_get_mot_s_buf->data_length; i++)
                {
                    papp_write_buf->data[i + papp_write_buf->len] = *pwork_addr;
                    pwork_addr++;
                }

                papp_write_buf->len += papp_get_mot_s_buf->data_length;
            }
            else
            {
                pwork_addr = papp_get_mot_s_buf->pdata;

                for (i = 0; i < put_number; i++)
                {
                    papp_write_buf->data[i + papp_write_buf->len] = *pwork_addr;
                    pwork_addr++;
                }

                papp_write_buf->len += put_number;

                plocal_write_buf = papp_write_buf->pnext;

                for (i = 0; i < (papp_get_mot_s_buf->data_length - put_number); i++)
                {
                    plocal_write_buf->data[i] = *pwork_addr;
                    pwork_addr++;
                }

                plocal_write_buf->len = i;
            }
        }

        /* Release address and data memory */
        vPortFree(papp_get_mot_s_buf->paddress);
        vPortFree(papp_get_mot_s_buf->pdata);

        /* Set NULL */
        papp_get_mot_s_buf->paddress = NULL;
        papp_get_mot_s_buf->pdata = NULL;

        /* Release buffer */
        papp_get_mot_s_buf->act = BUF_UNLOCK;

        /* Set currently buffer in use is next buffer */
        papp_get_mot_s_buf = papp_get_mot_s_buf->pnext;
    }

    /* Set data address, write address, data length to argument */
    /* Casting is valid because the width of address value is 32bits. */
    p_fl_data->src_addr = (uint32_t) papp_write_buf->data;
    p_fl_data->dst_addr = papp_write_buf->addr;
    p_fl_data->len = papp_write_buf->len;

    /* Set currently write buffer in use is next buffer */
    papp_write_buf = papp_write_buf->pnext;

    /* Increase currently write address */
    write_current_address += FW_UP_BINARY_BUF_SIZE;

    /* Set currently write address to write buffer */
    papp_write_buf->addr = write_current_address;

    return ret;

} /* End of function fw_up_get_binary() */

/***********************************************************************************************************************
 * Function Name: fw_up_ascii_to_hexbyte
 * Description  : Convert 2 bytes of ASCII data to 1 byte of hexadecimal.
 * Arguments    : in_upper -
 *                  upper data for ASCII.
 *                in_lower -
 *                  lower data for ASCII.
 *                *p_hex_buf -
 *                  Pointer to store converted 1 byte of hexadecimal
 * Return Value : true -
 *                  Processing completed successfully.
 *                false -
 *                  ASCII data is invalid.
 ***********************************************************************************************************************/
static bool fw_up_ascii_to_hexbyte(uint8_t in_upper, uint8_t in_lower, uint8_t *p_hex_buf)
{
    bool ret = true;

    /* Convert upper 4 bits */
    /* Casting is valid because the width is a constant expressible with 8 bits. */
    if (((uint8_t)0x30 <= in_upper) && ((uint8_t)0x39 >= in_upper))
    {
        /* Casting is valid because the width is a constant expressible with 8 bits. */
        in_upper = (in_upper & (uint8_t)0x0F);
    }
    /* Casting is valid because the width is a constant expressible with 8 bits. */
    else if (((uint8_t)0x41 <= in_upper) && ((uint8_t)0x46 >= in_upper))
    {
        /* Casting is valid because the width is a constant expressible with 8 bits. */
        in_upper = (in_upper - (uint8_t)0x37);
    }
    /* Casting is valid because the width is a constant expressible with 8 bits. */
    else if (((uint8_t)0x61 <= in_upper) && ((uint8_t)0x66 >= in_upper))
    {
        /* Casting is valid because the width is a constant expressible with 8 bits. */
        in_upper = (in_upper - (uint8_t)0x57);
    }
    else
    {
        ret = false;
    }

    /* Convert lower 4 bits */
    /* Casting is valid because the width is a constant expressible with 8 bits. */
    if (((uint8_t)0x30 <= in_lower) && ((uint8_t)0x39 >= in_lower))
    {
        /* Casting is valid because the width is a constant expressible with 8 bits. */
        in_lower = (in_lower & (uint8_t)0x0F);
    }
    /* Casting is valid because the width is a constant expressible with 8 bits. */
    else if (((uint8_t)0x41 <= in_lower) && ((uint8_t)0x46 >= in_lower))
    {
        /* Casting is valid because the width is a constant expressible with 8 bits. */
        in_lower = (in_lower - (uint8_t)0x37);
    }
    /* Casting is valid because the width is a constant expressible with 8 bits. */
    else if (((uint8_t)0x61 <= in_lower) && ((uint8_t)0x66 >= in_lower))
    {
        /* Casting is valid because the width is a constant expressible with 8 bits. */
        in_lower = (in_lower - (uint8_t)0x57);
    }
    else
    {
        ret = false;
    }

    if (true == ret)
    {
        /* Combine upper and lower */
        *p_hex_buf = (in_upper << 4) | in_lower;
    }

    return ret;
} /* End of function fw_up_ascii_to_hexbyte() */
