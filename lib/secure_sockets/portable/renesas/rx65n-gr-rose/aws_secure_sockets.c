/*
 * Amazon FreeRTOS Secure Socket V1.0.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/**
 * @file aws_secure_sockets.c
 * @brief WiFi and Secure Socket interface implementation.
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "list.h"
#include "FreeRTOS_IP.h"
#include "aws_secure_sockets.h"
#include "FreeRTOS_Sockets.h"
#include "aws_tls.h"
#include "task.h"

/* Internal context structure. */
typedef struct SSOCKETContext
{
    Socket_t xSocket;
    char * pcDestination;
    void * pvTLSContext;
    BaseType_t xRequireTLS;
    BaseType_t xSendFlags;
    BaseType_t xRecvFlags;
    char * pcServerCertificate;
    uint32_t ulServerCertificateLength;
} SSOCKETContext_t, * SSOCKETContextPtr_t;

/**
 * @brief Maximum number of sockets.
 *
 * 16 total sockets
 */
#define MAX_NUM_SSOCKETS    (16)

/**
 * @brief Number of secure sockets allocated.
 *
 * Keep a count of the number of open sockets.
 */
static uint8_t ssockets_num_allocated = 0;


/*
 * @brief Network send callback.
 */
static BaseType_t prvNetworkSend( void * pvContext,
                                  const unsigned char * pucData,
                                  size_t xDataLength )
{
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) pvContext; /*lint !e9087 cast used for portability. */

    return FreeRTOS_send( pxContext->xSocket, pucData, xDataLength, pxContext->xSendFlags );
}
/*-----------------------------------------------------------*/

/*
 * @brief Network receive callback.
*/
static BaseType_t prvNetworkRecv( void * pvContext,
                                 unsigned char * pucReceiveBuffer,
                                 size_t xReceiveLength )
{
   SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) pvContext; /*lint !e9087 cast used for portability. */

   return FreeRTOS_recv( pxContext->xSocket, pucReceiveBuffer, xReceiveLength, pxContext->xRecvFlags );
}

/*-----------------------------------------------------------*/

/**
 * @brief Creates a TCP socket.
 *
 * This call allocates memory and claims a socket resource.
 *
 * @sa SOCKETS_Close()
 *
 * @param[in] lDomain Must be set to SOCKETS_AF_INET. See @ref SocketDomains.
 * @param[in] lType Set to SOCKETS_SOCK_STREAM to create a TCP socket.
 * No other value is valid.  See @ref SocketTypes.
 * @param[in] lProtocol Set to SOCKETS_IPPROTO_TCP to create a TCP socket.
 * No other value is valid. See @ref Protocols.
 *
 * @return
 * * If a socket is created successfully, then the socket handle is
 * returned
 * * @ref SOCKETS_INVALID_SOCKET is returned if an error occurred.
 */
Socket_t SOCKETS_Socket( int32_t lDomain,
                         int32_t lType,
                         int32_t lProtocol )
{
    int32_t lStatus = SOCKETS_ERROR_NONE;
    SSOCKETContextPtr_t pxContext = NULL;

    /* Ensure that only supported values are supplied. */
    configASSERT( lDomain == SOCKETS_AF_INET );
    configASSERT( lType == SOCKETS_SOCK_STREAM );
    configASSERT( lProtocol == SOCKETS_IPPROTO_TCP );

	if(ssockets_num_allocated >= MAX_NUM_SSOCKETS)
	{
        lStatus = SOCKETS_SOCKET_ERROR;
	}

	 if( SOCKETS_ERROR_NONE == lStatus )
	 {
		/* Allocate the internal context structure. */
		if( NULL == ( pxContext = pvPortMalloc( sizeof( SSOCKETContext_t ) ) ) )
		{
			lStatus = SOCKETS_ENOMEM;
		}
	}

    if( SOCKETS_ERROR_NONE == lStatus )
    {
        memset( pxContext, 0, sizeof( SSOCKETContext_t ) );

        /* Create the wrapped socket. */
        if( FREERTOS_INVALID_SOCKET == /*lint !e923 cast used for portability. */
            ( pxContext->xSocket = FreeRTOS_socket( lDomain, lType, lProtocol ) ) )
        {
            lStatus = SOCKETS_SOCKET_ERROR;
        }
    }

    if( SOCKETS_ERROR_NONE != lStatus )
    {
        vPortFree( pxContext );
        return SOCKETS_INVALID_SOCKET;
    }

    else
    {
    	if(ssockets_num_allocated < MAX_NUM_SSOCKETS)
    	{
    		ssockets_num_allocated++;
    	}
    	return pxContext;
    }
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Bind( Socket_t xSocket,
                      SocketsSockaddr_t * pxAddress,
                      Socklen_t xAddressLength )
{
    /* FIX ME. */
    return SOCKETS_SOCKET_ERROR;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Listen( Socket_t xSocket,
                        int32_t lBacklog )
{
    /* FIX ME. */
    return SOCKETS_SOCKET_ERROR;
}
/*-----------------------------------------------------------*/

Socket_t SOCKETS_Accept( Socket_t xSocket,
                         SocketsSockaddr_t * pxAddress,
                         Socklen_t * pxAddressLength )
{
    /* FIX ME. */
    return SOCKETS_INVALID_SOCKET;
}
/*-----------------------------------------------------------*/

/**
 * @brief Connects the socket to the specified IP address and port.
 *
 * The socket must first have been successfully created by a call to SOCKETS_Socket().
 *
 * @param[in] xSocket The handle of the socket to be connected.
 * @param[in] pxAddress A pointer to a SocketsSockaddr_t structure that contains the
 * the address to connect the socket to.
 * @param[in] xAddressLength Should be set to sizeof( @ref SocketsSockaddr_t ).
 *
 * @return
 * * @ref SOCKETS_ERROR_NONE if a connection is established.
 * * If an error occured, a negative value is returned. @ref SocketsErrors
 */
int32_t SOCKETS_Connect( Socket_t xSocket,
                         SocketsSockaddr_t * pxAddress,
                         Socklen_t xAddressLength )
{
    int32_t lStatus = SOCKETS_ERROR_NONE;
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) xSocket; /*lint !e9087 cast used for portability. */
    TLSParams_t xTLSParams = { 0 };
    struct freertos_sockaddr xTempAddress = { 0 };

    if( ( pxContext != SOCKETS_INVALID_SOCKET ) && ( pxAddress != NULL ) )
    {
        /* Connect the wrapped socket. */
        xTempAddress.sin_addr = pxAddress->ulAddress;
        xTempAddress.sin_family = pxAddress->ucSocketDomain;
        xTempAddress.sin_len = ( uint8_t ) sizeof( xTempAddress );
        xTempAddress.sin_port = pxAddress->usPort;
        lStatus = FreeRTOS_connect( pxContext->xSocket, &xTempAddress, xAddressLength );

        /* Negotiate TLS if requested. */
        if( ( SOCKETS_ERROR_NONE == lStatus ) && ( pdTRUE == pxContext->xRequireTLS ) )
        {
            xTLSParams.ulSize = sizeof( xTLSParams );
            xTLSParams.pcDestination = pxContext->pcDestination;
            xTLSParams.pcServerCertificate = pxContext->pcServerCertificate;
            xTLSParams.ulServerCertificateLength = pxContext->ulServerCertificateLength;
            xTLSParams.pvCallerContext = pxContext;
            xTLSParams.pxNetworkRecv = prvNetworkRecv;
            xTLSParams.pxNetworkSend = prvNetworkSend;
            lStatus = TLS_Init( &pxContext->pvTLSContext, &xTLSParams );

            if( SOCKETS_ERROR_NONE == lStatus )
            {
                lStatus = TLS_Connect( pxContext->pvTLSContext );
            }
        }
    }
    else
    {
        lStatus = SOCKETS_SOCKET_ERROR;
    }

    return lStatus;
}
/*-----------------------------------------------------------*/

/**
 * @brief Receive data from a TCP socket.
 *
 * The socket must have already been created using a call to SOCKETS_Socket()
 * and connected to a remote socket using SOCKETS_Connect().
 *
 * @param[in] xSocket The handle of the socket from which data is being received.
 * @param[out] pvBuffer The buffer into which the received data will be placed.
 * @param[in] xBufferLength The maximum number of bytes which can be received.
 * pvBuffer must be at least xBufferLength bytes long.
 * @param[in] ulFlags Not currently used. Should be set to 0.
 *
 * @return
 * * If the receive was successful then the number of bytes received (placed in the
 *   buffer pointed to by pvBuffer) is returned.
 * * If a timeout occurred before data could be received then 0 is returned (timeout
 *   is set using @ref SOCKETS_SO_RCVTIMEO).
 * * If an error occured, a negative value is returned. @ref SocketsErrors
 */
int32_t SOCKETS_Recv( Socket_t xSocket,
                      void * pvBuffer,
                      size_t xBufferLength,
                      uint32_t ulFlags )
{
    int32_t lStatus = SOCKETS_SOCKET_ERROR;
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) xSocket; /*lint !e9087 cast used for portability. */

    pxContext->xRecvFlags = ( BaseType_t ) ulFlags;

    if( ( xSocket != SOCKETS_INVALID_SOCKET ) &&
        ( pvBuffer != NULL ) )
    {
        if( pdTRUE == pxContext->xRequireTLS )
        {
            /* Receive through TLS pipe, if negotiated. */
            lStatus = TLS_Recv( pxContext->pvTLSContext, pvBuffer, xBufferLength );
        }
        else
        {
            /* Receive unencrypted. */
            lStatus = prvNetworkRecv( pxContext, pvBuffer, xBufferLength );
        }
    }

    return lStatus;
}
/*-----------------------------------------------------------*/

/**
 * @brief Transmit data to the remote socket.
 *
 * The socket must have already been created using a call to SOCKETS_Socket() and
 * connected to a remote socket using SOCKETS_Connect().
 *
 * @param[in] xSocket The handle of the sending socket.
 * @param[in] pvBuffer The buffer containing the data to be sent.
 * @param[in] xDataLength The length of the data to be sent.
 * @param[in] ulFlags Not currently used. Should be set to 0.
 *
 * @return
 * * On success, the number of bytes actually sent is returned.
 * * If an error occured, a negative value is returned. @ref SocketsErrors
 */
int32_t SOCKETS_Send( Socket_t xSocket,
                      const void * pvBuffer,
                      size_t xDataLength,
                      uint32_t ulFlags )
{
    int32_t lStatus = SOCKETS_SOCKET_ERROR;
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) xSocket; /*lint !e9087 cast used for portability. */

    if( ( xSocket != SOCKETS_INVALID_SOCKET ) &&
        ( pvBuffer != NULL ) )
    {
        pxContext->xSendFlags = ( BaseType_t ) ulFlags;

        if( pdTRUE == pxContext->xRequireTLS )
        {
            /* Send through TLS pipe, if negotiated. */
            lStatus = TLS_Send( pxContext->pvTLSContext, pvBuffer, xDataLength );
        }
        else
        {
            /* Send unencrypted. */
            lStatus = prvNetworkSend( pxContext, pvBuffer, xDataLength );
        }
    }

    return lStatus;
}
/*-----------------------------------------------------------*/

/**
 * @brief Closes all or part of a full-duplex connection on the socket.
 *
 * @param[in] xSocket The handle of the socket to shutdown.
 * @param[in] ulHow SOCKETS_SHUT_RD, SOCKETS_SHUT_WR or SOCKETS_SHUT_RDWR.
 * @ref ShutdownFlags
 *
 * @return
 * * If the operation was successful, 0 is returned.
 * * If an error occured, a negative value is returned. @ref SocketsErrors
 */
int32_t SOCKETS_Shutdown( Socket_t xSocket,
                          uint32_t ulHow )
{
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) xSocket; /*lint !e9087 cast used for portability. */

    return FreeRTOS_shutdown( pxContext->xSocket, ( BaseType_t ) ulHow );
}
/*-----------------------------------------------------------*/

/**
 * @brief Closes the socket and frees the related resources.
 *
 * @param[in] xSocket The handle of the socket to close.
 *
 * @return
 * * On success, 0 is returned.
 * * If an error occurred, a negative value is returned. @ref SocketsErrors
 */
int32_t SOCKETS_Close( Socket_t xSocket )
{
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) xSocket; /*lint !e9087 cast used for portability. */

    if( NULL != pxContext )
    {
        if( NULL != pxContext->pcDestination )
        {
            vPortFree( pxContext->pcDestination );
        }

        if( NULL != pxContext->pcServerCertificate )
        {
            vPortFree( pxContext->pcServerCertificate );
        }

        if( pdTRUE == pxContext->xRequireTLS )
        {
            TLS_Cleanup( pxContext->pvTLSContext );
        }

        ( void ) FreeRTOS_closesocket( pxContext->xSocket );
        vPortFree( pxContext );
    }

	if(ssockets_num_allocated > 0)
	{
		ssockets_num_allocated--;
	}
	else
	{
		ssockets_num_allocated = 0;
	}
    return pdFREERTOS_ERRNO_NONE;
}
/*-----------------------------------------------------------*/

/**
 * @brief Manipulates the options for the socket.
 *
 * @param[in] xSocket The handle of the socket to set the option for.
 * @param[in] lLevel Not currently used. Should be set to 0.
 * @param[in] lOptionName See @ref SetSockOptOptions.
 * @param[in] pvOptionValue A buffer containing the value of the option to set.
 * @param[in] xOptionLength The length of the buffer pointed to by pvOptionValue.
 *
 * @note Socket option support and possible values vary by port. Please see
 * PORT_SPECIFIC_LINK to check the valid options and limitations of your device.
 *
 *  - Berkeley Socket Options
 *    - @ref SOCKETS_SO_RCVTIMEO
 *      - Sets the receive timeout
 *      - pvOptionValue (TickType_t) is the number of milliseconds that the
 *      receive function should wait before timing out.
 *      - Setting pvOptionValue = 0 causes receive to wait forever.
 *      - See PORT_SPECIFIC_LINK for device limitations.
 *    - @ref SOCKETS_SO_SNDTIMEO
 *      - Sets the send timeout
 *      - pvOptionValue (TickType_t) is the number of milliseconds that the
 *      send function should wait before timing out.
 *      - Setting pvOptionValue = 0 causes send to wait forever.
 *      - See PORT_SPECIFIC_LINK for device limitations.
 *  - Non-Standard Options
 *    - @ref SOCKETS_SO_NONBLOCK
 *      - Makes a socket non-blocking.
 *      - pvOptionValue is ignored for this option.
 *  - Security Sockets Options
 *    - @ref SOCKETS_SO_REQUIRE_TLS
 *      - Use TLS for all connect, send, and receive on this socket.
 *      - This socket options MUST be set for TLS to be used, even
 *        if other secure socket options are set.
 *      - pvOptionValue is ignored for this option.
 *    - @ref SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE
 *      - Set the root of trust server certificiate for the socket.
 *      - This socket option only takes effect if @ref SOCKETS_SO_REQUIRE_TLS
 *        is also set.  If @ref SOCKETS_SO_REQUIRE_TLS is not set,
 *        this option will be ignored.
 *      - pvOptionValue is a pointer to the formatted server certificate.
 *        TODO: Link to description of how to format certificates with \n
 *      - xOptionLength (BaseType_t) is the length of the certificate
 *        in bytes.
 *    - @ref SOCKETS_SO_SERVER_NAME_INDICATION
 *      - Use Server Name Indication (SNI)
 *      - This socket option only takes effect if @ref SOCKETS_SO_REQUIRE_TLS
 *        is also set.  If @ref SOCKETS_SO_REQUIRE_TLS is not set,
 *        this option will be ignored.
 *      - pvOptionValue is a pointer to a string containing the hostname
 *      - xOptionLength is the length of the hostname string in bytes.
 *
 * @return
 * * On success, 0 is returned.
 * * If an error occured, a negative value is returned. @ref SocketsErrors
 */
int32_t SOCKETS_SetSockOpt( Socket_t xSocket,
                            int32_t lLevel,
                            int32_t lOptionName,
                            const void * pvOptionValue,
                            size_t xOptionLength )
{
    int32_t lStatus = pdFREERTOS_ERRNO_NONE;
    TickType_t xTimeout;
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) xSocket; /*lint !e9087 cast used for portability. */

    switch( lOptionName )
    {
        case SOCKETS_SO_SERVER_NAME_INDICATION:

            /* Non-NULL destination string indicates that SNI extension should
             * be used during TLS negotiation. */
            if( NULL == ( pxContext->pcDestination =
                              ( char * ) pvPortMalloc( 1U + xOptionLength ) ) )
            {
                lStatus = pdFREERTOS_ERRNO_ENOMEM;
            }
            else
            {
                memcpy( pxContext->pcDestination, pvOptionValue, xOptionLength );
                pxContext->pcDestination[ xOptionLength ] = '\0';
            }

            break;

        case SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE:

            /* Non-NULL server certificate field indicates that the default trust
             * list should not be used. */
            if( NULL == ( pxContext->pcServerCertificate =
                              ( char * ) pvPortMalloc( xOptionLength ) ) )
            {
                lStatus = pdFREERTOS_ERRNO_ENOMEM;
            }
            else
            {
                memcpy( pxContext->pcServerCertificate, pvOptionValue, xOptionLength );
                pxContext->ulServerCertificateLength = xOptionLength;
            }

            break;

        case SOCKETS_SO_REQUIRE_TLS:
            pxContext->xRequireTLS = pdTRUE;
            break;

        case SOCKETS_SO_NONBLOCK:
            xTimeout = 0;
            lStatus = FreeRTOS_setsockopt( pxContext->xSocket,
                                           lLevel,
                                           SOCKETS_SO_RCVTIMEO,
                                           &xTimeout,
                                           sizeof( xTimeout ) );

            if( lStatus == SOCKETS_ERROR_NONE )
            {
                lStatus = FreeRTOS_setsockopt( pxContext->xSocket,
                                               lLevel,
                                               SOCKETS_SO_SNDTIMEO,
                                               &xTimeout,
                                               sizeof( xTimeout ) );
            }

            break;

        case SOCKETS_SO_RCVTIMEO:
        case SOCKETS_SO_SNDTIMEO:
            /* Comply with Berkeley standard - a 0 timeout is wait forever. */
            xTimeout = *( ( const TickType_t * ) pvOptionValue ); /*lint !e9087 pvOptionValue passed should be of TickType_t */

            if( xTimeout == 0U )
            {
                xTimeout = portMAX_DELAY;
            }

            lStatus = FreeRTOS_setsockopt( pxContext->xSocket,
                                           lLevel,
                                           lOptionName,
                                           &xTimeout,
                                           xOptionLength );
            break;

        default:
            lStatus = FreeRTOS_setsockopt( pxContext->xSocket,
                                           lLevel,
                                           lOptionName,
                                           pvOptionValue,
                                           xOptionLength );
            break;
    }

    return lStatus;
}
/*-----------------------------------------------------------*/

/**
 * @brief Resolve a host name using Domain Name Service.
 *
 * @param[in] pcHostName The host name to resolve.
 * @return
 * * The IPv4 address of the specified host.
 * * If an error has occured, 0 is returned.
 */
uint32_t SOCKETS_GetHostByName( const char * pcHostName )
{
    return FreeRTOS_gethostbyname( pcHostName );
}
/*-----------------------------------------------------------*/

/**
 * @brief Secure Sockets library initialization function.
 *
 * This function does general initialization and setup. It must be called once
 * and only once before calling any other function.
 *
 * @return
 * * @ref pdPASS if everything succeeds
 * * @ref pdFAIL otherwise.
 */
BaseType_t SOCKETS_Init( void )
{
    /* FIX ME. */
    /* Empty initialization */
    return pdPASS;
}
/*-----------------------------------------------------------*/


