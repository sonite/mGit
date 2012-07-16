#include <setjmp.h>
#include "Microchip/Microchip.h"
#include "Numeric.h"
#include "Flash.h"

#define CFG_NETWORK_WRITER 0

// FInsun: Why.... we have space and it seem to sometime write HTTp data to files
// Reuse the HTTP buffer if networking is enabled
//#if MICROCHIP_USE_IP
//#	define Cfg_StreamBuffer	HTTP_Data
//#	define Cfg_StreamBufferSize HTTP_DATA_SIZE
//#else
static char Cfg_StreamBuffer[64];
#	define Cfg_StreamBufferSize (sizeof Cfg_StreamBuffer)
//#endif

#define Cfg_Error() longjmp(Cfg_Unwind, 0)
static jmp_buf Cfg_Unwind;
static Cfg_Stream_t *Cfg_Stream;

signed char Cfg_StringCompare(const char *p1, const char rom *p2)
{
	unsigned char ch1, ch2;

	do
	{
		ch1 = *p1++;

		if(ch1 -= 'a', ch1 <= 'z' - 'a')
			ch1 += 'A' - 'a';

		ch2 = *p2++;

		if(ch2 -= 'a', ch2 <= 'z' - 'a')
			ch2 += 'A' - 'a';
	}
	while(!(ch1 -= ch2) && ch2 != (unsigned char) -'a');

	return ch1;
}

static const Cfg_Variable rom *Cfg_LookupVar(const Cfg_Variable rom *set, const char *key)
{
	const char rom *name;

	goto enter;

	do
	{
		++set;
enter:
		if(name = set->name, !name)
			Cfg_Error();
	}
	while(Cfg_StringCompare(key, name));

	return set;
}


/******************************************************************************
 * Configuration parser
 ******************************************************************************/
#define Cfg_UngetChar() (--Cfg_BufPtr)

enum
{
	CFG_TOK_ID = 'A',
	CFG_TOK_NUM = '0'
};

static char Cfg_GetChar(void);
static char Cfg_GetToken(void);
static void Cfg_ExpectToken(char tok);

char *Cfg_BufPtr;
char *Cfg_BufEnd;
static unsigned int Cfg_LastLine;
static unsigned int Cfg_CurrentLine;
static char Cfg_EndOfFile;

static union
{
	unsigned long num;
	char name[32];
}
Cfg_Token;

static char Cfg_GetChar(void)
{
	if(Cfg_BufPtr == Cfg_BufEnd)
	{
		size_t len = Cfg_Stream();

		if(!len)
		{
			// Force a newline at the end of the file
			Cfg_EndOfFile = 1;
			return '\n';
		}

		Cfg_BufEnd = Cfg_BufPtr + len;
	}

	return *Cfg_BufPtr++;
}

static char Cfg_GetToken(void)
{
	char ch;

retry:
	Cfg_LastLine = Cfg_CurrentLine;
	ch = Cfg_GetChar();

	switch(ch)
	{
	// Whitespace
	case ' ': case '\t': case '\v': case '\r':
		goto retry;

	// Comment
	case '#':
		do
			ch = Cfg_GetChar();
		while(ch != '\n');
	// Newline or end-of-file
	case '\n':
		++Cfg_CurrentLine;
		break;

	// Decimal number
	case '0': case '1': case '2': case '3':
	case '4': case '5': case '6': case '7':
	case '8': case '9':
		{
			unsigned long num = 0;
			do
			{
				num *= 10;
				num += ch - '0';
				ch = Cfg_GetChar();
			} while(ch >= '0' && ch <= '9');
			Cfg_UngetChar();
			Cfg_Token.num = num;
			ch = CFG_TOK_NUM;
		}
		break;

	// Hexadecimal number
	case '$':
		{
			unsigned long num = 0;
			for(;;)
			{
				ch = Cfg_GetChar();

				if(ch >= '0' && ch <= '9')
					ch -= '0';
				else if(ch >= 'A' && ch <= 'F')
					ch += 10 - 'A';
				else if(ch >= 'a' && ch <= 'f')
					ch += 10 - 'a';
				else
					break;

				num <<= 4;
				num += ch;
			}
			Cfg_UngetChar();
			Cfg_Token.num = num;
			ch = CFG_TOK_NUM;
		}
		break;

	// Identifier
	case 'A': case 'B': case 'C': case 'D':
	case 'E': case 'F': case 'G': case 'H':
	case 'I': case 'J': case 'K': case 'L':
	case 'M': case 'N': case 'O': case 'P':
	case 'Q': case 'R': case 'S': case 'T':
	case 'U': case 'V': case 'W': case 'X':
	case 'Y': case 'Z':
	case 'a': case 'b': case 'c': case 'd':
	case 'e': case 'f': case 'g': case 'h':
	case 'i': case 'j': case 'k': case 'l':
	case 'm': case 'n': case 'o': case 'p':
	case 'q': case 'r': case 's': case 't':
	case 'u': case 'v': case 'w': case 'x':
	case 'y': case 'z':
	case '_':
		{
			char *ptr = Cfg_Token.name;
			do
			{
				if
				(
					(ch < 'A' || ch > 'Z') &&
					(ch < 'a' || ch > 'z') &&
					(ch < '0' || ch > '9') &&
					ch != '_'
				)
				{
					Cfg_UngetChar();
					break;
				}

				*ptr++ = ch;
				ch = Cfg_GetChar();
			}
			while(ptr != &Cfg_Token.name[sizeof(Cfg_Token.name) - 1]);
			*ptr = '\0';
			ch = CFG_TOK_ID;
		}
		break;
	}

	return ch;
}

static void Cfg_ExpectToken(char tok)
{
	if(Cfg_GetToken() != tok)
		Cfg_Error();
}

int Cfg_ParseAll(const rom Cfg_Variable *set, Cfg_Stream_t *stream)
{
	volatile int error = 0;

	Cfg_Stream = stream;
	Cfg_BufPtr = NULL;
	Cfg_BufEnd = NULL;
	Cfg_CurrentLine = 1;
	Cfg_EndOfFile = 0;

	// On syntax error eat the rest of the line and try to recover
	if(setjmp(Cfg_Unwind))
	{
		if(!error)
			error = Cfg_LastLine;
		while(Cfg_GetToken() != '\n');
	}

	// Walk the file line-by-line
	while(!Cfg_EndOfFile)
	{
		const rom Cfg_Variable *var;

		switch(Cfg_GetToken())
		{
		default:
			Cfg_Error();

		case '\n':
			continue;

		case CFG_TOK_ID:
			;
		}

		var = Cfg_LookupVar(set, Cfg_Token.name);

		Cfg_ExpectToken('=');
		var->type(var, true);
		Cfg_ExpectToken('\n');
	}

	return error;
}

// Parse the value of a single variable from the chosen buffer
bool Cfg_ParseOne(const rom Cfg_Variable *set, const char *name, Cfg_Stream_t *stream)
{
	Cfg_Stream = stream;
	Cfg_BufPtr = NULL;
	Cfg_BufEnd = NULL;

	if(setjmp(Cfg_Unwind))
		return false;

	set = Cfg_LookupVar(set, name);
	set->type(set, true);
	return true;
}


/******************************************************************************
 * Configuration writer
 ******************************************************************************/
static void Cfg_PutChar(const char ch);
static void Cfg_PutBuffer(const char *string);
static void Cfg_PutString(const char rom *string);
static void Cfg_PutInteger(unsigned long value);

static void Cfg_PutChar(const char ch)
{
	if(Cfg_BufPtr == Cfg_BufEnd)
	{
		size_t len = Cfg_Stream();
		
		if(!len)
			Cfg_Error();

		Cfg_BufEnd = &Cfg_BufPtr[len];
	}

	*Cfg_BufPtr++ = ch;
}

static void Cfg_PutBuffer(const char *string)
{
	char ch;
	
	while(ch = *string++, ch)
		Cfg_PutChar(ch);
}

static void Cfg_PutString(const char rom *string)
{
	char ch;
	
	while(ch = *string++, ch)
		Cfg_PutChar(ch);
}

static void Cfg_PutInteger(unsigned long value)
{
	Numeric(Num_Buffer, value);
	Cfg_PutBuffer(Num_Buffer);
}

#if CFG_NETWORK_WRITER
static void Cfg_PutHex(unsigned char value)
{
	Num_Buffer[0] = '$';
	Num.Radix = 0x10;
	Numeric(&Num_Buffer[1], value);
	Cfg_PutBuffer(Num_Buffer);
}
#endif

bool Cfg_SaveAll(const rom Cfg_Variable *set, Cfg_Stream_t *stream)
{
	const char rom *id;

	if(setjmp(Cfg_Unwind))
		return false;

	// Prepare the output buffer
	Cfg_Stream = stream;
	Cfg_BufPtr = NULL;
	Cfg_BufEnd = NULL;

	// Write out each variable
	Cfg_PutString("# Generated file\r\n");

	while(id = set->name, id)
	{
		Cfg_PutString(id);
		Cfg_PutString(" = ");
		set->type(set, false);
		Cfg_PutString("\r\n");
		++set;
	}

	// Flush any buffered output
	Cfg_Stream();
	return true;
}

// Write the value of a single variable to the chosen buffer
bool Cfg_SaveOne(const rom Cfg_Variable *set, const char *name, Cfg_Stream_t *stream)
{
	Cfg_Stream = stream;
	Cfg_BufPtr = NULL;
	Cfg_BufEnd = NULL;

	if(setjmp(Cfg_Unwind))
		return false;

	set = Cfg_LookupVar(set, name);
	set->type(set, false);
	return true;
}


/******************************************************************************
 * Deal with configurations in the filesystem
 ******************************************************************************/
static FSFILE *Cfg_File;

static size_t Cfg_ReadFile(void)
{
	//FINSUN: Have seen write errors in files, try the original function instead (new update)
	/*return FSfread((void *) &Cfg_BufPtr, Cfg_BufPtr -  Cfg_StreamBuffer, -1, Cfg_File);*/ return FSfreadbuf((void **) &Cfg_BufPtr, -1, Cfg_File);
}

// return FSfread((void *) Cfg_BufPtr, Cfg_StreamBuffer - Cfg_BufPtr, -1, Cfg_File); -> No Startup (black screen)
// return FSfread((void *) Cfg_BufPtr, Cfg_BufPtr - Cfg_StreamBuffer, -1, Cfg_File); -> File Error no SD start
// return FSfread((void *) &Cfg_BufPtr, Cfg_BufPtr -  Cfg_StreamBuffer, -1, Cfg_File); -> Not reading correct maner? wont startup form file values
// return FSfread((void *) &Cfg_StreamBuffer, Cfg_BufPtr -  Cfg_StreamBuffer, -1, Cfg_File); -> Not reading correct maner? wont startup form file values
static size_t Cfg_WriteFile(void)
{
	if(Cfg_BufPtr)
	{
		if(FSfwrite((const void *) Cfg_StreamBuffer,
			Cfg_BufPtr - Cfg_StreamBuffer, 1, Cfg_File) != 1)
		{
			return 0;
		}
	}

	Cfg_BufPtr = Cfg_StreamBuffer;
	return Cfg_StreamBufferSize;
}

int Cfg_ParseFile(const rom Cfg_Variable *set, const char rom *name)
{
	int error;

	// Open the file and flush the file buffer
	strcpypgm2ram(Num_Buffer, name);

	if(Cfg_File = Microchip_FileOpen(Num_Buffer, MICROCHIP_READ), !Cfg_File)
		return -1;

	error = Cfg_ParseAll(set, Cfg_ReadFile);
	Microchip_FileClose(Cfg_File);
	return error;
}

bool Cfg_SaveFile(const rom Cfg_Variable *set, const char rom *name)
{
	bool success = false;

	// Try to create the file
	strcpypgm2ram(Num_Buffer, name);

	if(Cfg_File = Microchip_FileOpen(Num_Buffer, MICROCHIP_WRITE))
	{
		// Abort on an I/O error and try to close the file
		success = Cfg_SaveAll(set, Cfg_WriteFile);
		Microchip_FileClose(Cfg_File);
	}

	return success;
}


/******************************************************************************
 * Deal with configurations in a special FLASH buffer
 ******************************************************************************/
// Synchronized w. ConfigBackup in the linker script
#define Cfg_FlashStorageHead ((char rom *) 0x001400)
#define Cfg_FlashStorageTail ((char rom *) 0x0017FF)

static char rom *Cfg_FlashPtr;

static size_t Cfg_ReadFlash(void)
{
	size_t space;
	char *tail;

	// Copy the next chunk, up to the size of the buffer
	space = Cfg_FlashStorageTail - Cfg_FlashPtr;

	if(space > Cfg_StreamBufferSize)
		space = Cfg_StreamBufferSize;

	Cfg_BufPtr = memcpypgm2ram(Cfg_StreamBuffer, (const void far rom *) Cfg_FlashPtr, space);

	// See if we've reached the end of the programmed FLASH by searching
	// for unprogrammed bytes
	for(tail = &Cfg_StreamBuffer[space]; space; --space)
	{
		if(*--tail != (char) FLASH_UNPROGRAMMED)
			break;
	}

	Cfg_FlashPtr += space;
	return space;
}

static size_t Cfg_WriteFlash(void)
{
	typedef char check[Cfg_StreamBufferSize >= FLASH_WRITE_BLOCK ? +1 : -1];

	if(Cfg_BufPtr)
	{
		// Clear out the remainder in the final write
		memset
		(
			Cfg_BufPtr,
			FLASH_UNPROGRAMMED,
			&Cfg_StreamBuffer[FLASH_WRITE_BLOCK] - Cfg_BufPtr
		);

		Cfg_FlashPtr = (char rom *) Flash_Write
		(
			Cfg_FlashPtr,
			(const void *) Cfg_StreamBuffer
		);
	}

	Cfg_BufPtr = Cfg_StreamBuffer;
	return Cfg_FlashPtr != Cfg_FlashStorageTail ? FLASH_WRITE_BLOCK : 0;
}

static size_t Cfg_CompareFlash(void)
{
	size_t space;

	if(Cfg_BufPtr)
	{
		if
		(
			memcmppgm2ram
			(
				Cfg_StreamBuffer,
				(void far rom *) Cfg_FlashPtr,
				Cfg_BufPtr - Cfg_FlashStorageHead
			)
		)
		{
			return 0;
		}
	}

	Cfg_BufPtr = Cfg_StreamBuffer;

	space = Cfg_FlashStorageTail - Cfg_FlashPtr;
	if(space > Cfg_StreamBufferSize)
		space = Cfg_StreamBufferSize;
	return space;
}

int Cfg_ParseFlash(const rom Cfg_Variable *set)
{
	Cfg_FlashPtr = Cfg_FlashStorageHead;
	return Cfg_ParseAll(set, Cfg_ReadFlash);
}

bool Cfg_SaveFlash(const rom Cfg_Variable *set)
{
	// Avoid rewriting if nothing has changed
	Cfg_FlashPtr = Cfg_FlashStorageHead;
	if(Cfg_SaveAll(set, Cfg_CompareFlash))
		return true;

	Flash_Erase(Cfg_FlashStorageHead);
	Cfg_FlashPtr = Cfg_FlashStorageHead;
	return Cfg_SaveAll(set, Cfg_WriteFlash);
}


/******************************************************************************
 * Variable (de-)serializers
 ******************************************************************************/
void Cfg_FormatString(const rom Cfg_Variable *var, bool parse);

void Cfg_FormatInt(const rom Cfg_Variable *var, bool parse)
{
	int8_t *value = var->value;
	unsigned char length = var->length;

	if(parse)
	{
		switch(Cfg_GetToken())
		{
		case '-':
			Cfg_ExpectToken(CFG_TOK_NUM);
			Cfg_Token.num = -(signed long) Cfg_Token.num;
			break;

		case CFG_TOK_NUM:
			break;

		default:
			Cfg_Error();
		}

		memcpy(value, (const void *) &Cfg_Token.num, length);
	}
	else
	{
		signed long num = 0;
		if(value[length - 1] < 0)
			num = -1;
		memcpy(&num, var->value, length);
		
		if(num < 0)
		{
			num = -num;
			Cfg_PutChar('-');
		}

		Cfg_PutInteger(num);
	}
}

void Cfg_FormatUInt(const rom Cfg_Variable *var, bool parse)
{
	uint8_t *value = var->value;
	unsigned char length = var->length;

	if(parse)
	{
		Cfg_ExpectToken(CFG_TOK_NUM);
		memcpy(value, (const void *) &Cfg_Token.num, length);
	}
	else
	{
		Cfg_Token.num = 0;
		memcpy(&Cfg_Token.num, (const void *) value, length);
		Cfg_PutInteger(Cfg_Token.num);
	}
}

void Cfg_FormatIP(const rom Cfg_Variable *var, bool parse)
{
	uint8_t *value = ((IP_ADDR *) var->value)->v;
	unsigned char i = 4;

	if(parse)
	{
		goto skip_separator1;

		do
		{
			Cfg_ExpectToken('.');
skip_separator1:
			Cfg_ExpectToken(CFG_TOK_NUM);
			*value++ = Cfg_Token.num;
		}
		while(--i);
	}
#	if CFG_NETWORK_WRITER
	else
	{
		goto skip_separator2;

		do
		{
			Cfg_PutChar('.');
skip_separator2:
			Cfg_PutInteger(*value++);
		}
		while(--i);
	}
#	endif
}

void Cfg_FormatMAC(const rom Cfg_Variable *var, bool parse)
{
	uint8_t *value = ((MAC_ADDR *) var->value)->v;
	unsigned char i = 6;

	if(parse)
	{
		goto skip_separator1;
	
		do
		{
			Cfg_ExpectToken('-');
skip_separator1:
			Cfg_ExpectToken(CFG_TOK_NUM);
			*value++ = Cfg_Token.num;
		}
		while(--i);
	}
#	if CFG_NETWORK_WRITER
	else
	{
		goto skip_separator2;
		
		do
		{
			Cfg_PutChar('.');
skip_separator2:
			Cfg_PutHex(*value++);
		}
		while(--i);
	}
#	endif
}