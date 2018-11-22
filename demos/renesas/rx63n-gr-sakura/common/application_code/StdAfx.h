/* stdafx.h : include file for standard system include files,
 * or project specific include files that are used frequently, but
 * are changed infrequently (this is true not only in case of PCH
 * but also in general case of MAKE or build systems like MAKE)
 */

#ifndef _STDAFX_H_
#define _STDAFX_H_

#ifdef __cplusplus
extern "C" {
#endif

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
//#include "croutine.h" // Amazon FreeRTOS does not have this header
#include "timers.h"
#include "event_groups.h"
#include "message_buffer.h"

/* Version includes. */
#include "aws_application_version.h"

/* System init includes. */
#include "aws_system_init.h"

/* PKCS#11 includes. */
#include "aws_pkcs11.h"
#include "aws_pkcs11_config.h"

/* Client credential includes. */
#include "aws_clientcredential.h"
#include "aws_default_root_certificates.h"

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
#include "aws_greengrass_discovery_demo.h"

/* Logging includes. */
#include "aws_logging_task.h"

/* MQTT includes. */
#include "aws_mqtt_agent.h"

/* Amazon FreeRTOS OTA agent includes. */
#include "aws_ota_agent.h"

/* Required for shadow APIs. */
#include "aws_shadow.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h" // Comment out when unnecessary
#include "FreeRTOS_Sockets.h" // Comment out when unnecessary

/* WiFi interface includes. */
//#include "aws_wifi.h" // Remove '//' when necessary

/* Standard includes. */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

#ifdef __cplusplus
}
#endif

#endif /* _STDAFX_H_ */
