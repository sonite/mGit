#include "Log.h"

// The maximum number of years to keep before rotating the logs.
// Note that the FAT-16 root directory can hold at most 512 files,
// or less with long file names
#define LOG_YEARS        3
// Log filename
#define LOG_NAME_PREFIX  "LOG-"
#define LOG_NAME_SUFFIX  ".TXT"
// Write high-frequency log record every N seconds
#define LOG_SHORT_PERIOD 6
// Separator character
#if NUM_DECIMAL_POINT == ','
#	define LOG_SEPARATOR ';'
#else
#	define LOG_SEPARATOR ','
#endif

// Rewind and write summary every N seconds
uint16_t Log_LongPeriod = 600;

static long Log_Sum[LOG_NUM_FIELDS];
static long Log_ShortTimeout;
static long Log_LongTimeout;
static unsigned int Log_Average;
static unsigned long Log_RewindTo;
static unsigned long Log_ExpectedSize /* Initially empty */;
static unsigned int Log_ExpectedTime;
static unsigned int Log_ExpectedDate;

// A table of month names
static const char rom Log_MonthNames[1 + 12][4] =
{
	"UNK", // Catchall for unset RTC dates
	"JAN",
	"FEB",
	"MAR",
	"APR",
	"MAY",
	"JUN",
	"JUL",
	"AUG",
	"SEP",
	"OCT",
	"NOV",
	"DEC"
};

static void Log_Record(FSFILE *stream, unsigned int average);
static char *Log_AppendString(char *dst, const char rom *src);

// Periodically write log values
void Log_Run(void)
{
	long tick;
	FSFILE *stream;

	memset(&Num_Buffer[0], 0, 48); // FINSUN - Test for clearing
	// Check whether it is time to write another log record
	tick = TickGet();

	if(tick - Log_ShortTimeout >= 0)
	{
		Log_ShortTimeout = tick + TICKS_PER_SECOND * LOG_SHORT_PERIOD;

		// Get an updated clock value while we still can
		RTC_Read_Clock();

		// Generate the file name for this month and try to open it
		Log_AppendString
		(
			Num_Integer
			(
				Log_AppendString
				(
					Log_AppendString
					(
						Num_Buffer,
						LOG_NAME_PREFIX
					),
					Log_MonthNames[RTCclk.month]
				),
				(RTCclk.year % LOG_YEARS) + 1
			),
			LOG_NAME_SUFFIX
		);

		DisableWDT();

		if(stream = Microchip_FileOpen(Num_Buffer, MICROCHIP_APPEND), stream)
		{
			// Flush the file at the start of a new month, that is if the file
			// modification month is older than the current date
			if(FSCurrentDate() > (stream->date | FS_DATE_DY_MASK))
				Log_RewindTo = 0;
			// Otherwise append to the end of the file unless it has been modified in
			// the background, or been explicitly invalidated
			else if(Log_ExpectedSize != stream->size ||
				Log_ExpectedTime != stream->time || Log_ExpectedDate != stream->date)
				Log_RewindTo = FSfpos(stream);

			Log_Record(stream, 0);
			++Log_Average;

			// Periodically combine the last N records into a common average
			Log_ExpectedSize = stream->size;

			if(tick - Log_LongTimeout >= 0)
			{
				Log_LongTimeout = tick + TICKS_PER_SECOND * Log_LongPeriod;

				FSfseek(stream, Log_RewindTo, SEEK_SET);
				FSftruncate(stream);

				Log_Record(stream, Log_Average);
				Log_Average = 0;
				Log_ExpectedSize = 0;
			}

			Microchip_FileClose(stream);

			// The the file date to compare to needs to be read carefully after closing
			// the file since FSfclose will query the real-time clock
			Log_ExpectedTime = FSCurrentTime();
			Log_ExpectedDate = FSCurrentDate();
		}

		ClearWDT();
		EnableWDT();
	}
}

// Combine strings in ROM into a buffer
static char *Log_AppendString(char *dst, const char rom *src)
{
	while(*dst = *src++)
		++dst;
	return dst;
}

// Combine clock fields into a time record of the format:
// "20YY-MM-DD HH:MM:SS"
char *Log_Timestamp(char *buffer)
{
	unsigned char ch;

	static const rom struct
	{
		char separator;
		unsigned char *value;
	}
	clock[] =
	{
		{ '0', &RTCclk.year   },
		{ '-', &RTCclk.month  },
		{ '-', &RTCclk.mday   },
		{ ' ', &RTCclk.hour   },
		{ ':', &RTCclk.minute },
		{ ':', &RTCclk.second }
	},
	*field;

	field = clock;

	*buffer++ = '2';
	ch = '0';

	do
	{
		Num.Flags |= NUM_ZERO_EXPANSION;
		Num.Alignment = 2;

		*buffer++ = field->separator;
		ch = *field->value;

		if(ch >= 100)
			ch = 0;

		buffer = Numeric(buffer, ch);
	}
	while(++field != &clock[sizeof clock / sizeof *clock]);

	return buffer;
}

// Form CSV record
static void Log_Record(FSFILE *stream, unsigned int average)
{
	char *buffer;
	unsigned char index;
	long *sum;

	// Begin with the time
	buffer = Log_Timestamp(Num_Buffer);

	// Write records
	index = 0;
	sum = Log_Sum;

	do
	{
		long value;

		FSfwrite((const void *) Num_Buffer, buffer - Num_Buffer, 1, stream);
		memset(&Num_Buffer[0], 0, 48); // FINSUN - Test for clearing
		value = Log_Value(index);

		// Dsiable decimal grouping to avoid the separator clashing with anything
		Num.Flags &= ~NUM_GROUPING;

		if(!average)
			*sum += value;
		else
		{
			value = *sum / average;
			*sum = 0;
		}

		++sum;

		Num_Buffer[0] = LOG_SEPARATOR;
		buffer = Numeric(&Num_Buffer[1], value);
	}
	while(++index, index != LOG_NUM_FIELDS);

	*buffer++ = '\r';
	*buffer++ = '\n';

	FSfwrite((const void *) Num_Buffer, buffer - Num_Buffer, 1, stream);
	memset(&Num_Buffer[0], 0, 48); // FINSUN - Test for clearing
}