/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIESREGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_pinset.c
* Version      : 1.2.5
* Device(s)    : R5F563NBDxFP
* Tool-Chain   : RXC
* Description  : Setting of port and mpc registers
* Creation Date: 25 9 2018
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_pinset.h"
#include "platform.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: R_ETHER_PinSet_ETHERC_RMII
* Description  : This function initializes pins for r_ether_rx module
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void R_ETHER_PinSet_ETHERC_RMII()
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC);

    /* Set REF50CK pin */
    MPC.PB2PFS.BYTE = 0x12U;
    PORTB.PMR.BIT.B2 = 1U;

    /* Set RMII_TXD_EN pin */
    MPC.PB4PFS.BYTE = 0x12U;
    PORTB.PMR.BIT.B4 = 1U;

    /* Set RMII_TXD1 pin */
    MPC.PB6PFS.BYTE = 0x12U;
    PORTB.PMR.BIT.B6 = 1U;

    /* Set RMII_TXD0 pin */
    MPC.PB5PFS.BYTE = 0x12U;
    PORTB.PMR.BIT.B5 = 1U;

    /* Set RMII_RXD1 pin */
    MPC.PB0PFS.BYTE = 0x12U;
    PORTB.PMR.BIT.B0 = 1U;

    /* Set RMII_RXD0 pin */
    MPC.PB1PFS.BYTE = 0x12U;
    PORTB.PMR.BIT.B1 = 1U;

    /* Set RMII_RX_ER pin */
    MPC.PB3PFS.BYTE = 0x12U;
    PORTB.PMR.BIT.B3 = 1U;

    /* Set RMII_CRS_DV pin */
    MPC.PB7PFS.BYTE = 0x12U;
    PORTB.PMR.BIT.B7 = 1U;

    /* Set ET_MDC pin */
    MPC.PA4PFS.BYTE = 0x11U;
    PORTA.PMR.BIT.B4 = 1U;

    /* Set ET_MDIO pin */
    MPC.PA3PFS.BYTE = 0x11U;
    PORTA.PMR.BIT.B3 = 1U;

    /* Set ET_LINKSTA pin */
    MPC.PA5PFS.BYTE = 0x11U;
    PORTA.PMR.BIT.B5 = 1U;

    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC);
}

/***********************************************************************************************************************
* Function Name: R_SCI_PinSet_SCI0
* Description  : This function initializes pins for r_sci_rx module
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void R_SCI_PinSet_SCI0()
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC);

    /* Set RXD0/SMISO0 pin */
    MPC.P21PFS.BYTE = 0x0AU;
    PORT2.PMR.BIT.B1 = 1U;

    /* Set TXD0/SMOSI0 pin */
    MPC.P20PFS.BYTE = 0x0AU;
    PORT2.PMR.BIT.B0 = 1U;

    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC);
}

