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
