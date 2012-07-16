/*********************************************************************
 *
 *  HyperText Transfer Protocol (HTTP) Server
 *  Module for Microchip TCP/IP Stack
 *   -Serves dynamic pages to web browsers such as Microsoft Internet 
 *    Explorer, Mozilla Firefox, etc.
 *	 -Reference: RFC 2068
 *
 **********************************************************************
 * FileName:        HTTP.c
 * Dependencies:    TCP, MPFS, HTTPGetVar() callback, HTTPExecCmd() callback
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
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     8/14/01     Original
 * Nilesh Rajbharti     9/12/01     Released (Rev. 1.0)
 * Nilesh Rajbharti     2/9/02      Cleanup
 * Nilesh Rajbharti     5/22/02     Rev 2.0 (See version.log for detail)
 * Nilesh Rajbharti     7/9/02      Rev 2.1 (Fixed HTTPParse bug)
 * Howard Schlunder		2/9/05		Fixed variable substitution 
 *									parsing (uses hex now)
 ********************************************************************/
#define __HTTP_C

#include "TCPIPConfig.h"

#if defined(STACK_USE_HTTP_SERVER)

#include "HTTP.h"

// HTTP File Types
#define HTTP_UNKNOWN (sizeof HTTP_FileContent / sizeof *HTTP_FileContent - 1)

static const rom struct HTTP_FileContent
{
	char ext[4];
	const char rom *mime;
	bool cache : 1;
	bool dynamic : 1;
}

// File extension, Content type
HTTP_FileContent[] =
{
	{ { 'T', 'X', 'T' }, "text/plain",               true,  false },	// HTTP_TXT			type: 0
	{ { 'H', 'T', 'M' }, "text/html",                false, true  },	// HTTP_HTM			type: 1
	{ { 'C', 'S', 'S' }, "text/css",	             true,  false },	// HTTP_CSS			type: 2
	{ { 'G', 'I', 'F' }, "image/gif",                true,  false },	// HTTP_GIF
	{ { 'P', 'N', 'G' }, "image/png",                true,  false },	// HTTP_PNG
	{ { 'J', 'P', 'G' }, "image/jpeg",               true,  false },	// HTTP_JPG
	{ { 'J', 'S'      }, "application/java-vm",      true,  false },	// HTTP_JAVA
	{ { '\0',         }, "application/octet-stream", true,  false }		// HTTP_UNKNOWN
};

// Following message order must match with that of HTTP_MESSAGES enum.
typedef enum
{
	HTTP_ERR_FORBIDDEN_POST,
	HTTP_ERR_NOT_FOUND,
	HTTP_ERR_LINE_TOO_LONG,
	HTTP_ERR_CANNOT_CREATE,
	HTTP_ERR_NOT_IMPLEMENTED,
	HTTP_ERR_NOT_AVAILABLE
}
HTTP_Error;

static const char rom *rom HTTP_Errors[] =
{
	"HTTP/1.1 403 Forbidden\r\n\r\n"             "Forbidden filename.\r\n",
	"HTTP/1.1 404 Not Found\r\n\r\n"             "Not found.\r\n",
	"HTTP/1.1 500 Internal Server Error\r\n\r\n" "Header line too long.\r\n",
	"HTTP/1.1 500 Internal Server Error\r\n\r\n" "Cannot create file.\r\n",
	"HTTP/1.1 501 Not Implemented\r\n\r\n"       "Not implemented.\r\n",
	"HTTP/1.1 503 Service Unavailable\r\n\r\n"   "Filesystem not available.\r\n"
};

// Standard HTTP messages.
static ROM BYTE HTTP_OK_STRING[] = "HTTP/1.1 200 OK\r\nContent-Type: ";
static ROM BYTE HTTP_NL_NO_CACHE_STRING[] = "\r\nCache-Control: no-cache";
static ROM BYTE HTTP_NL_DO_CACHE_STRING[] = "\r\nCache-Control: max-age=604800";
static ROM BYTE HTTP_NL_CONTENT_LENGTH_STRING[] = "\r\nContent-Length: ";
static ROM BYTE HTTP_NL_HEADER_END_STRING[] = "\r\n\r\n";
#define HTTP_CONTENT_LENGTH_STRING (HTTP_NL_CONTENT_LENGTH_STRING + 2)

static HTTP_CONN HTTP;
static HTTP_CONN HTTP_Array[MAX_HTTP_CONNECTIONS];
static BYTE currHttpConnection;
static void HTTPInit(void);
static void HTTPLoadConn(BYTE hHTTP);
static void HTTP_Process(bool available);
static bool HTTP_Parse(void);
static bool HTTP_ParseAttribs(void);
static bool HTTP_SendFile(void);
#define HTTP_ParseName (HTTP_Data)

unsigned short int HTTP_Port = HTTP_DEFAULT_PORT;


/*********************************************************************
 * Utility functions for reading through a line of data in the
 * controller's RAM, and saving up identifiers in a buffer
 *********************************************************************/
char HTTP_Data[HTTP_MAX_DATA_LEN + 1];

static struct
{
	char *read;
	char *write;
	unsigned remaining;
}
HTTP_Buffer;

#define HTTP_UngetBuf() (--HTTP_Buffer.read)

static char HTTP_GetBuf(void)
{
	char ch;

retry:
	while(ch = *HTTP_Buffer.read++, !ch)
	{
		unsigned space;

		if(!HTTP_Buffer.remaining)
			break;

		space = &HTTP_Data[HTTP_MAX_DATA_LEN] - HTTP_Buffer.write;

		if(space >= HTTP_Buffer.remaining)
			space = HTTP_Buffer.remaining;

		HTTP_Buffer.read = &HTTP_Data[HTTP_MAX_DATA_LEN] - space;
		HTTP_Buffer.remaining -= space;

		TCPGetArray(HTTP.socket, (BYTE *) HTTP_Buffer.read, space);
	}

	return ch;
}

static void HTTP_PutBuf(char ch)
{
	char *ptr = HTTP_Buffer.write;

	if(ptr != &HTTP_Data[HTTP_MAX_DATA_LEN - 1])
		++HTTP_Buffer.write;

	*ptr++ = ch;
}

static void HTTP_FlushBuf(void)
{
	while(HTTP_Buffer.remaining)
	{
		HTTP_Buffer.read != &HTTP_Data[HTTP_MAX_DATA_LEN];
		HTTP_GetBuf();
	}
}

static char HTTP_SetBuf(unsigned len)
{
	// Clear out any previously buffered data
	HTTP_FlushBuf();

	// Is the desired maximum amount of data already present?
	if(TCPIsGetReady(HTTP.socket) < len)
	{
		// Search for the CRLF deliminating the end of the line 
		// to Verify the entire first line is in the FIFO
		len = TCPFindROMArray(HTTP.socket, (ROM BYTE*)"\r\n", 2, 0, false);

		if(len == 0xFFFFu)
		{
			// Nothing yet..
			if(!TCPGetRxFIFOFree(HTTP.socket))
			{// If the FIFO is full, we overflowed
				HTTP.StateMachine = SM_HTTP_ERROR;
				HTTP.Status = HTTP_OVERFLOW;
				HTTP.FileType = HTTP_ERR_LINE_TOO_LONG;
			}
			return 0;
		}

		// Include the newlines themselves
		len += 2;
	}

	// Prepare the buffer
	HTTP_Buffer.read = &HTTP_Data[HTTP_MAX_DATA_LEN];
	HTTP_Buffer.write = HTTP_Data;
	HTTP_Buffer.remaining = len;
	return 1;
}


/*********************************************************************
 * Switch active socket
 *********************************************************************/
static void HTTPLoadConn(BYTE hHTTP)
{
	static BYTE active = MAX_HTTP_CONNECTIONS;

	if(hHTTP != active)
	{
		memcpy
		(
			&HTTP_Array[active],
			(const void *) &HTTP,
			sizeof HTTP
		);

		memcpy
		(
			&HTTP,
			(const void *) &HTTP_Array[hHTTP],
			sizeof HTTP
		);

		active = hHTTP;
	}
	currHttpConnection = active;
}
/*********************************************************************
 * Function:        void HTTPInit(void)
 *
 * PreCondition:    TCP must already be initialized.
 *
 * Input:           None
 *
 * Output:          HTTP FSM and connections are initialized
 *
 * Side Effects:    None
 *
 * Overview:        Set all HTTP connections to Listening state.
 *                  Initialize FSM for each connection.
 *
 * Note:            This function is called only one during lifetime
 *                  of the application.
 ********************************************************************/
static void HTTPInit(void)
{
	HTTP.socket = TCPOpen
	(
		0,
		TCP_OPEN_SERVER,
		HTTP_Port,
		TCP_PURPOSE_HTTP_SERVER
	);

	HTTP.StateMachine = SM_HTTP_IDLE;
}


/*********************************************************************
 * Function:        void HTTPServer(void)
 *
 * PreCondition:    HTTPInit() must already be called.
 *
 * Input:           None
 *
 * Output:          Opened HTTP connections are served.
 *
 * Side Effects:    None
 *
 * Overview:        Browse through each connections and let it
 *                  handle its connection.
 *                  If a connection is not finished, do not process
 *                  next connections.  This must be done, all
 *                  connections use some static variables that are
 *                  common.
 *
 * Note:            This function acts as a task (similar to one in
 *                  RTOS).  This function performs its task in
 *                  co-operative manner.  Main application must call
 *                  this function repeatdly to ensure all open
 *                  or new connections are served on time.
 ********************************************************************/
void HTTP_Server(BOOL available)
{
    BYTE conn = MAX_HTTP_CONNECTIONS;
	
	do
    {
		HTTPLoadConn(--conn);
		
		if(HTTP.StateMachine == SM_HTTP_UNINITIALIZED)
		{
			HTTPInit();
		}
		
		HTTP_Process(available);
	}
	while(conn);
}


/*********************************************************************
 * Forcibly shut down all connections with open files when the file
 * system is dismounted
 *********************************************************************/
void HTTP_FileSystemReset(BOOL safe)
{
	BYTE index = MAX_HTTP_CONNECTIONS;

	do
	{
		HTTPLoadConn(--index);

		if(HTTP.file)
		{
			TCPFlush(HTTP.socket);
			TCPDisconnect(HTTP.socket);
			HTTP.StateMachine = SM_HTTP_IDLE;

			// Should the file be properly closed or just throw it away?
			if(safe)
				FSfclose(HTTP.file);
			HTTP.file = NULL;
		}
	}
	while(index);
}


/*********************************************************************
 * Serve a single connection
 ********************************************************************/
static void HTTP_Process(BOOL available)
{
    BYTE error;
	const struct HTTP_FileContent rom *content;

retry:
	switch(HTTP.StateMachine)
	{
	// Introduced explicit disconnect state handling in order to properly
	// finish all queries
	case SM_HTTP_IDLE:
		// Connections closed for any reason will automatically end up in
		// the disconnected state, so this is a good place to make sure
		// we don't leak files.
		if(HTTP.file)
		{
			FSfclose(HTTP.file);
			HTTP.file = NULL;
		}

		// Just wait for any connection to be made
		if(!TCPIsConnected(HTTP.socket))
			// Don't break for new connections.  There may be 
 			// an entire request in the buffer already.
			return;
		
		HTTP.StateMachine = SM_HTTP_PARSE_REQUEST;
		
		// Adjust the TCP FIFOs for optimal reception of 
		// the next HTTP request from the browser
		///while(!TCPAdjustFIFOSize(HTTP.socket, 1, 0, TCP_ADJUST_PRESERVE_RX | TCP_ADJUST_GIVE_REST_TO_RX));
		// Fallthrough
		// .
		// .
		// .
	case SM_HTTP_PARSE_REQUEST:
		// Verify the entire first line is in the FIFO and
		if(!HTTP_SetBuf(-1))
			break;

		// Check that the file system is mounted
		error = HTTP_ERR_NOT_AVAILABLE;

		if(available)
		{
			// Parse the request line
			error = HTTP_ERR_NOT_IMPLEMENTED;
			// Parses the first line for a file name and GET args 
			if(HTTP_Parse())
			{				
				static char read[] = FS_READ;
				
				error = HTTP_ERR_NOT_FOUND;
				// Open the file
				if(HTTP.file = FSfopen(HTTP_ParseName, read), HTTP.file)
					// Success!
					goto retry;
			}
		}

		HTTP.FileType = error;
		HTTP.StateMachine = SM_HTTP_ERROR;

		// Fallthrough
		// .
		// .
		// .
	case SM_HTTP_ERROR:
		if(TCPIsPutReady(HTTP.socket) >= strlenpgm((const char far rom *) HTTP_Errors[HTTP.FileType]))
		{
			TCPPutROMString(HTTP.socket, (ROM BYTE *) HTTP_Errors[HTTP.FileType]);
			TCPFlush(HTTP.socket);
			TCPDisconnect(HTTP.socket);
			HTTP.StateMachine = SM_HTTP_IDLE;
		}
		break;

	// Parse out the POST header lines
	case SM_HTTP_POST_HEADER:
		// Is the next line ready yet?
		while(HTTP_SetBuf(-1))
		{
			char ch;

			// Isolate the field name
			HTTP_Buffer.write = HTTP_Data;

			do
			{
				ch = HTTP_GetBuf();

				// The payload starts after the first empty line
				if(ch == '\0')
				{
					HTTP.StateMachine = SM_HTTP_POST_ARGUMENTS;
					goto retry;
				}

				HTTP_PutBuf(ch);
			}
			while(ch != ' ');

			HTTP_PutBuf('\0');

			// The content length is needed to know the length of the variable block
			if(!Cfg_StringCompare(HTTP_Data, (const char ROM *) HTTP_CONTENT_LENGTH_STRING))
			{
				unsigned int len = 0;

				while(ch = HTTP_GetBuf() - '0', (unsigned char) ch <= 9)
				{
					len *= 10;
					len += ch;
				}

				HTTP.length = len;
			}
		}
		break;

	case SM_HTTP_POST_ARGUMENTS:
		if(!HTTP_SetBuf(HTTP.length))
			break;

		if(HTTP_ParseAttribs())
		{
			Microchip_RefreshSettings();
			Microchip_RestoreSPI();
		};

		HTTP.StateMachine = SM_HTTP_PARSE_HEADERS;

		// Fallthrough
		// .
		// .
		// .
	case SM_HTTP_PARSE_HEADERS:
		content = &HTTP_FileContent[HTTP.FileType];
		
		// Write response string
		TCPPutROMString(HTTP.socket, HTTP_OK_STRING);
		TCPPutROMString(HTTP.socket, (ROM BYTE *) content->mime);

		// Generic cache control mechanism
		TCPPutROMString(HTTP.socket, content->cache ? HTTP_NL_DO_CACHE_STRING : HTTP_NL_NO_CACHE_STRING);

		//  Generate the content-length field so that dismount errors can be detected
		HTTP.dynamic = content->dynamic;

		if(!HTTP.dynamic)
		{
			Numeric(Num_Buffer, HTTP.file->size);
			TCPPutROMString(HTTP.socket, HTTP_NL_CONTENT_LENGTH_STRING);
			TCPPutString(HTTP.socket, (BYTE *) Num_Buffer);
		}

		TCPPutROMString(HTTP.socket, HTTP_NL_HEADER_END_STRING);

		HTTP.StateMachine = SM_HTTP_PROCESS_GET;

		// Fallthrough
		// .
		// .
		// .
	case SM_HTTP_PROCESS_GET:
		// Throw away any more data receieved - we aren't going to use it.
		TCPDiscard(HTTP.socket);

		if(HTTP_SendFile())
		{
			TCPFlush(HTTP.socket);
			TCPDisconnect(HTTP.socket);
			HTTP.StateMachine = SM_HTTP_IDLE;
		}
		break;
	}

	// Flush out any remaining line-buffered data
	HTTP_FlushBuf();

	// If, during handling of HTTP socket, it gets disconnected then forget
	// about any previous processing and return to the idle state.
	//
	// Do wait until as much processing as possible has been completed on
	// whatever data remains in the buffers however
	if(!TCPIsConnected(HTTP.socket))
		HTTP.StateMachine = SM_HTTP_IDLE;
}


/*********************************************************************
 * Manage an HTML output buffer for use when dumping variable contents
 * through the configuration writer
 *********************************************************************/
static size_t HTTP_WriteSocket(void)
{
	if(Cfg_BufPtr)
	{
		unsigned char ch;
		char *head, *tail;

		*Cfg_BufPtr = '\0';

		head = HTTP_Data;
		tail = HTTP_Data;

		while(ch = *tail++, ch)
		{
			// Escape any special characters
			if(ch <= '&' || ch == '<' || ch == '>')
			{
				TCPPutArray(HTTP.socket, (BYTE *) head, tail - head);

				Num_Buffer[0] = '&';
				Num_Buffer[1] = '#';
				head = Num_Integer(&Num_Buffer[2], ch);
				*head++ = ';';

				TCPPutString(HTTP.socket, (BYTE *) Num_Buffer);

				head = tail;
			}
		}

		TCPPutString(HTTP.socket, (BYTE *) head);
	}

	Cfg_BufPtr = HTTP_Data;
	return HTTP_MAX_DATA_LEN;
}


/*********************************************************************
 * Transfer files to the client
 ********************************************************************/
static BOOL HTTP_SendFile(void)
{
	ptrdiff_t read;
	size_t write;
	char *buffer;

	// First handle static file data, which is simply pushed through
	// with minimal processing
	if(!HTTP.dynamic)
	{
		write = TCPIsPutReady(HTTP.socket);

		while(write)
		{
			read = FSfreadbuf((void **) &buffer, write, HTTP.file);
			write -= TCPPutArray(HTTP.socket, (BYTE *) buffer, read);

			if(FSerror() != CE_GOOD)
				return true;
		}

		return false;
	}

	// Transfer data from the file to the socket while looking for escape
	// characters and substituting variable values
	while(read = FSfreadbuf((void **) &buffer, TCPIsPutReady(HTTP.socket), HTTP.file), read)
	{
		// Reserved output buffer space for (escaped) variable values
		enum { VARIABLE_SPACE = HTTP_MAX_DATA_LEN * 2 };

		char *escape;

		// Look for the escape characters within the buffer
		while(escape = memchr((const char *) buffer, '%', read), escape)
		{
			char ch;

			// First flush all data up to the escape character
			read -= TCPPutArray(HTTP.socket, (BYTE *) buffer, escape - buffer);

			// Abort if there isn't enough output buffer space available
			if(TCPIsPutReady(HTTP.socket) < VARIABLE_SPACE)
			{
				FSfseek(HTTP.file, -read, SEEK_CUR);
				return false;
			}

			// Eat the escape character itself
			--read;
			buffer = escape + 1;

			// Read the variable name into a buffer
			HTTP_Buffer.write = HTTP_Data;

			do
			{
				if(read)
					--read;
				else if(!FSfreadbuf((void **) &buffer, 1, HTTP.file))
					return true;

				ch = *buffer++;

				if(ch == '%')
					ch = '\0';

				HTTP_PutBuf(ch);
			}
			while(ch);

			// Two escape characters in a row are used to emit the '%'
			// character itself
			if(HTTP_Buffer.write == &HTTP_Data[1])
				TCPPut(HTTP.socket, '%');
			// Write the variable's value with appropriate escape characters
			else
			{
				Cfg_SaveOne(Microchip_Settings, HTTP_Data, HTTP_WriteSocket);
				HTTP_WriteSocket();
			}
		}

		// Write the remainder and check for EOF
		TCPPutArray(HTTP.socket, (BYTE *) buffer, read);

		if(FSerror() != CE_GOOD)
			return true;
	}

	// We're not done yet..
	return false;
}


/*********************************************************************
 * Parse HTTP request line
 ********************************************************************/
static bool HTTP_Parse(void)
{
	char ch;
	char *ext;
	unsigned char index;
	unsigned char HttpRequestLength = 0;

	// Parse command identifier.. POST or GET
	while(ch = HTTP_GetBuf(), (unsigned char) ch > ' ')
		HTTP_PutBuf(ch);
	HTTP_PutBuf('\0');

	if ( memcmppgm2ram(HTTP_Data, (ROM void*)"GET", 3) == 0)
		HTTP.StateMachine = SM_HTTP_PARSE_HEADERS; // HTTP_GET
	else if ( memcmppgm2ram(HTTP_Data, (ROM void*)"POST", 4) == 0)
		HTTP.StateMachine = SM_HTTP_POST_HEADER; // HTTP_POST
	else
		{// Unrecognized method, so return not implemented
		 // Only "GET" and "POST" are supported for time being.
			HTTP.Status = HTTP_NOT_IMPLEMENTED;
			//HTTP.StateMachine = SM_HTTP_SERVE_HEADERS;
			return false;
		}

	HTTP_Buffer.write = HTTP_Data;

	// Skip white space
	do
		ch = HTTP_GetBuf();
	while(ch == ' ');

	// Parse file name
 	if(ch == '/' || ch == '\\')
	{
		do
		{
			ch = HTTP_GetBuf();

			switch(ch)
			{
			case '?':
			case '&':
				HTTP_PutBuf('\0');
				HTTP_ParseAttribs();
				goto break_out;

			case ' ':
				ch = '\0';
			}

			HTTP_PutBuf(ch); // takes eof and dot
		
		HttpRequestLength++;
		}
		while(ch != '\0');

break_out:;
	}

	// Use default file name?
	if(HTTP_Buffer.write <= &HTTP_Data[1])
		strcpypgm2ram((void*)&HTTP_Data, Microchip_DefaultPage);

	// Check the file extension
	index = HTTP_UNKNOWN;

	if(ext = strrchr(HTTP_Data, '.'), ext)
	{
		++ext;
		index = 0;
		while(memcmppgm2ram(ext, (ROM void*)HTTP_FileContent[index].ext, 3))
		//while(Cfg_StringCompare(ext, HTTP_FileContent[index].ext))
		{
			if(++index == HTTP_UNKNOWN)
				break;
		}
	}
	
	HTTP.FileType = index;
	
	//Check if the file name length requested by the client is more then 8 chars  
/*	if(HttpRequestLength>13)//13 bytes = 8(FileName)+ 3 + 1dot +1eof (Max file extension lengtrh) 
	{
		BYTE j, cntr=0;
		BYTE *dummyPtr=NULL;
		BYTE *ptr = NULL;
		BYTE *tempPtr=NULL;
		BYTE dummyCntr;
		
		//Modify the file name to be compatible to MDD format
		for(j=0;j<6;j++) 
		{
			//copy first 6 chars of the file name
			*(dummyPtr++)=HTTP_Data[++dummyCntr];
		}
		
		//Serach by first six chars of the file name
		*(dummyPtr++)= '~';
		*(dummyPtr++)= '1';
		
		//Parse till file extension starts
		do
		{
			;	
		}
		while(HTTP_Data[++dummyCntr] != 0x2e);
		
		//Add the extension to the modifed fle name
		*(dummyPtr++)=0x2e;
		*(dummyPtr++)=HTTP_Data[++dummyCntr];
		*(dummyPtr++)=HTTP_Data[++dummyCntr];
		*(dummyPtr++)=HTTP_Data[++dummyCntr];
		*(dummyPtr++)='\0';
		memcmppgm2ram(HTTP_Data, (ROM void*)dummyPtr, 8);
	}*/
	return true;
}


/*********************************************************************
 * Parse out the name/values attribute pairs from the input line into
 * the HTTP configuration variable set
 *********************************************************************/
static char *HTTP_AttribValue;

static size_t HTTP_ParseAttribValue(void)
{
	Cfg_BufPtr = HTTP_AttribValue;
	return HTTP_Buffer.write - Cfg_BufPtr;
}

static bool HTTP_ParseAttribs(void)
{
	bool changed = false;
	char *const name = HTTP_Buffer.write;

	goto enter;

	for(;;)
	{
		char ch = HTTP_GetBuf();

		switch(ch)
		{
		case ' ':
			ch = '\0';
		case '\0':
		case '?':
		case '&':
			if(HTTP_AttribValue)
			{
				HTTP_PutBuf('\0');
				changed |= Cfg_ParseOne(Microchip_Settings, name, HTTP_ParseAttribValue);
			}

			if(!ch)
				return changed;

			HTTP_Buffer.write = name;
enter:
			HTTP_AttribValue = NULL;
			continue;

		case '=':
			HTTP_AttribValue = HTTP_Buffer.write + 1;
			ch = '\0';
			break;

		case '+':
			ch = ' ';
			break;

		// Escape character
		case '%':
			ch = 0;

			for(;;)
			{
				unsigned char digit = HTTP_GetBuf();

				if(digit >= '0' && digit <= '9')
					digit -= '0';
				else if(digit >= 'A' && digit <= 'F')
					digit -= 'A' - 10;
				else if(digit >= 'a' && digit <= 'f')
					digit -= 'a' - 10;
				else
					break;

				ch <<= 4;
				ch += digit;
			}

			HTTP_UngetBuf();
		}

		HTTP_PutBuf(ch);
	}
}

#endif //#if defined(STACK_USE_HTTP_SERVER)
