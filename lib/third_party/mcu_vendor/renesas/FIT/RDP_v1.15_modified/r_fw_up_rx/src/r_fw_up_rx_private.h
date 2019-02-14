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
 * Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_fw_up_rx_private.h
 * Version      : 1.00
 * Device       : RX651/RX65N
 * Description  : Firmware update API private header file
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 02.10.2017 1.00     First Release
 ***********************************************************************************************************************/

#ifndef R_FW_UP_RX_PRIVATE_H_
#define R_FW_UP_RX_PRIVATE_H_

/***********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 ***********************************************************************************************************************/
/* Interface information for this package. */
#include "r_fw_up_rx_if.h"

/* Interface information for Flash API */
#include "r_flash_rx_if.h"

/***********************************************************************************************************************
 Macro definitions
 ***********************************************************************************************************************/
/* Write buffer size */
#define FW_UP_BINARY_BUF_SIZE (256u)

/* Number of write buffers */
#define FW_UP_BINARY_BUF_NUM (2u)

/* Number of buffer for put in firmware data */
#define FW_UP_BUF_NUM (60u)

/* The value of blank of code flash */
#define FW_UP_BLANK_VALUE (0xFFFFFFFFu)

#endif /* R_FW_UP_RX_PRIVATE_H_ */
