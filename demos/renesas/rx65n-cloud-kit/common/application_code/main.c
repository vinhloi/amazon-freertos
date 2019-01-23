/*
Amazon FreeRTOS
Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 http://aws.amazon.com/freertos
 http://www.FreeRTOS.org
*/

#include <stdio.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

/* Version includes. */
#include "aws_application_version.h"

/* System init includes. */
#include "aws_system_init.h"

/* Logging includes. */
#include "aws_logging_task.h"

/* Key provisioning includes. */
#include "aws_dev_mode_key_provisioning.h"

/* TCP/IP abstraction includes. */
#include "aws_secure_sockets.h"

/* WiFi interface includes. */
#include "aws_wifi.h"

/* Client credential includes. */
#include "aws_clientcredential.h"

/* Demo includes */
#include "aws_demo_runner.h"

#define mainLOGGING_TASK_STACK_SIZE         ( configMINIMAL_STACK_SIZE * 6 )
#define mainLOGGING_MESSAGE_QUEUE_LENGTH    ( 15 )
#define mainTEST_RUNNER_TASK_STACK_SIZE    ( configMINIMAL_STACK_SIZE * 8 )

/* Declare the firmware version structure for all to see. */
const AppVersion32_t xAppFirmwareVersion = {
   .u.x.ucMajor = APP_VERSION_MAJOR,
   .u.x.ucMinor = APP_VERSION_MINOR,
   .u.x.usBuild = APP_VERSION_BUILD,
};

/* The MAC address array is not declared const as the MAC address will
normally be read from an EEPROM and not hard coded (in real deployed
applications).*/
static uint8_t ucMACAddress[ 6 ] =
{
    configMAC_ADDR0,
    configMAC_ADDR1,
    configMAC_ADDR2,
    configMAC_ADDR3,
    configMAC_ADDR4,
    configMAC_ADDR5
}; //XXX

/* Define the network addressing.  These parameters will be used if either
ipconfigUDE_DHCP is 0 or if ipconfigUSE_DHCP is 1 but DHCP auto configuration
failed. */
static const uint8_t ucIPAddress[ 4 ] =
{
    configIP_ADDR0,
    configIP_ADDR1,
    configIP_ADDR2,
    configIP_ADDR3
};
static const uint8_t ucNetMask[ 4 ] =
{
	configNET_MASK0,
	configNET_MASK1,
	configNET_MASK2,
	configNET_MASK3
};
static const uint8_t ucGatewayAddress[ 4 ] =
{
	configGATEWAY_ADDR0,
	configGATEWAY_ADDR1,
	configGATEWAY_ADDR2,
	configGATEWAY_ADDR3
};

/* The following is the address of an OpenDNS server. */
static const uint8_t ucDNSServerAddress[ 4 ] =
{
	configDNS_SERVER_ADDR0,
	configDNS_SERVER_ADDR1,
	configDNS_SERVER_ADDR2,
	configDNS_SERVER_ADDR3
};

/**
 * @brief Application task startup hook.
 */
void vApplicationDaemonTaskStartupHook( void );

/**
 * @brief Connects to WiFi.
 */
static void prvWifiConnect( void );

/**
 * @brief Initializes the board.
 */
static void prvMiscInitialization( void );
/*-----------------------------------------------------------*/

/**
 * @brief Application runtime entry point.
 */
void main( void )
{
    /* Perform any hardware initialization that does not require the RTOS to be
     * running.  */

    /* Start the scheduler.  Initialization that requires the OS to be running,
     * including the WiFi initialization, is performed in the RTOS daemon task
     * startup hook. */
    //vTaskStartScheduler();

    while(1)
    {
     	vTaskDelay(10000);
    }
}
/*-----------------------------------------------------------*/

static void prvMiscInitialization( void )
{
    /* FIX ME. */
    uart_config();
    configPRINT_STRING(("Hello World.\r\n"));
    /* Start logging task. */
    xLoggingTaskInitialize( mainLOGGING_TASK_STACK_SIZE,
                            tskIDLE_PRIORITY,
                            mainLOGGING_MESSAGE_QUEUE_LENGTH );
}
/*-----------------------------------------------------------*/

#include "r_bsp.h"

EventGroupHandle_t xUserSwitchEventGroup;

#pragma interrupt  userSwitchISR(vect=VECT(ICU,IRQ1))

void userSwitchISR(void) {

    BaseType_t xHigherPriorityTaskWoken = pdTRUE;
    BaseType_t xResult;

    ///TODO: xEventGroupSetBitsFromISR does not work properly.

//    xResult = xEventGroupSetBitsFromISR(xUserSwitchEventGroup, /* The event group being updated. */
//            (1 << 0), /* The bits being set. */
//    &xHigherPriorityTaskWoken);

    xResult = xEventGroupSetBits(xUserSwitchEventGroup, /* The event group being updated. */
            (1 << 0) /* The bits being set. */);

//    /* Was the message posted successfully? */
//    if (xResult != pdFAIL) {
//        /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context
//         switch should be requested.  The macro used is port specific and will
//         be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - refer to
//         the documentation page for the port being used. */
//        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//    }

}

void prvUserSwitchISRInitialization(){

    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC);

    IEN(ICU,IRQ1)= 0;

    MPC.P31PFS.BIT.ISEL = 1;

    ICU.IRQFLTE0.BIT.FLTEN1 = 0;
    ICU.IRQFLTC0.BIT.FCLKSEL1 = 0;

    ICU.IRQCR[1].BIT.IRQMD = 3;
    IPR(ICU,IRQ1)= 15;
    IR(ICU,IRQ1)= 0;
    ICU.IRQFLTE0.BIT.FLTEN1 = 1;
    IEN(ICU,IRQ1)= 1;

    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC);

}

void prvUserSwitchISRTermination(){

    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC);

    IEN(ICU,IRQ1)= 0;

    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC);

}

void prvGetStringFromTerminal(char *pString, size_t num) {

    uint32_t pointer = 0;

    while (pointer < num) {
        scanf("%c", (pString + pointer));

        if (pString[pointer] == '\b') {

//            printf(" ");
//            printf("\b");

            if (pointer > 0) {
                printf(" \b");
                pointer--;
            }else{
                printf(" ");
            }
            continue;

        } else if (pString[pointer] == '\n') {
            pString[pointer] = '\0';
            break;
        }
        pointer++;
    }

}

#define MAX_SSID_CHARACTER_NUM  (32)
#define MAX_PASSPHRASE_CHARACTER_NUM  (64)
#define MAX_SERVER_ADDRESS_CHARACTER_NUM  (64)
#define MAX_DEVICE_NAME_CHARACTER_NUM  (64)
#define MAX_CERTIFICATE_CHARACTER_NUM  (64)

#define SAMPLE_CERTIFICATE_FORMAT           "-----BEGIN CERTIFICATE-----\n"\
        "YMPGn8u67GB9t+aEMr5P+1gmIgNb1LTV+/Xjli5wwOQuvfwu7uJBVcA0Ln0kcmnL\n"\
        "R7EUQIN9Z/SG9jGr8XmksrUuEvmEF/Bibyc+E1ixVA0hmnM3oTDPb5Lc9un8rNsu\n"\
        "KNF+AksjoBXyOGVkCeoMbo4bF6BxyLObyavpw/LPh5aPgAIynplYb6LVAgMBAAGj\n"\
        ".......\n"\
        "-----END CERTIFICATE-----\n\n\n"
#define CERTIFICATE_BEGIN_FORMAT "-----BEGIN CERTIFICATE-----\n"
#define CERTIFICATE_END_FORMAT   "-----END CERTIFICATE-----"

void prvConnectivityConfigurationUpdate() {

    uint8_t str_ssid[MAX_SSID_CHARACTER_NUM + 1];
    uint8_t str_passphrase[MAX_PASSPHRASE_CHARACTER_NUM + 1];
    uint8_t str_sertificate[MAX_CERTIFICATE_CHARACTER_NUM + 1];
    uint8_t str_server_address[MAX_SERVER_ADDRESS_CHARACTER_NUM + 1];
    uint8_t str_device_name[MAX_DEVICE_NAME_CHARACTER_NUM + 1];
    uint32_t current_str_pointer;
    uint8_t answer_char;
    uint32_t security_mode;

    printf("Do you want to configure the Wifi credentials ? (y/n)"); ///TODO Fix messeage.

    scanf("%1s", &answer_char);

    printf("\n"); ///TODO Fix messeage.

    if (answer_char == 'y') {

        memset(str_ssid, '\0', MAX_SSID_CHARACTER_NUM + 1);

        printf("Enter SSID:"); ///TODO Fix messeage.

        prvGetStringFromTerminal(str_ssid, 32);

        printf("\n"); ///TODO Fix messeage.

        printf("You have enterd %s as the ssid.\n\n", str_ssid); ///TODO Fix messeage.

        printf(("Enter Security Mode (0 - Open, 1 - WEP, 2 - WPA, 3 - WPA2):")); ///TODO Fix messeage.

        scanf("%d ", security_mode);

        printf("\n"); ///TODO Fix messeage.

        if ((0 < security_mode) && (security_mode <= 3)) {

            memset(str_passphrase, '\0', MAX_PASSPHRASE_CHARACTER_NUM + 1);

            printf(""); ///TODO Fix messeage.

            printf("Enter :"); ///TODO Fix messeage.
            scanf("%64s", str_passphrase);

        } else {

            printf(""); ///TODO Fix messeage.

            security_mode = 0;

        }

    }

    printf("Do you want to update your AWS security credentials ? (y/n)"); ///TODO Fix messeage.

    scanf("%1s", &answer_char);

    printf("\n"); ///TODO Fix messeage.

    if (answer_char == 'y') {

        memset(str_sertificate, '\0', MAX_CERTIFICATE_CHARACTER_NUM + 1);
        current_str_pointer = 0;

        printf("Enter keys as per the following format:\n\n"); ///TODO Fix messeage.

        printf(SAMPLE_CERTIFICATE_FORMAT); ///TODO Fix messeage.

        printf("Enter the root CA: \n\n"); ///TODO Fix messeage.

        do {

            scanf("%1s",(str_sertificate + current_str_pointer++));

            if (current_str_pointer < sizeof(CERTIFICATE_BEGIN_FORMAT)) {
                continue;
            }

            if ((strncmp(str_sertificate, CERTIFICATE_BEGIN_FORMAT, sizeof(CERTIFICATE_BEGIN_FORMAT) - 1) != 0)
                    || strlen(str_sertificate) > sizeof(str_sertificate)) {
                memset(str_sertificate, '\0', MAX_CERTIFICATE_CHARACTER_NUM + 1);

                current_str_pointer = 0;
                printf("Incorrect format: \n\n");
                continue;
            }

        } while (strncmp(str_sertificate + strlen(str_sertificate) - sizeof(CERTIFICATE_END_FORMAT),
                CERTIFICATE_END_FORMAT, sizeof(CERTIFICATE_END_FORMAT)) != 0);

        printf("read: ---> \n\n"); ///TODO Fix messeage.

        printf("%s", str_sertificate);

        printf(("<---\n")); ///TODO Fix messeage.

        ///TODO update function here!

    }

    printf("Do you want to update server address and device name ? (y/n)"); ///TODO Fix messeage.

    scanf("%1s", &answer_char);

    printf("\n"); ///TODO Fix messeage.

    if (answer_char == 'y') {

        printf("Enter the server address:"); ///TODO Fix messeage.

        scanf("%64s", str_server_address);

        printf("\n"); ///TODO Fix messeage.

        printf("You have enterd %64s as the server address.\n\n", str_server_address); ///TODO Fix messeage.

        printf("Enter the device name:"); ///TODO Fix messeage.

        scanf("%64s", str_device_name);

        printf("\n"); ///TODO Fix messeage.

        printf("You have enterd %64s as the device name.\n\n",str_server_address); ///TODO Fix messeage.

    }

}


void vApplicationDaemonTaskStartupHook( void )
{
    char test[8];

	prvMiscInitialization();

	xUserSwitchEventGroup = xEventGroupCreate();

	if (xUserSwitchEventGroup == NULL) {
	    /* The event group was not created because there was insufficient
        FreeRTOS heap available. */
        while (1)
            ;
    }

    prvGetStringFromTerminal(test, 8);

	prvUserSwitchISRInitialization();

    configPRINT_STRING(("Press the User button (Blue) within the next 5 seconds if you want to change the configuration"
                        "(Wifi and AWS security credentials).\r\n"));///TODO Fix messeage.

    /* Wait a maximum of 5000ms for being User switch pushed. */
	if (xEventGroupWaitBits(xUserSwitchEventGroup, (1 << 0), pdTRUE, pdFALSE, 5000) != 0) {

	    prvConnectivityConfigurationUpdate();

    }

	prvUserSwitchISRTermination();

    if( SYSTEM_Init() == pdPASS )
    {
#if(0)
        /* Initialise the RTOS's TCP/IP stack.  The tasks that use the network
        are created in the vApplicationIPNetworkEventHook() hook function
        below.  The hook function is called when the network connects. */
        FreeRTOS_IPInit( ucIPAddress,
                         ucNetMask,
                         ucGatewayAddress,
                         ucDNSServerAddress,
                         ucMACAddress );
#endif

    	/* Connect to the wifi before running the demos */
        prvWifiConnect();

        /* Provision the device with AWS certificate and private key. */
        vDevModeKeyProvisioning();

        /* Run all demos. */
        vStartRenesasDemoTask();
        //DEMO_RUNNER_RunDemos();
#if(0)
        /* Create the task to run tests. */
        xTaskCreate( TEST_RUNNER_RunTests_task,
                     "RunTests_task",
                     mainTEST_RUNNER_TASK_STACK_SIZE,
                     NULL,
                     tskIDLE_PRIORITY,
                     NULL );
#endif
    }
}
/*-----------------------------------------------------------*/

#if(1)
void prvWifiConnect( void )
{
    WIFINetworkParams_t xJoinAPParams;
    WIFIReturnCode_t xWifiStatus;

    xWifiStatus = WIFI_On();

    if( xWifiStatus == eWiFiSuccess )
    {
        configPRINTF( ( "WiFi module initialized. Connecting to AP...\r\n" ) );
    }
    else
    {
        configPRINTF( ( "WiFi module failed to initialize.\r\n" ) );
        while( 1 )
        {
        }
    }

    /* Setup parameters. */
    xJoinAPParams.pcSSID = clientcredentialWIFI_SSID;
    xJoinAPParams.pcPassword = clientcredentialWIFI_PASSWORD;
    xJoinAPParams.xSecurity = clientcredentialWIFI_SECURITY;
    xJoinAPParams.ucPasswordLength = sizeof(clientcredentialWIFI_PASSWORD);

    xWifiStatus = WIFI_ConnectAP( &( xJoinAPParams ) );

    if( xWifiStatus == eWiFiSuccess )
    {
        configPRINTF( ( "WiFi Connected to AP. Creating tasks which use network...\r\n" ) );
    }
    else
    {
        configPRINTF( ( "WiFi failed to connect to AP.\r\n" ) );

        while( 1 )
        {
        }
    }
}
#endif
/*-----------------------------------------------------------*/

//const char * pcApplicationHostnameHook( void )
//{
//    /* Assign the name "FreeRTOS" to this network node.  This function will
//     * be called during the DHCP: the machine will be registered with an IP
//     * address plus this name. */
//    return "RX65N_FREERTOS_TCP_TEST";
//}
///*-----------------------------------------------------------*/
