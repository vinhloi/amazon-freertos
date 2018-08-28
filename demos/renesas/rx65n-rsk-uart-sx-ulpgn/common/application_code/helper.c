#include "FreeRTOS.h"
#include "aws_application_version.h"

/* Declare the firmware version structure for all to see. */
const AppVersion32_t xAppFirmwareVersion = {
   .u.x.ucMajor = APP_VERSION_MAJOR,
   .u.x.ucMinor = APP_VERSION_MINOR,
   .u.x.usBuild = APP_VERSION_BUILD,
};

/* CC-RX's user define function */
void abort( void )
{
    for(;;){}
}

/* Logging task */
void vMainUARTPrintString( char * pcString )
{
}
