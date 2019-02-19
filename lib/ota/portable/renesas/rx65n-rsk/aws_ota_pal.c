/*
 * Amazon FreeRTOS OTA PAL V1.0.0
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/* C Runtime includes. */
#include <stdio.h>
#include <stdlib.h>

/* Amazon FreeRTOS include. */
#include "FreeRTOS.h"
#include "aws_ota_pal.h"
#include "aws_ota_agent_internal.h"
#include "aws_ota_codesigner_certificate.h"
#include "aws_crypto.h"

/* Renesas RX platform includes */
#include "platform.h"
#include "r_fw_up_rx_if.h"
#include "r_flash_rx_if.h"

/* Specify the OTA signature algorithm we support on this platform. */
const char pcOTA_JSON_FileSignatureKey[ OTA_FILE_SIG_KEY_STR_MAX_LENGTH ] = "sig-sha256-ecdsa";   /* FIX ME. */


/* The static functions below (prvPAL_CheckFileSignature and prvPAL_ReadAndAssumeCertificate) 
 * are optionally implemented. If these functions are implemented then please set the following macros in 
 * aws_test_ota_config.h to 1:
 * otatestpalCHECK_FILE_SIGNATURE_SUPPORTED
 * otatestpalREAD_AND_ASSUME_CERTIFICATE_SUPPORTED
 */

/**
 * @brief Verify the signature of the specified file.
 * 
 * This function should be implemented if signature verification is not offloaded
 * to non-volatile memory io functions.
 * 
 * This function is called from prvPAL_Close(). 
 * 
 * @param[in] C OTA file context information.
 * 
 * @return Below are the valid return values for this function.
 * kOTA_Err_None if the signature verification passes.
 * kOTA_Err_SignatureCheckFailed if the signature verification fails.
 * kOTA_Err_BadSignerCert if the if the signature verification certificate cannot be read.
 * 
 */
static OTA_Err_t prvPAL_CheckFileSignature( OTA_FileContext_t * const C );

/**
 * @brief Read the specified signer certificate from the filesystem into a local buffer.
 * 
 * The allocated memory returned becomes the property of the caller who is responsible for freeing it.
 * 
 * This function is called from prvPAL_CheckFileSignature(). It should be implemented if signature
 * verification is not offloaded to non-volatile memory io function.
 * 
 * @param[in] pucCertName The file path of the certificate file.
 * @param[out] ulSignerCertSize The size of the certificate file read.
 * 
 * @return A pointer to the signer certificate in the file system. NULL if the certificate cannot be read.
 * This returned pointer is the responsibility of the caller; if the memory is allocated the caller must free it.
 */
static uint8_t * prvPAL_ReadAndAssumeCertificate( const uint8_t * const pucCertName,
                                                  uint32_t * const ulSignerCertSize );

/*-----------------------------------------------------------*/

OTA_ImageState_t eSavedAgentState = eOTA_ImageState_Unknown;

/* Size of buffer used in file operations on this platform. */
#define OTA_PAL_RX_BUF_SIZE ( ( size_t ) 4096UL )

OTA_Err_t prvPAL_CreateFileForRx( OTA_FileContext_t * const C )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_CreateFileForRx" );

    OTA_LOG_L1("Function call: [%s].\r\n", OTA_METHOD_NAME);

	OTA_Err_t eResult = kOTA_Err_Uninitialized; /* For MISRA mandatory. */
	fw_up_return_t flash_status;

	if( C != NULL )
	{
		/* No need for pacFilepath, we write directly to ROM instead of file system
		 * But still check for its existence because it is required by AWS */
		if ( C->pacFilepath != NULL )
		{
			flash_status = fw_up_open();
			if (FW_UP_SUCCESS != flash_status)
			{
				OTA_LOG_L1( "[%s] ERROR - Failed to initialize firmware update.\r\n", OTA_METHOD_NAME );
				return eResult;
			}

			flash_status = erase_another_bank();
			if (FW_UP_SUCCESS != flash_status)
			{
				OTA_LOG_L1("[%s] ERROR - Failed to erase another bank.\r\n", OTA_METHOD_NAME);
				return eResult;
			}

			C->pucFile = (uint8_t *)FLASH_CF_LO_BANK_LO_ADDR;

			eResult = kOTA_Err_None;
			OTA_LOG_L1( "[%s] Firmware update initialized.\r\n", OTA_METHOD_NAME );
		}
		else
		{
			eResult = kOTA_Err_RxFileCreateFailed;
			OTA_LOG_L1( "[%s] ERROR - Invalid context provided.\r\n", OTA_METHOD_NAME );
		}
	}
	else
	{
		eResult = kOTA_Err_RxFileCreateFailed;
		OTA_LOG_L1( "[%s] ERROR - Invalid context provided.\r\n", OTA_METHOD_NAME );
	}

	return eResult;
}
/*-----------------------------------------------------------*/

/* Abort receiving the specified OTA update by closing the file. */

OTA_Err_t prvPAL_Abort( OTA_FileContext_t * const C )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_Abort" );

	OTA_LOG_L1("Function call: [%s].\r\n", OTA_METHOD_NAME);

    /* Set default return status to uninitialized. */
    OTA_Err_t eResult = kOTA_Err_Uninitialized;

    if( NULL != C )
    {
        if( NULL != C->pucFile )
        {
            C->pucFile = NULL;
        }
		eResult = kOTA_Err_None;
    }
    else /* Context was not valid. */
    {
        OTA_LOG_L1( "[%s] ERROR - Invalid context.\r\n", OTA_METHOD_NAME );
        eResult = kOTA_Err_FileAbort;
    }

    return eResult;
}
/*-----------------------------------------------------------*/

/* Write a block of data to the specified file.
 * Returns the most recent number of bytes written upon success or an error code.
 */
int16_t prvPAL_WriteBlock( OTA_FileContext_t * const C,
                           uint32_t ulOffset,
                           uint8_t * const pacData,
                           uint32_t ulBlockSize )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_WriteBlock" );

    int32_t lResult = 0;
    fw_up_return_t status;

	if( C != NULL )
	{
		status = analyze_and_write_data(pacData, ulBlockSize);

		if (FW_UP_SUCCESS == status)
		{
			lResult = ulBlockSize;
		}
		else
		{
			OTA_LOG_L1( "[%s] ERROR - fwrite failed\r\n", OTA_METHOD_NAME );
			lResult = ( status | ( errno & kOTA_PAL_ErrMask ) );
		}

	}
	else /* Invalid context provided. */
	{
		OTA_LOG_L1( "[%s] ERROR - Invalid context.\r\n", OTA_METHOD_NAME );
		lResult = ( kOTA_Err_NoFreeContext | ( errno & kOTA_PAL_ErrMask ) );
	}

	return ( int16_t ) lResult;
}
/*-----------------------------------------------------------*/

/* Close the specified file. This will also authenticate the file if it is marked as secure. */

OTA_Err_t prvPAL_CloseFile( OTA_FileContext_t * const C )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_CloseFile" );

	OTA_LOG_L1("Function call: [%s].\r\n", OTA_METHOD_NAME);

    OTA_Err_t eResult = kOTA_Err_None;
    fw_up_return_t flash_status;

    if( C != NULL )
    {
        if( C->pxSignature != NULL )
        {
            /* Verify the file signature, close the file and return the signature verification result. */
            eResult = prvPAL_CheckFileSignature( C );
        }
        else
        {
            OTA_LOG_L1( "[%s] ERROR - NULL OTA Signature structure.\r\n", OTA_METHOD_NAME );
            eResult = kOTA_Err_SignatureCheckFailed;
        }

        /* Close the file. */
        flash_status = fw_up_close();
        if (FW_UP_SUCCESS != flash_status)
		{
        	OTA_LOG_L1( "[%s] ERROR - Failed to close firmware update.\r\n", OTA_METHOD_NAME );
        	eResult = ( kOTA_Err_FileClose | ( errno & kOTA_PAL_ErrMask ) );
		}

        if( eResult == kOTA_Err_None )
        {
            OTA_LOG_L1( "[%s] %s signature verification passed.\r\n", OTA_METHOD_NAME, pcOTA_JSON_FileSignatureKey );
        }
        else
        {
            OTA_LOG_L1( "[%s] ERROR - Failed to pass %s signature verification: %d.\r\n", OTA_METHOD_NAME,
                        pcOTA_JSON_FileSignatureKey, eResult );

			/* If we fail to verify the file signature that means the image is not valid. We need to set the image state to aborted. */
			prvPAL_SetPlatformImageState( eOTA_ImageState_Aborted );

        }
    }
    else /* Invalid OTA Context. */
    {
        /* FIXME: Invalid error code for a null file context and file handle. */
        OTA_LOG_L1( "[%s] ERROR - Invalid context.\r\n", OTA_METHOD_NAME );
        eResult = kOTA_Err_FileClose;
    }

    return eResult;
}
/*-----------------------------------------------------------*/


/* Verify the signature of the specified file. */

static OTA_Err_t prvPAL_CheckFileSignature( OTA_FileContext_t * const C )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_CheckFileSignature" );

	OTA_LOG_L1("Function call: [%s].\r\n", OTA_METHOD_NAME);

    OTA_Err_t eResult = kOTA_Err_None;
    uint32_t ulBytesRead;
    uint32_t ulSignerCertSize;
    uint8_t * pucBuf, * pucSignerCert;
    void * pvSigVerifyContext;

    if( C != NULL )
    {
        /* Verify an ECDSA-SHA256 signature. */
        if( pdFALSE == CRYPTO_SignatureVerificationStart( &pvSigVerifyContext, cryptoASYMMETRIC_ALGORITHM_ECDSA, cryptoHASH_ALGORITHM_SHA256 ) )
        {
            eResult = kOTA_Err_SignatureCheckFailed;
        }
        else
        {
            OTA_LOG_L1( "[%s] Started %s signature verification\r\n", OTA_METHOD_NAME, pcOTA_JSON_FileSignatureKey );
            pucSignerCert = signingcredentialSIGNING_CERTIFICATE_PEM;

            if( pucSignerCert != NULL )
            {
                pucBuf = pvPortMalloc( OTA_PAL_RX_BUF_SIZE ); /*lint !e9079 Allow conversion. */

                if( pucBuf != NULL )
                {
                    /* Rewind the received file to the beginning. */
                    if( fseek( C->pucFile, 0L, SEEK_SET ) == 0 ) /*lint !e586
                                                                  * C standard library call is being used for portability. */
                    {
                        do
                        {
                            ulBytesRead = fread( pucBuf, 1, OTA_PAL_RX_BUF_SIZE, C->pucFile ); /*lint !e586
                                                                                               * C standard library call is being used for portability. */
                            /* Include the file chunk in the signature validation. Zero size is OK. */
                            CRYPTO_SignatureVerificationUpdate( pvSigVerifyContext, pucBuf, ulBytesRead );
                        } while( ulBytesRead > 0UL );

                        if( pdFALSE == CRYPTO_SignatureVerificationFinal( pvSigVerifyContext,
                                                                          ( char * ) pucSignerCert,
                                                                          ( size_t ) ulSignerCertSize,
                                                                          C->pxSignature->ucData,
                                                                          C->pxSignature->usSize ) ) /*lint !e732 !e9034 Allow comparison in this context. */
                        {
                            eResult = kOTA_Err_SignatureCheckFailed;
                        }
						pvSigVerifyContext = NULL;	/* The context has been freed by CRYPTO_SignatureVerificationFinal(). */
                    }
                    else
                    {
                        /* Nothing special to do. */
                    }

                    /* Free the temporary file page buffer. */
                    vPortFree( pucBuf );
                }
                else
                {
                    OTA_LOG_L1( "[%s] ERROR - Failed to allocate buffer memory.\r\n", OTA_METHOD_NAME );
                    eResult = kOTA_Err_OutOfMemory;
                }

                /* Free the signer certificate that we now own after prvReadAndAssumeCertificate(). */
                vPortFree( pucSignerCert );
            }
            else
            {
                eResult = kOTA_Err_BadSignerCert;
            }
        }
    }
    else
    {
        /* FIXME: Invalid error code for a NULL file context. */
        OTA_LOG_L1( "[%s] ERROR - Invalid OTA file context.\r\n", OTA_METHOD_NAME );
        /* Invalid OTA context or file pointer. */
        eResult = kOTA_Err_NullFilePtr;
    }

    return eResult;
}
/*-----------------------------------------------------------*/

static uint8_t * prvPAL_ReadAndAssumeCertificate( const uint8_t * const pucCertName,
                                                  uint32_t * const ulSignerCertSize )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_ReadAndAssumeCertificate" );

    /* Avoid compiler warnings about unused variables for a release including source code. */
    R_INTERNAL_NOT_USED(OTA_METHOD_NAME);
    R_INTERNAL_NOT_USED(pucCertName);
    R_INTERNAL_NOT_USED(ulSignerCertSize);

    /* FIX ME. */
    return NULL;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_ResetDevice( void )
{
    DEFINE_OTA_METHOD_NAME("prvPAL_ResetDevice");

    /* Avoid compiler warnings about unused variables for a release including source code. */
    R_INTERNAL_NOT_USED(OTA_METHOD_NAME);

    /* FIX ME. */
    return kOTA_Err_ResetNotSupported;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_ActivateNewImage( void )
{
    DEFINE_OTA_METHOD_NAME("prvPAL_ActivateNewImage");

    OTA_LOG_L1("Function call: prvPAL_ActivateNewImage.\r\n");

    fw_up_return_t flash_status;
	bool reset_vector_ok;

	reset_vector_ok = fw_up_check_reset_vector();

	if (reset_vector_ok)
	{
		OTA_LOG_L1( "[%s] Changing the Startup Bank\r\n", OTA_METHOD_NAME );

		flash_status = bank_toggle();
		if (FW_UP_SUCCESS == flash_status)
		{
			OTA_LOG_L1( "[%s] Reset\r\n", OTA_METHOD_NAME );

			/* soft reset */
			fw_up_soft_reset();
		}
		else
		{
			OTA_LOG_L1( "[%s] ERROR - Flash error\r\n", OTA_METHOD_NAME );
		}
	}
	else
	{
		OTA_LOG_L1( "[%s] ERROR - Reset vector is invalid.\r\n", OTA_METHOD_NAME );
	}

    return kOTA_Err_Uninitialized;
}
/*-----------------------------------------------------------*/

/*
 * Set the final state of the last transferred (final) OTA file (or bundle).
 */

OTA_Err_t prvPAL_SetPlatformImageState( OTA_ImageState_t eState )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_SetPlatformImageState" );

    OTA_LOG_L1("Function call: prvPAL_GetPlatformImageState: [%d]\r\n", eState);

    OTA_Err_t eResult = kOTA_Err_None;

    if( eState != eOTA_ImageState_Unknown && eState <= eOTA_LastImageState )
    {
    	/* Save the image state to eSavedAgentState. */
    	eSavedAgentState = eState;

    }
    else /* Image state invalid. */
    {
        OTA_LOG_L1( "[%s] ERROR - Invalid image state provided.\r\n", OTA_METHOD_NAME );
        eResult = kOTA_Err_BadImageState;
    }

    return eResult;
}
/*-----------------------------------------------------------*/

/* Get the state of the currently running image.
 *
 * We read this at OTA_Init time so we can tell if the MCU image is in self
 * test mode. If it is, we expect a successful connection to the OTA services
 * within a reasonable amount of time. If we don't satisfy that requirement,
 * we assume there is something wrong with the firmware and reset the device,
 * causing it to rollback to the previous code.
 */

OTA_PAL_ImageState_t prvPAL_GetPlatformImageState( void )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_GetPlatformImageState" );

	OTA_PAL_ImageState_t ePalState = eOTA_PAL_ImageState_Unknown;

	switch (eSavedAgentState)
	{
		case eOTA_ImageState_Testing:
			ePalState = eOTA_PAL_ImageState_PendingCommit;
			break;
		case eOTA_ImageState_Accepted:
			ePalState = eOTA_PAL_ImageState_Valid;
			break;
		case eOTA_ImageState_Unknown: // Uninitialized image state, assume a factory image
			ePalState = eOTA_PAL_ImageState_Valid;
			break;
		case eOTA_ImageState_Rejected:
		case eOTA_ImageState_Aborted:
		default:
			ePalState = eOTA_PAL_ImageState_Invalid;
			break;
	}

	OTA_LOG_L1("Function call: prvPAL_GetPlatformImageState: [%d]\r\n", ePalState);
    return ePalState; /*lint !e64 !e480 !e481 I/O calls and return type are used per design. */
}
/*-----------------------------------------------------------*/

/* Provide access to private members for testing. */
#ifdef AMAZON_FREERTOS_ENABLE_UNIT_TESTS
    #include "aws_ota_pal_test_access_define.h"
#endif
