#ifndef AWS_CLIENT_CREDENTIAL_KEYS_H
#define AWS_CLIENT_CREDENTIAL_KEYS_H

#include <stdint.h>

/*
 * PEM-encoded client certificate.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----"
 */
#define keyCLIENT_CERTIFICATE_PEM \
"-----BEGIN CERTIFICATE-----\n"\
"MIIDWTCCAkGgAwIBAgIUCdmVgYEaSK52Ogvfm1VTv2ZT6xEwDQYJKoZIhvcNAQEL\n"\
"BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n"\
"SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTE5MDExMDEwNTAy\n"\
"MloXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n"\
"ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMiliK9puMoUnJKUPNwI\n"\
"/j+DvfVDOQ3Qi31fm/gKlf987qN4DzRipY+Uz3V3TNSSpCeDGx+ootc9njWxlqLt\n"\
"9MdnMeEYnQ9tXWFlZYmzhZ8TAEsw/uPStumIBw9BCq/7XtvYDom5I9KS+c3HwRun\n"\
"TLTMz4So9fN5Vfw94Hs1GDK0BKa6cC2xUwCHAX+D+9DKimj0DvxsJQryK2JykccG\n"\
"l0sM8TdLcSDLNqqTBXWBOgXhAA3haNvSHX1wLChYrqCFzbmGUO7uZ7NUrwzo0xSu\n"\
"iRQ7UAuPD2GwXiFd6m0teSkPnvg3Ref5/ICiNeWk28VlMZo7bCw3C6DCQnCOtuG+\n"\
"SLMCAwEAAaNgMF4wHwYDVR0jBBgwFoAUydQTuaaWd2sRxTehC0FsOa5yrkgwHQYD\n"\
"VR0OBBYEFOi7MEDCbqNgJJVqqBG2quo1FWIQMAwGA1UdEwEB/wQCMAAwDgYDVR0P\n"\
"AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQBFzI03AwLnpb7dvVWu4bNSlav2\n"\
"D1aZXZL3DZAGmu2r11uml05kQv2BmIPVBp4AZfFHwqpaFFjB6/y7pUHQgC0WQcun\n"\
"AOYZfsIJDIQqAcOQL/UZ2qx9gqnAO2qmuiDkfvlt/1APwPJyF0Y0FABl8Z91iZBm\n"\
"z957nBv2XAJuG/WhzUvimGTKWeveIbwLWAtfQ+9pPG4IhRSXfvR825nhcIRgV5lX\n"\
"fnONoxjDwsptE9MOu+181WYPu0vqD69LKPMoAfRDwWskrxa3GLBf25UZ+8/Sun3R\n"\
"yIefxyJdyNy3n8nA2TpX8D+l8EV54PoQMPIOR95xjLba4uJnJTJAB85YdJdI\n"\
"-----END CERTIFICATE-----"

/*
 * PEM-encoded client private key.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN RSA PRIVATE KEY-----\n"\
 * "...base64 data...\n"\
 * "-----END RSA PRIVATE KEY-----"
 */
#define keyCLIENT_PRIVATE_KEY_PEM \
"-----BEGIN RSA PRIVATE KEY-----\n"\
"MIIEowIBAAKCAQEAyKWIr2m4yhSckpQ83Aj+P4O99UM5DdCLfV+b+AqV/3zuo3gP\n"\
"NGKlj5TPdXdM1JKkJ4MbH6ii1z2eNbGWou30x2cx4RidD21dYWVlibOFnxMASzD+\n"\
"49K26YgHD0EKr/te29gOibkj0pL5zcfBG6dMtMzPhKj183lV/D3gezUYMrQEprpw\n"\
"LbFTAIcBf4P70MqKaPQO/GwlCvIrYnKRxwaXSwzxN0txIMs2qpMFdYE6BeEADeFo\n"\
"29IdfXAsKFiuoIXNuYZQ7u5ns1SvDOjTFK6JFDtQC48PYbBeIV3qbS15KQ+e+DdF\n"\
"5/n8gKI15aTbxWUxmjtsLDcLoMJCcI624b5IswIDAQABAoIBACYvGmwHeps6gPGE\n"\
"NHDLZVdBAw2B1YHDhkg9wbjaiuEsLV3+oyYc/9TrRO/d3Ek3/vVmBetodFTx7QHa\n"\
"4gfvsP2hreRggsNE1NiMK16hQ3SacRveHyPMHfMjZ/SHNDl5ZPwBigK7sQ2J2vBG\n"\
"a4GPvRFLSNK+NNA47xwpymaoHArJ0RHlewwz2SKwsusv7NaBeexPLteVnljGpMUX\n"\
"3ejXJiufzkSVTFxZ11hsPyJhRdg8rLCeJtR8FVWFQ8C8vFiGM7Q4dc/J6qtwmnKQ\n"\
"oId+VFYC5jT3JF6+gDDhHCViYSSI8SNExrWPvByU7hxKAMA4EcHKYmmslnNOLqAL\n"\
"D4CGQsECgYEA7+V7YAMRNdGIzPRH/Grdm7rczMxatSSCs9zRys37i514l8b+ZhGr\n"\
"/wh2U/OxJT5NcJlZrnVzee16ZZGCLR/RnHcoqWYJp8aMU0wrqCezZXnDkDCHTJa5\n"\
"+cccVpHn/ZpvbnkzGbJ/fdMKs6TnTVhjsVuEwo6H8sp6XvSLNq/Uk00CgYEA1h2P\n"\
"lnQ2lT/eZeQDLmTPh8RmgZjnxVI3r8TAvHLyGziKdUOQL1CtgncuG8cZpEQoj1i4\n"\
"glnUgLelH6GAW0xu+WD8uuf5t8TQHppATmRiLtg5waommsT996jij/JKcGnM5tg5\n"\
"Vd6k/YWq/oh/N9e5X4eJa0xB5XEyqopt6rE4S/8CgYAe5ea7GgoIbYF6LGhtKXJ7\n"\
"kVjJA/UW6anv8pwOy/L/K3wRyG6RxOrrQTLEY1nImzqsKwQzBSunY5/lHxWuLC/6\n"\
"QBq7nsnVkywVunTRBrQDeOhoVOfIRI2Vj0CmIcrIjjCWAQRE5gmnshvpwW6NNro4\n"\
"aLozuNnnuo6CVmrr0z/E7QKBgQCIagkDWEM2T5zH2b9VHZFC9JRCWULA6qLj7zzE\n"\
"U4+AXEOKdmM0YfhJuNi9hdoGWA5nVaUoF0dWQqqHPoPipw3FAE/EXOsyc8gM10XF\n"\
"LBuJsOULR/aUgJE16nl+MGXQvPd406Wj2dfQVgsdY6V51I0cGeQpYkpRfg9DrDJf\n"\
"Zk1xzQKBgHnn9ShwsavH5+a0FtBchCPJLAqMIjbaXYyxTnsCkAyaNf2ahwmtCZaL\n"\
"Cir4+/4LcWBM9W8WGvlmtl1URzCo9OGCtblrjGPgDv5AcSlYfpHZUdBoyYnqUvRM\n"\
"gPA1kpPrkoJyLSuUfj5GJJ++gw02YS6kEOCPBBEmPY7cwLweE/BJ\n"\
"-----END RSA PRIVATE KEY-----"

/*
 * PEM-encoded Just-in-Time Registration (JITR) certificate (optional).
 *
 * If used, must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----"
 */
#define keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM  NULL

/* The constants above are set to const char * pointers defined in aws_dev_mode_key_provisioning.c,
 * and externed here for use in C files.  NOTE!  THIS IS DONE FOR CONVENIENCE
 * DURING AN EVALUATION PHASE AND IS NOT GOOD PRACTICE FOR PRODUCTION SYSTEMS
 * WHICH MUST STORE KEYS SECURELY. */
extern const char clientcredentialCLIENT_CERTIFICATE_PEM[];
extern const char* clientcredentialJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM;
extern const char clientcredentialCLIENT_PRIVATE_KEY_PEM[];
extern const uint32_t clientcredentialCLIENT_CERTIFICATE_LENGTH;
extern const uint32_t clientcredentialCLIENT_PRIVATE_KEY_LENGTH;

#endif /* AWS_CLIENT_CREDENTIAL_KEYS_H */
