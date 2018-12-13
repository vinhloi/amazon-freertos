/*
 * Amazon FreeRTOS PKCS #11 PAL V1.0.0
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
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/**
 * @file aws_pkcs11_pal.c
 * @brief Device specific helpers for PKCS11 Interface.
 */

/* Amazon FreeRTOS Includes. */
#include "aws_pkcs11.h"
#include "FreeRTOS.h"

/* C runtime includes. */
#include <stdio.h>
#include <string.h>

/* Renesas platform includes */
#include "platform.h"

typedef struct _pkcs_data
{
	CK_ATTRIBUTE Label;
	uint32_t local_storage_index;
	uint32_t ulDataSize;
	uint32_t status;
}PKCS_DATA;

#define PKCS_DATA_STATUS_EMPTY 0
#define PKCS_DATA_STATUS_REGISTERD 1
#define PKCS_DATA_STATUS_DELETED 2

#define PKCS_HANDLES_LABEL_MAX_LENGTH 40
#define PKCS_OBJECT_HANDLES_NUM 5

enum eObjectHandles
{
    eInvalidHandle = 0, /* According to PKCS #11 spec, 0 is never a valid object handle. */
    eAwsDevicePrivateKey = 1,
    eAwsDevicePublicKey,
    eAwsDeviceCertificate,
    eAwsCodeSigningKey,
    //eAwsRootCertificate
};

uint8_t object_handle_dictionary[PKCS_OBJECT_HANDLES_NUM][PKCS_HANDLES_LABEL_MAX_LENGTH] = {
        "",
        pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
        pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
        pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
        pkcs11configLABEL_CODE_VERIFICATION_KEY,
        //pkcs11configLABEL_ROOT_CERTIFICATE,
};

static PKCS_DATA pkcs_data[PKCS_OBJECT_HANDLES_NUM];
static uint32_t stored_data_size = 0;

R_ATTRIB_SECTION_CHANGE(B, _PKCS11_STORAGE, 1)
static uint8_t local_storage[60000];	/* need to use NVM, now on RAM, this is experimental. (Renesas/Ishiguro) */
R_ATTRIB_SECTION_CHANGE_END

static uint32_t current_stored_size(void);

/**
* @brief Writes a file to local storage.
*
* Port-specific file write for crytographic information.
*
* @param[in] pxLabel       Label of the object to be saved.
* @param[in] pucData       Data buffer to be written to file
* @param[in] ulDataSize    Size (in bytes) of data to be saved.
*
* @return The file handle of the object that was stored.
*/
CK_OBJECT_HANDLE PKCS11_PAL_SaveObject( CK_ATTRIBUTE_PTR pxLabel,
    uint8_t * pucData,
    uint32_t ulDataSize )
{
    CK_OBJECT_HANDLE xHandle = eInvalidHandle;
    uint32_t size;
    int i;

    for (i = 1; i < PKCS_OBJECT_HANDLES_NUM; i++) {
        if (!strcmp((char * )&object_handle_dictionary[i],
                pxLabel->pValue)) {
            xHandle = i;
        }
    }

    if (xHandle != eInvalidHandle) {

        size = current_stored_size();

        memcpy(&local_storage[size], pucData, ulDataSize);

        pkcs_data[xHandle].Label.pValue = pxLabel->pValue;
        pkcs_data[xHandle].Label.type = pxLabel->type;
        pkcs_data[xHandle].Label.ulValueLen = pxLabel->ulValueLen;
        pkcs_data[xHandle].ulDataSize = ulDataSize;
        pkcs_data[xHandle].local_storage_index = size;
        pkcs_data[xHandle].status = PKCS_DATA_STATUS_REGISTERD;

        stored_data_size += ulDataSize;

    }

    return xHandle;

}

/**
* @brief Translates a PKCS #11 label into an object handle.
*
* Port-specific object handle retrieval.
*
*
* @param[in] pLabel         Pointer to the label of the object
*                           who's handle should be found.
* @param[in] usLength       The length of the label, in bytes.
*
* @return The object handle if operation was successful.
* Returns eInvalidHandle if unsuccessful.
*/
CK_OBJECT_HANDLE PKCS11_PAL_FindObject( uint8_t * pLabel,
    uint8_t usLength )
{
	CK_OBJECT_HANDLE xHandle = eInvalidHandle;
	int i;

	for(i = 1; i < PKCS_OBJECT_HANDLES_NUM; i++)
	{
		if(!strcmp((char *)&object_handle_dictionary[i], (char *)pLabel))
		{
            xHandle = i;
		}
	}

    return xHandle;
}

/**
* @brief Gets the value of an object in storage, by handle.
*
* Port-specific file access for cryptographic information.
*
* This call dynamically allocates the buffer which object value
* data is copied into.  PKCS11_PAL_GetObjectValueCleanup()
* should be called after each use to free the dynamically allocated
* buffer.
*
* @sa PKCS11_PAL_GetObjectValueCleanup
*
* @param[in] pcFileName    The name of the file to be read.
* @param[out] ppucData     Pointer to buffer for file data.
* @param[out] pulDataSize  Size (in bytes) of data located in file.
* @param[out] pIsPrivate   Boolean indicating if value is private (CK_TRUE)
*                          or exportable (CK_FALSE)
*
* @return CKR_OK if operation was successful.  CKR_KEY_HANDLE_INVALID if
* no such object handle was found, CKR_DEVICE_MEMORY if memory for
* buffer could not be allocated, CKR_FUNCTION_FAILED for device driver
* error.
*/
CK_RV PKCS11_PAL_GetObjectValue( CK_OBJECT_HANDLE xHandle,
    uint8_t ** ppucData,
    uint32_t * pulDataSize,
    CK_BBOOL * pIsPrivate )
{
    CK_RV xReturn = CKR_FUNCTION_FAILED;

    CK_OBJECT_HANDLE xHandleStorage = xHandle;

    if (xHandleStorage == eAwsDevicePublicKey) {

        xHandleStorage = eAwsDevicePrivateKey;

    }

    if (xHandle != eInvalidHandle) {
        *ppucData = &local_storage[pkcs_data[xHandleStorage].local_storage_index];
        *pulDataSize = pkcs_data[xHandleStorage].ulDataSize;

        if (!strcmp((char * )&object_handle_dictionary[xHandle],
                pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS)) {
            *pIsPrivate = CK_TRUE;
        } else {
            *pIsPrivate = CK_FALSE;
        }

        xReturn = CKR_OK;

    }

    return xReturn;
}


/**
* @brief Cleanup after PKCS11_GetObjectValue().
*
* @param[in] pucData       The buffer to free.
*                          (*ppucData from PKCS11_PAL_GetObjectValue())
* @param[in] ulDataSize    The length of the buffer to free.
*                          (*pulDataSize from PKCS11_PAL_GetObjectValue())
*/
void PKCS11_PAL_GetObjectValueCleanup( uint8_t * pucData,
    uint32_t ulDataSize )
{
    /* FIX ME. */
}

uint32_t current_stored_size(void)
{
	return stored_data_size;
}
