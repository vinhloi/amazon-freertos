/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "message_buffer.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

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
//#include "FreeRTOS_IP.h" // Remove '//' when necessary
//#include "FreeRTOS_Sockets.h" // Remove '//' when necessary

/* WiFi interface includes. */
#include "aws_wifi.h" // Comment out when unnecessary

/* Standard includes. */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
