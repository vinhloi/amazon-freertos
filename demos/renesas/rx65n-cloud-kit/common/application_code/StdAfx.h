/* stdafx.h : include file for standard system include files,
 * or project specific include files that are used frequently, but
 * are changed infrequently (this is true not only in case of PCH
 * but also in general case of MAKE or build systems like MAKE)
 */

#ifndef _STDAFX_H_
#define _STDAFX_H_

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
//#include "croutine.h" // Amazon FreeRTOS does not have this header
#include "timers.h"
#include "event_groups.h"
#include "message_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/* FreeRTOS headers seem to lack following declaration */
#if( configUSE_DAEMON_TASK_STARTUP_HOOK == 1 )
extern void vApplicationDaemonTaskStartupHook( void );
#endif /* configUSE_DAEMON_TASK_STARTUP_HOOK */

/* Version includes. */
#include "aws_application_version.h"

/* System init includes. */
#include "aws_system_init.h"

/* PKCS#11 includes. */
#include "aws_pkcs11.h"
#include "aws_pkcs11_config.h"

/* Client credential includes. */
#include "aws_clientcredential.h"

/* mbedTLS includes. */
#include "mbedtls/base64.h"

/* Defender includes. */
#include "aws_defender.h"

/* Key provisioning includes. */
#include "aws_dev_mode_key_provisioning.h"

/* Greengrass includes. */
#include "aws_ggd_config.h"
#include "aws_ggd_config_defaults.h"
#include "aws_greengrass_discovery.h"

/* Logging includes. */
#include "aws_logging_task.h"

/* MQTT includes. */
#include "aws_mqtt_agent.h"

/* Amazon FreeRTOS OTA agent includes. */
#include "aws_ota_agent.h"

/* Required for shadow APIs. */
#include "aws_shadow.h"

/* FreeRTOS+TCP includes. */
//#include "FreeRTOS_IP.h" // Remove '//' when necessary
////#include "FreeRTOS_Sockets.h" // Using aws_secure_sockets.h is better

/* TCP/IP abstraction includes. */
#include "aws_secure_sockets.h"

/* WiFi interface includes. */
#include "aws_wifi.h" // Comment out when unnecessary

/* Standard includes. */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

/* Project specific includes. (You can replace headers for your needs.) */
//#include "include/aws_demo_runner.h"

#ifdef __cplusplus
}
#endif

#endif /* _STDAFX_H_ */
