#ifdef INCLUDE_FROM_OTA_AGENT

/* Abort receiving the specified OTA update by closing the file.
 * This shall do whatever is necessary to clean up the file on the STM32L4xx platform.
 * Today this is does not require much of anything.
 */

static OTA_Err_t prvAbort(OTA_FileContext_t * const C)
{
    /* FIX ME. */
	return kOTA_Err_SoftwareBug;
}

/* Attempt to create a new receive file for the OTA update. */

static bool_t prvCreateFileForRx(OTA_FileContext_t * const C)
{
    /* FIX ME. */
	return pdFALSE;
}

/* Close the specified file. This may authenticate the file if it is marked as secure. */

static OTA_Err_t prvCloseFile(OTA_FileContext_t * const C)
{
    /* FIX ME. */
	return kOTA_Err_SoftwareBug;
}

static OTA_Err_t prvActivateNewImage(void)
{
    /* FIX ME. */
	return kOTA_Err_SoftwareBug;
}

/* Set the final state of the last transferred (final) OTA file.
 */

static OTA_Err_t prvSetImageState (OTA_ImageState_t eState)
{
    /* FIX ME. */
	return kOTA_Err_SoftwareBug;
}

/* Verify the signature of the specified file. */

static OTA_Err_t prvCheckFileSignature(OTA_FileContext_t * const C)
{
    /* FIX ME. */
	return kOTA_Err_SoftwareBug;
}

/* Read the specified signer certificate from the file system into a local buffer. The allocated
   memory becomes the property of the caller who is responsible for freeing it.
 */

static u8 * prvReadAndAssumeCertificate(const u8 * const pucCertName, s32 * const lSignerCertSize)
{
    /* FIX ME. */
	return NULL;
}

/* Write a block of data to the specified file. */

static s16 prvWriteBlock(OTA_FileContext_t * const C, s32 iOffset, u8 * const pacData, u32 iBlockSize)
{
    /* FIX ME. */
	return -1;
}

#else
void dummy_function_to_avoid_empty_translation_unit_warning (void);
#endif /* INCLUDE_FROM_OTA_AGENT */

