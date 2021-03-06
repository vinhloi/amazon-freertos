<#--
  Copyright(C) 2016 Renesas Electronics Corporation
  RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY
  This program must be used solely for the purpose for which it was furnished 
  by Renesas Electronics Corporation. No part of this program may be reproduced
  or disclosed to others, in any form, without the prior written permission of 
  Renesas Electronics Corporation.
-->
<#-- = DECLARE FUNCTION INFORMATION HERE =================== -->
<#assign Function_Base_Name = "R_ADC_PinSet">
<#assign Function_Description = "This function initializes pins for r_s12ad_rx module">
<#assign Function_Arg = "none">
<#assign Function_Ret = "none">
<#assign Version = 1.00>

<#-- = DECLARE FUNCTION CONTENT HERE ======================= -->
<#macro initialsection postfix>
<#assign Function_Name = "${Function_Base_Name}${postfix}">
<#include "lib/functionheader.ftl">
void ${Function_Name}()
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC);
</#macro> 

<#macro peripheralpincode pin>
</#macro> 

<#macro channelpincode pin>

    <#if (pin.pinName?contains("ADTRG")?string("true","false")) == "true" || (pin.pinName?contains("ADST")?string("true","false")) == "true"  >
    /* Set ${pin.pinName} pin */
    MPC.${pin.assignedPinName}PFS.BYTE = 0x${pin.pinMPC}U;
    PORT${pin.portNum}.PMR.BIT.B${pin.pinBitNum} = 1U;
    <#else>
    /* Set ${pin.pinName} pin */
    <#if headerInfo.device?contains("R5F5110")>
        <#if pin.portNum != "4" && pin.portNum != "J" >
    PORT${pin.portNum}.PCR.BIT.B${pin.pinBitNum} = 0U;
        </#if>
    <#elseif headerInfo.device?contains("R5F5111")>
        <#if pin.portNum != "4" && pin.portNum != "H" && pin.portNum != "J" >
    PORT${pin.portNum}.PCR.BIT.B${pin.pinBitNum} = 0U;
        </#if>
    <#elseif headerInfo.device?contains("R5F5113")>
        <#if pin.portNum != "4" && pin.portNum != "9" && pin.portNum != "H" && pin.portNum != "J" >
    PORT${pin.portNum}.PCR.BIT.B${pin.pinBitNum} = 0U;
        </#if>
    <#else>
    PORT${pin.portNum}.PCR.BIT.B${pin.pinBitNum} = 0U;
    </#if>
    PORT${pin.portNum}.PDR.BIT.B${pin.pinBitNum} = 0U;
    PORT${pin.portNum}.PMR.BIT.B${pin.pinBitNum} = 0U;
    MPC.${pin.assignedPinName}PFS.BYTE = 0x${pin.pinMPC}U;
    </#if>
</#macro> 

<#macro endsection>

    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC);
}
</#macro> 

<#macro headerfilesection postfix>
void ${Function_Base_Name}${postfix}();
</#macro> 

<#-- = END OF FILE ========================================= -->