#include <stdio.h>
#include <string.h>

#include "platform.h"
#include "r_sci_rx_if.h"
#include "sx_ulpgn_driver.h"



const uint8_t ulpgn_return_text_ok[]          = ULPGN_RETURN_TEXT_OK;
const uint8_t ulpgn_return_text_connect[]     = ULPGN_RETURN_TEXT_CONNECT;
const uint8_t ulpgn_return_text_ring[]        = ULPGN_RETURN_TEXT_RING;
const uint8_t ulpgn_return_text_no_carrier[]  = ULPGN_RETURN_TEXT_NO_CARRIER;
const uint8_t ulpgn_return_text_error[]       = ULPGN_RETURN_TEXT_ERROR;
const uint8_t ulpgn_return_text_no_dialtone[] = ULPGN_RETURN_TEXT_NO_DIALTONE;
const uint8_t ulpgn_return_text_busy[]        = ULPGN_RETURN_TEXT_BUSY;
const uint8_t ulpgn_return_text_no_answer[]   = ULPGN_RETURN_TEXT_NO_ANSWER;

const uint8_t ulpgn_return_numeric_ok[]          = ULPGN_RETURN_NUMERIC_OK;
const uint8_t ulpgn_return_numeric_connect[]     = ULPGN_RETURN_NUMERIC_CONNECT;
const uint8_t ulpgn_return_numeric_ring[]        = ULPGN_RETURN_NUMERIC_RING;
const uint8_t ulpgn_return_numeric_no_carrier[]  = ULPGN_RETURN_NUMERIC_NO_CARRIER;
const uint8_t ulpgn_return_numeric_error[]       = ULPGN_RETURN_NUMERIC_ERROR;
const uint8_t ulpgn_return_numeric_no_dialtone[] = ULPGN_RETURN_NUMERIC_NO_DIALTONE;
const uint8_t ulpgn_return_numeric_busy[]        = ULPGN_RETURN_NUMERIC_BUSY;
const uint8_t ulpgn_return_numeric_no_answer[]   = ULPGN_RETURN_NUMERIC_NO_ANSWER;

const uint8_t ulpgn_return_dummy[]   = "";

const uint8_t *ulpgn_result_code[ULPGN_RETURN_ENUM_MAX][ULPGN_RETURN_STRING_MAX] =
{
	/* text mode*/                  /* numeric mode */
	{ulpgn_return_text_ok,          ulpgn_return_numeric_ok          },
	{ulpgn_return_text_connect,     ulpgn_return_numeric_connect     },
	{ulpgn_return_text_ring,        ulpgn_return_numeric_ring        },
	{ulpgn_return_text_no_carrier,  ulpgn_return_numeric_no_carrier  },
	{ulpgn_return_text_error,       ulpgn_return_numeric_error       },
	{ulpgn_return_dummy,            ulpgn_return_dummy               },
	{ulpgn_return_text_no_dialtone, ulpgn_return_numeric_no_dialtone },
	{ulpgn_return_text_busy,        ulpgn_return_numeric_busy        },
	{ulpgn_return_text_no_answer,   ulpgn_return_numeric_no_answer   },
};



uint8_t buff[1000];
uint8_t recvbuff[1000];
sci_cfg_t   g_sx_ulpgn_sci_config;
sci_err_t   g_sx_ulpgn_sci_err;
uint8_t g_sx_ulpgn_return_mode;
uint8_t socket_create_flag;
static void sx_ulpgn_uart_callback(void *pArgs);
static void timeout_init(uint16_t timeout_ms);
static void bytetimeout_init(uint16_t timeout_ms);
static int32_t check_timeout(uint32_t rcvcount);
static int32_t check_bytetimeout(uint32_t rcvcount);
static int32_t sx_ulpgn_serial_open(void);
static int32_t sx_ulpgn_serial_send_basic(uint8_t *ptextstring, uint16_t response_type, uint16_t timeout_ms, sx_ulpgn_return_code_t expect_code);
static sci_hdl_t sx_ulpgn_uart_sci_handle;
static TickType_t starttime, thistime, endtime;
static uint8_t timeout_overflow_flag;
static TickType_t startbytetime, thisbytetime, endbytetime;
static uint8_t byte_timeout_overflow_flag;
static TickType_t g_sl_ulpgn_tcp_timeout = 3000;		/* ## slowly problem ## unit: 1ms */
volatile uint32_t g_sx_ulpgn_uart_teiflag;

int32_t sx_ulpgn_init(void)
{
	int32_t ret;
	ret = sx_ulpgn_serial_open();
	if(ret != 0)
	{
		return ret;
	}

	/* Wifi Module hardware reset   */
	PORTF.PDR.BIT.B5 = 1;
	PORTF.PODR.BIT.B5 = 0; /* Low */
	R_BSP_SoftwareDelay(26, BSP_DELAY_MILLISECS); /* 5us mergin 1us */
	PORTF.PODR.BIT.B5 = 1; /* High */
//	R_BSP_SoftwareDelay(26, BSP_DELAY_MILLISECS); /* 5us mergin 1us */

	ret = sx_ulpgn_serial_send_basic("ATZ\r", 1000, 2000, ULPGN_RETURN_OK);
	if(ret != 0)
	{
		ret = sx_ulpgn_serial_send_basic("ATZ\r", 1000, 2000, ULPGN_RETURN_OK);
		if(ret != 0)
		{
			return ret;
		}
	}
	g_sx_ulpgn_return_mode = 0;

	ret = sx_ulpgn_serial_send_basic("ATE0\r", 3, 200, ULPGN_RETURN_OK);
	if(ret != 0)
	{
		return ret;
	}
	g_sx_ulpgn_return_mode = 1;
	ret = sx_ulpgn_serial_send_basic("ATV0\r", 3, 200, ULPGN_RETURN_OK);
	if(ret != 0)
	{
		return ret;
	}

	ret = sx_ulpgn_serial_send_basic("ATS12=1\r", 3, 200, ULPGN_RETURN_OK);
	if(ret != 0)
	{
		return ret;
	}

	ret = sx_ulpgn_serial_send_basic("ATWD\r", 3, 200, ULPGN_RETURN_OK);
	if(ret != 0)
	{
		return ret;
	}

	return 0;
}

int32_t sx_ulpgn_wifi_connect(uint8_t *pssid, uint32_t security, uint8_t *ppass)
{
	int32_t ret;
	char *pstr,pstr2;
	int32_t line_len;
	int32_t scanf_ret;
	volatile char secu[3][10];
	uint32_t security_encrypt_type = 1;

#if 0
	pstr = recvbuff;
    /* FIX ME. */
	ret = sx_ulpgn_serial_send_basic("ATWS\r", 3, 1000, ULPGN_RETURN_OK);
	if(0 != ret)
	{
		return -1;
	}
	while(1)
	{
		pstr = strstr(pstr, pssid);
		if(pstr != NULL)
		{
			if(*(pstr + strlen(pssid)) == '\r')
			{
				/* find */
				pstr = strstr(pstr,"security = \r\n");
				pstr += strlen("security = \r\n");
				pstr2 = strstr(pstr,"\r\n");
				pstr2 = '\0';
				/* type */
				if(NULL != strstr(pstr,"RSN/WPA2="))
				{
					if(security != ULPGN_SECURITY_WPA2)
					{
						return -1;
					}
				}
				else if(NULL != strstr(pstr,"WPA="))
				{
					if(security != ULPGN_SECURITY_WPA)
					{
						return -1;
					}
				}
				else
				{
					return -1;
				}

				if(NULL != strstr(pstr,"PSK"))
				{
				}
				else
				{
					return -1;
				}
				if(NULL != strstr(pstr,"AES"))
				{
					security_encrypt_type = 1;
				}
				else if(NULL != strstr(pstr,"TKIP"))
				{
					security_encrypt_type = 0;
				}
				else
				{
					return -1;
				}
			}
			else
			{
				pstr += strlen(pssid);
			}
		}
		else
		{
			break;
		}
	}
#endif
	strcpy((char *)buff,"ATWAWPA=");
	strcat((char *)buff,(const char *)pssid);
	strcat((char *)buff,",");
	if(security != ULPGN_SECURITY_WPA && security != ULPGN_SECURITY_WPA2)
	{
		return -1;
	}
	if(security == ULPGN_SECURITY_WPA)
	{
		strcat((char *)buff,"1,");
	}
	else
	{
		strcat((char *)buff,"2,");
	}
	if(security_encrypt_type == 1)
	{
		strcat((char *)buff,"1,1,");
	}
	else
	{
		strcat((char *)buff,"0,0,");
	}
	strcat((char *)buff,(const char *)ppass);
	strcat((char *)buff,"\r");

	ret = sx_ulpgn_serial_send_basic(buff, 3, 5000, ULPGN_RETURN_OK);
	if(0 == ret)
	{
		sx_ulpgn_serial_send_basic("ATW\r", 3, 1000, ULPGN_RETURN_OK);
	}
	return ret;
}

int32_t sx_ulpgn_wifi_get_macaddr(uint8_t *ptextstring)
{
//	return sx_ulpgn_serial_send_basic("ATW\r", 0, 10000);
	return sx_ulpgn_serial_send_basic("ATNSET=\?\r", 3, 200, ULPGN_RETURN_OK);
}

int32_t sx_ulpgn_socket_create(uint32_t type,uint32_t ipversion)
{
	int32_t ret;
	ret = sx_ulpgn_serial_send_basic("ATNSTAT=\r", 3, 200, ULPGN_RETURN_OK);

	sprintf((char *)buff,"ATNSOCK=%d,%d\r",(uint8_t)(type),(uint8_t)(ipversion));

	ret = sx_ulpgn_serial_send_basic(buff, 3, 200, ULPGN_RETURN_OK);
	if(ret != 0)
	{
		return ret;
	}
	socket_create_flag = 1;
//	ret = sx_ulpgn_serial_send_basic("ATNSTAT=\r", 3, 200, ULPGN_RETURN_OK);

	return 0;
}


int32_t sx_ulpgn_tcp_connect(uint32_t ipaddr, uint16_t port)
{
	strcpy((char *)buff,"ATNCTCP=");
	sprintf((char *)buff+strlen((char *)buff),"%d.%d.%d.%d,%d\r",(uint8_t)(ipaddr>>24),(uint8_t)(ipaddr>>16),(uint8_t)(ipaddr>>8),(uint8_t)(ipaddr),port);

	return sx_ulpgn_serial_send_basic(buff, 3, 10000, ULPGN_RETURN_CONNECT);
}

int32_t sx_ulpgn_tcp_send(uint8_t *pdata, int32_t length)
{
	int32_t timeout;
	int32_t sended_length, lenghttmp1;
	sci_err_t ercd;
//	sci_baud_t baud;

	sended_length = 0;
	timeout_init(g_sl_ulpgn_tcp_timeout);

	timeout = 0;

	while(sended_length < length)
	{
		if(length - sended_length > SCI_CFG_CH6_TX_BUFSIZ)
		{
			lenghttmp1 = SCI_CFG_CH6_TX_BUFSIZ;
		}
		else
		{
			lenghttmp1 = length - sended_length;
		}
		g_sx_ulpgn_uart_teiflag = 0;
		ercd = R_SCI_Send(sx_ulpgn_uart_sci_handle, pdata, lenghttmp1);
		if(SCI_SUCCESS != ercd)
		{
			return -1;
		}

		while(1)
		{
			if(0 != g_sx_ulpgn_uart_teiflag)
			{
				break;
			}
			if(-1 == check_timeout(0))
			{
				timeout = 1;
				break;
			}
		}
		if(timeout == 1)
		{
			return -1;
		}
		sended_length += lenghttmp1;
	}
	if(timeout == 1 )
	{
		return -1;
	}

#if DEBUGLOG == 1
	printf("tcp %d byte send\r\n",length);
#endif
	return length;
}

int32_t sx_ulpgn_tcp_recv(uint8_t *pdata, int32_t length)
{
	int32_t timeout;
	sci_err_t ercd;
	uint32_t recvcnt = 0;

	timeout_init(g_sl_ulpgn_tcp_timeout);

	timeout = 0;
	while(1)
	{
		ercd = R_SCI_Receive(sx_ulpgn_uart_sci_handle, (pdata + recvcnt), 1);
		if(SCI_SUCCESS == ercd)
		{
			recvcnt++;
			if(recvcnt >= length)
			{
				break;
			}
		}
		if(-1 == check_timeout(0))
		{
			timeout = 1;
			break;
		}
	}
//	if(timeout == 1 && recvcnt == 0)
//	{
//		return -1;
//	}
#if DEBUGLOG == 1
	printf("tcp %d byte received.reqsize=%d\r\n",recvcnt, length);
#endif

	return recvcnt;
}

int32_t sx_ulpgn_serial_tcp_timeout_set(TickType_t timeout_ms)
{
	g_sl_ulpgn_tcp_timeout = timeout_ms;
	return 0;
}

int32_t sx_ulpgn_tcp_disconnect(void)
{
	int32_t ret = 0;
	if(1 == socket_create_flag)
	{
		R_BSP_SoftwareDelay(22, BSP_DELAY_MILLISECS); /* 1s */
		ret = sx_ulpgn_serial_send_basic("+++", 3, 200, ULPGN_RETURN_OK);

		ret = sx_ulpgn_serial_send_basic("ATNCLOSE\r", 3, 200, ULPGN_RETURN_OK);
		if(0 == ret)
		{
			socket_create_flag = 0;
		}
	}
	return ret;

}

int32_t sx_ulpgn_dns_query(uint8_t *ptextstring, uint32_t *ulipaddr)
{
	uint32_t result;
	uint32_t ipaddr[4];
	int32_t func_ret;
	int32_t scanf_ret;
	strcpy((char *)buff,"ATNDNSQUERY=");
	sprintf((char *)buff+strlen((char *)buff),"%s\r",ptextstring);

	func_ret = sx_ulpgn_serial_send_basic(buff, 3, 3000, ULPGN_RETURN_OK);
	if(func_ret != 0)
	{
		return -1;
	}
	scanf_ret = sscanf((const char *)recvbuff, "%d\r\n%d.%d.%d.%d\r\n",&result, &ipaddr[0], &ipaddr[1], &ipaddr[2], &ipaddr[3]);
	if(scanf_ret != 5)
	{
		return -1;
	}
	if(result != 1)
	{
		return -1;
	}
	*ulipaddr = (((uint32_t)ipaddr[0]) << 24) | (((uint32_t)ipaddr[1]) << 16) | (((uint32_t)ipaddr[2]) << 8) | ((uint32_t)ipaddr[3]);
	return 0;
}


static int32_t sx_ulpgn_serial_send_basic(uint8_t *ptextstring, uint16_t response_type, uint16_t timeout_ms, sx_ulpgn_return_code_t expect_code )
{
#if DEBUGLOG == 1
	TickType_t tmptime1,tmptime2;
#endif
	volatile int32_t timeout;
	sci_err_t ercd;
	uint32_t recvcnt = 0;

	timeout_init(timeout_ms);

	timeout = 0;
	recvcnt = 0;
	g_sx_ulpgn_uart_teiflag = 0;
	ercd = R_SCI_Send(sx_ulpgn_uart_sci_handle, ptextstring, strlen((const char *)ptextstring));
	if(SCI_SUCCESS != ercd)
	{
		return -1;
	}

	while(1)
	{
		if(0 != g_sx_ulpgn_uart_teiflag)
//		ercd = R_SCI_Control(sx_ulpgn_uart_sci_handle, SCI_CMD_TX_Q_BYTES_FREE, &non_used);
//		if(non_used == SCI_CFG_CH6_TX_BUFSIZ)
		{
			break;
		}
		if(-1 == check_timeout(recvcnt))
		{
			timeout = 1;
			break;
		}
	}
	if(timeout == 1)
	{
		return -1;
	}

#if DEBUGLOG == 1
	tmptime1 = xTaskGetTickCount();
	if(ptextstring[strlen((const char *)ptextstring)-1] != '\r')
	{
		printf("s:%06d:%s\r",tmptime1,ptextstring);
	}
	else
	{
		printf("s:%06d:%s",tmptime1,ptextstring);
	}
#endif
	while(1)
	{
		ercd = R_SCI_Receive(sx_ulpgn_uart_sci_handle, &recvbuff[recvcnt], 1);
		if(SCI_SUCCESS == ercd)
		{
			recvcnt++;
			bytetimeout_init(response_type);
			if(recvcnt < 4)
			{
				continue;
			}
			if(recvcnt == sizeof(recvbuff))
			{
				break;
			}
		}
		if(-1 == check_bytetimeout(recvcnt))
		{
			break;
		}
		if(-1 == check_timeout(recvcnt))
		{
			timeout = 1;
			break;
		}
	}
	if(timeout == 1)
	{
		return -1;
	}

#if DEBUGLOG == 1
	tmptime2 = xTaskGetTickCount();
	if(recvbuff[recvcnt-1] != '\r')
	{
		recvbuff[recvcnt] = '\r';
		recvbuff[recvcnt+1] = '\0';
	}
	else
	{
		recvbuff[recvcnt] = '\0';
	}
	printf("r:%06d:%s",tmptime2,recvbuff);
#endif
	/* Response data check */
	if(recvcnt < strlen((const char *)ulpgn_result_code[expect_code][g_sx_ulpgn_return_mode]))
	{
		return -1;
	}
	if(0 != strncmp((const char *)&recvbuff[recvcnt - strlen((const char *)ulpgn_result_code[expect_code][g_sx_ulpgn_return_mode]) ],
			(const char *)ulpgn_result_code[expect_code][g_sx_ulpgn_return_mode], strlen((const char *)ULPGN_RETURN_TEXT_OK)))
	{
		return -1;
	}
	return 0;
}

static void timeout_init(uint16_t timeout_ms)
{
	starttime = xTaskGetTickCount();
	endtime = starttime + timeout_ms;
	if((starttime + endtime) < starttime)
	{
		/* overflow */
		timeout_overflow_flag = 1;
	}
	else
	{
		timeout_overflow_flag = 0;
	}
}

static int32_t check_timeout(uint32_t rcvcount)
{
	if(0 == rcvcount)
	{
		thistime = xTaskGetTickCount();
		if(timeout_overflow_flag == 0)
		{
			if(thistime >= endtime || thistime < starttime)
			{
				return -1;
			}
		}
		else
		{
			if(thistime < starttime && thistime <= endtime)
			{
				/* Not timeout  */
				return -1;
			}
		}
	}
	/* Not timeout  */
	return 0;
}

static void bytetimeout_init(uint16_t timeout_ms)
{
	startbytetime = xTaskGetTickCount();
	endbytetime = startbytetime + timeout_ms;
	if((startbytetime + endbytetime) < startbytetime)
	{
		/* overflow */
		byte_timeout_overflow_flag = 1;
	}
	else
	{
		byte_timeout_overflow_flag = 0;
	}
}

static int32_t check_bytetimeout(uint32_t rcvcount)
{
	if(0 != rcvcount)
	{
		thisbytetime = xTaskGetTickCount();
		if(byte_timeout_overflow_flag == 0)
		{
			if(thisbytetime >= endbytetime || thisbytetime < startbytetime)
			{
				return -1;
			}
		}
		else
		{
			if(thisbytetime < startbytetime && thisbytetime <= endbytetime)
			{
				/* Not timeout  */
				return -1;
			}
		}
	}
	/* Not timeout  */
	return 0;
}

static int32_t sx_ulpgn_serial_open(void)
{
	sci_err_t   my_sci_err;

	R_SCI_PinSet_SCI6();

	g_sx_ulpgn_sci_config.async.baud_rate    = 115200;
	g_sx_ulpgn_sci_config.async.clk_src      = SCI_CLK_INT;
	g_sx_ulpgn_sci_config.async.data_size    = SCI_DATA_8BIT;
	g_sx_ulpgn_sci_config.async.parity_en    = SCI_PARITY_OFF;
	g_sx_ulpgn_sci_config.async.parity_type  = SCI_EVEN_PARITY;
	g_sx_ulpgn_sci_config.async.stop_bits    = SCI_STOPBITS_1;
	g_sx_ulpgn_sci_config.async.int_priority = 3;    // 1=lowest, 15=highest

    my_sci_err = R_SCI_Open(SCI_CH6, SCI_MODE_ASYNC, &g_sx_ulpgn_sci_config, sx_ulpgn_uart_callback, &sx_ulpgn_uart_sci_handle);

    if(SCI_SUCCESS != my_sci_err)
    {
    	return -1;
    }
	return 0;

}

static void sx_ulpgn_uart_callback(void *pArgs)
{
    sci_cb_args_t   *p_args;

    p_args = (sci_cb_args_t *)pArgs;

    if (SCI_EVT_RX_CHAR == p_args->event)
    {
        /* From RXI interrupt; received character data is in p_args->byte */
        nop();
    }
#if SCI_CFG_TEI_INCLUDED
	else if (SCI_EVT_TEI == p_args->event)
	{
		g_sx_ulpgn_uart_teiflag = 1;
		nop();

	}
#endif
    else if (SCI_EVT_RXBUF_OVFL == p_args->event)
    {
        /* From RXI interrupt; rx queue is full; 'lost' data is in p_args->byte
           You will need to increase buffer size or reduce baud rate */
        nop();
    }
    else if (SCI_EVT_OVFL_ERR == p_args->event)
    {
        /* From receiver overflow error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
        nop();
    }
    else if (SCI_EVT_FRAMING_ERR == p_args->event)
    {
        /* From receiver framing error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
        nop();
    }
    else if (SCI_EVT_PARITY_ERR == p_args->event)
    {
        /* From receiver parity error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
        nop();
    }
    else
    {
        /* Do nothing */
    }

} /* End of function my_sci_callback() */
