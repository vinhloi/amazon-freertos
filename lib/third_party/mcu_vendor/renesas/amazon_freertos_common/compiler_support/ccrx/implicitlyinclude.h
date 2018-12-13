#define MBEDTLS_USER_CONFIG_FILE "mbedtls_user_config.h"

/* Avoid CC-RX's compiler warning message 'M0520193: Zero used for 
 * undefined preprocessing identifier "XXXX"' in CC-RX's math.h */
#if !defined(_FEVAL)
#define _FEVAL 0
#endif
#if !defined(_HAS_C9X_FAST_FMA)
#define _HAS_C9X_FAST_FMA 0
#endif
