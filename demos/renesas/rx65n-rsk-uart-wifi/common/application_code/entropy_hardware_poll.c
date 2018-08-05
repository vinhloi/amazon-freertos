/**
  ******************************************************************************
  * @file    entropy_hardware_poll.c
  *
  ******************************************************************************
  */

#include <string.h>
#include "FreeRTOS.h"

int mbedtls_hardware_poll( void *data, unsigned char *output, size_t len, size_t *olen );

int mbedtls_hardware_poll( void *data,
                    unsigned char *output, size_t len, size_t *olen )
{
	uint32_t random_number = 0;

	get_random_number(&random_number, sizeof(uint32_t));
	((void) data);
	*olen = 0;

	memcpy(output, &random_number, sizeof(uint32_t));
	*olen = sizeof(uint32_t);

	return 0;
}

