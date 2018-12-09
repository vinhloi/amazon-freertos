/* Remove mbedTLS's unused *_self_test functions to remove 
 * linker warnings of the GNURX projects such as 
 * '_fstat is not implemented and will always fail', 
 * '_isatty is not implemented and will always fail', 
 * and so on. These are cause by using fprintf function. 
 * (But such warnings cannot be removed by only this.)"
 */
#if defined(MBEDTLS_SELF_TEST)
#undef MBEDTLS_SELF_TEST
#endif

/* Workaround for CC-RX's non-standard 2nd argument type of 
 * memset function. CC-RX uses long but standard uses int.
 */
#if defined(__CCRX__)
#include "string.h"
typedef void * (* mbedtls_platform_zeroize_memset_t)( void *, int, size_t );
#undef memset
#define memset ((mbedtls_platform_zeroize_memset_t)memset)
#endif
