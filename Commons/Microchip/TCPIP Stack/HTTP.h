/*********************************************************************
 *
 *               HTTP definitions on Microchip TCP/IP Stack
 *
 *********************************************************************
 * FileName:        HTTP.h
 * Dependencies:    None
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.05 or higher
 *					Microchip C30 v3.12 or higher
 *					Microchip C18 v3.30 or higher
 *					HI-TECH PICC-18 PRO 9.63PL2 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2009 Microchip Technology Inc.  All rights
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i)  the Software when embedded on a Microchip microcontroller or
 *      digital signal controller product ("Device") which is
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
 *		ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *		used in conjunction with a Microchip ethernet controller for
 *		the sole purpose of interfacing with the ethernet controller.
 *
 * You should refer to the license agreement accompanying this
 * Software for additional information regarding your rights and
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     8/14/01 Original (Rev. 1.0)
 * Nilesh Rajbharti     2/9/02  Cleanup
 * Nilesh Rajbharti     5/22/02 Rev 2.0 (See version.log for detail)
********************************************************************/
#ifndef __HTTP_H
#define __HTTP_H

#include "../Commons/Microchip/TCPIP Stack/TCPIP.h"
#include "../Commons/Microchip/MDD File System/FSIO.h"
#include "../Commons/Microchip/Microchip.h"
#include "../Commons/Numeric.h"
#include "../Commons/Config.h"
/****************************************************************************
  Section:
	Commands and Server Responses
  ***************************************************************************/

	//Supported Commands and Server Response Codes
	typedef enum
	{
	    HTTP_GET = 0u,					// GET command is being processed
	    HTTP_POST,						// POST command is being processed
	    HTTP_BAD_REQUEST,				// 400 Bad Request will be returned
		HTTP_UNAUTHORIZED,				// 401 Unauthorized will be returned
	    HTTP_NOT_FOUND,					// 404 Not Found will be returned
		HTTP_OVERFLOW,					// 414 Request-URI Too Long will be returned
		HTTP_INTERNAL_SERVER_ERROR,		// 500 Internal Server Error will be returned
		HTTP_NOT_IMPLEMENTED,			// 501 Not Implemented (not a GET or POST command)
		HTTP_REDIRECT,					// 302 Redirect will be returned
		HTTP_SSL_REQUIRED				// 403 Forbidden is returned, indicating SSL is required
	} 
	HTTP_STATUS;
	
// Basic HTTP Connection State Machine
typedef enum
{
	SM_HTTP_UNINITIALIZED = 0u,
	SM_HTTP_IDLE,				// Socket is idle
	SM_HTTP_PARSE_REQUEST,		// Parses the first line for a file name and GET args
	SM_HTTP_PARSE_HEADERS,		// Reads and parses headers one at a time
	SM_HTTP_PROCESS_GET,		// Invokes user callback for GET args or cookies
	SM_HTTP_ERROR,
	SM_HTTP_POST_HEADER,
	SM_HTTP_POST_ARGUMENTS,
	SM_HTTP_DISCONNECTED		// Disconnects the server and closes all files
}
SM_HTTP;

// Stores extended state data for each connection
typedef struct
{
	TCP_SOCKET socket;			// Socket being served
	FSFILE *file;				// File pointer for the file being served
	SM_HTTP StateMachine;		// Current connection state
	HTTP_STATUS Status;			// Request method/status
	bool dynamic;
	uint8_t FileType;				// File type to return with Content-Type
	uint16_t length;
}
HTTP_CONN;




enum
{
	HTTP_DEFAULT_PORT = 80,
	HTTP_DATA_SIZE = 128
};

#define HTTP_Init() ((void) 0)

void HTTP_Server(BOOL available);
void HTTP_FileSystemReset(BOOL safe);

extern unsigned short int HTTP_Port;
extern struct currHTTP;

#endif
