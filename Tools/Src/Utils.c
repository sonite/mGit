#include <stdio.h>
#include "Utils.h"


/******************************************************************************
 * Enforce null-termination for MSVCRT's (v)snprintf functions
 ******************************************************************************/
#ifdef _MSC_VER
int safe_vsnprintf(char *buf, size_t len, const char *format, va_list args) {
	int written = _vsnprintf(buf, len, format, args);

	if((unsigned) written >= len) {
		written = len;
		if(len--)
			buf[len] = '\0';
	}

	return written;
}

int DECL_VARARGS safe_snprintf(char *buf, size_t len, const char *format, ...) {
	va_list args;
	int written;

	va_start(args, format);
	written = safe_vsnprintf(buf, len, format, args);
	va_end(args);

	return written;
}
#endif


/******************************************************************************
 * Error handling
 ******************************************************************************/
char err_message[256];

static err_frame_t *volatile _err_top;

static void _err_vformat(const char *format, va_list args) {
	char *ptr = err_message;
	err_frame_t *frame = _err_top;

	if(frame)
		ptr = frame->context(frame->userdata, ptr, endof(err_message));

	safe_vsnprintf(ptr, endof(err_message) - ptr, format, args);
}

void DECL_VARARGS err_format(const char *format, ...) {
	va_list args;

	va_start(args, format);
	_err_vformat(format, args);
	va_end(args);
}

void DECL_NO_RETURN DECL_VARARGS err_throw(const char *format, ...) {
	va_list args;

	va_start(args, format);
	_err_vformat(format, args);
	va_end(args);

	longjmp(err_unwind()->unwind, 1);
}

err_frame_t *err_unwind(void) {
	err_frame_t *frame = _err_top;
	_err_top = frame->parent;
	return frame;
}

err_frame_t *_err_try1(err_frame_t *frame) {
	return _err_try2(frame, NULL, NULL);
}

err_frame_t *_err_try2(err_frame_t *frame, err_context_t context,
	void *userdata) {
	frame->context = context;
	frame->userdata = userdata;
	frame->parent = _err_top;
	_err_top = frame;
	return frame;
}


/******************************************************************************
 * Read a whole file into a null-terminated buffer
 ******************************************************************************/
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

char *file_open(const char *name) {
	HANDLE handle = CreateFileA(name, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
		OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if(handle != INVALID_HANDLE_VALUE) {
		DWORD size = GetFileSize(handle, NULL);

		if(size != INVALID_FILE_SIZE) {
			// Allocate an extra byte to make sure there's at least one null at
			// the end of the file
			char *buffer = VirtualAlloc(NULL, ++size,
				MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			if(buffer) {
				DWORD read;

				if(ReadFile(handle, buffer, size, &read, NULL) && read < size) {
					CloseHandle(handle);
					return buffer;
				}

				VirtualFree(buffer, 0, MEM_RELEASE);
			}
		}

		CloseHandle(handle);
	}

	err_format("Cannot open file '%s'", name);
	return NULL;
}

void file_close(char *buffer) {
	VirtualFree(buffer, 0, MEM_RELEASE);
}


/******************************************************************************
 * Extract the relative input path and the file name without extension
 ******************************************************************************/
const char *PathOf(char *path, char *name, size_t space)
{
	char ch;

	char *file = path;
	char *ext = NULL;

	while(ch = *path++, ch) {
		if(ch == '.')
			ext = path;
		else if(ch == '/' || ch =='\\' || ch == ':') {
			file = path;
			ext = NULL;
		}
	}

	if(ext)
		ext[-1] = '\0';

	safe_snprintf(name, space, "%s", file);
	*file = '\0';
	return ext;
}