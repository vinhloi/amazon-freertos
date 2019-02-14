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
 * File Name    : r_fw_up_buf.h
 * Version      : 1.00
 * Description  : Firmware update buffer process header file
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.07.2017 1.00     First Release
 ***********************************************************************************************************************/

#ifndef R_FW_UP_BUF_H_
#define R_FW_UP_BUF_H_

/***********************************************************************************************************************
 Includes <System Includes> , "Project Includes"
 ***********************************************************************************************************************/
#include <stdint.h>
#include "r_fw_up_rx_if.h"
#include "r_fw_up_rx_private.h"

/***********************************************************************************************************************
 Macro definitions
 ***********************************************************************************************************************/
/* number of byte check sum field in Motorola s record */
#define MOT_S_CHECK_SUM_FIELD (0x02)

/* Address length of S1 format */
#define ADDRESS_LENGTH_S1 (0x04)

/* Address length of S2 format */
#define ADDRESS_LENGTH_S2 (0x06)

/* Address length of S3 format */
#define ADDRESS_LENGTH_S3 (0x08)

/* State of ring buffer */
#define BUF_LOCK ((uint8_t)1)
#define BUF_UNLOCK ((uint8_t)0)

/***********************************************************************************************************************
 Typedef definitions
 ***********************************************************************************************************************/
/* Analysis step of Motorola S record format */
typedef enum fw_up_mot_s_cnt_t
{
    STATE_MOT_S_RECORD_MARK = 0,
    STATE_MOT_S_RECORD_TYPE,
    STATE_MOT_S_LENGTH_1,
    STATE_MOT_S_LENGTH_2,
    STATE_MOT_S_ADDRESS,
    STATE_MOT_S_DATA,
    STATE_MOT_S_CHKSUM_1,
    STATE_MOT_S_CHKSUM_2
} fw_up_mot_s_cnt_t;

/* buffer structure for Motorola s record format */
typedef struct MotSBufS
{
    uint8_t addr_length;
    uint8_t data_length;
    uint8_t *paddress;
    uint8_t *pdata;
    uint8_t type;
    uint8_t act;
    struct MotSBufS *pnext;
} fw_up_mot_s_buf_t;

/* buffer structure for write data */
typedef struct WriteDataS
{
    uint32_t addr;
    uint32_t len;
    uint8_t data[FW_UP_BINARY_BUF_SIZE];
    struct WriteDataS *pnext;
    struct WriteDataS *pprev;
} fw_up_write_data_t;

/***********************************************************************************************************************
 Exported global variables and functions (to be accessed by other files)
 ***********************************************************************************************************************/
void fw_up_buf_init(void);
void fw_up_memory_init(void);
fw_up_return_t fw_up_put_mot_s(uint8_t mot_s_data);
fw_up_return_t fw_up_get_binary(fw_up_fl_data_t *p_fl_data);


#endif /* R_FW_UP_BUF_H_ */
