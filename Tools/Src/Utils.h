#ifndef _UTILS_H
#define _UTILS_H
#include <stdarg.h>
#include <setjmp.h>


/******************************************************************************
 * The usual portability boilerplate
 ******************************************************************************/
#ifdef _MSC_VER
#	include <malloc.h>
#	define alloca _alloca
#	pragma warning(disable: 4244)
#	pragma warning(disable: 4761)

typedef signed   __int8   int8_t;
typedef unsigned __int8  uint8_t;
typedef signed   __int16  int16_t;
typedef unsigned __int16 uint16_t;
typedef signed   __int32  int32_t;
typedef unsigned __int32 uint32_t;

typedef enum { false, true } bool;
#else
#	ifdef __GNUC__
#		include <alloca.h>
#		define alloca _alloca
#	endif
#	include <stdint.h>
#	include <stdbool.h>
#endif

#if defined(_MSC_VER)
#	define DECL_VARARGS __cdecl
#	define DECL_NO_RETURN __declspec(noreturn)
#	define DECL_NO_DEFAULT { default: __assume(0); }
#	define DECL_INLINE static __forceinline
#elif defined(__GNUC__)
#	define DECL_VARARGS
#	define DECL_NO_RETURN __attribute__((noreturn))
#	define DECL_NO_DEFAULT
#	define DECL_INLINE static __attribute__((always_inline))
#else
#	define DECL_VARARGS
#	define DECL_NO_RETURN
#	define DECL_NO_DEFAULT
#	define DECL_INLINE static
#endif

// Enforce null-termination for MSVCRT's (v)snprintf functions
#ifdef _MSC_VER
int safe_vsnprintf(char *buf, size_t len, const char *format, va_list args);
int DECL_VARARGS safe_snprintf(char *buf, size_t len, const char *format, ...);
#else
#	define saef_vsnprintf vsnprintf
#	define safe_snprintf snprintf
#endif


/******************************************************************************
 * Various utility and book-keeping functions
 ******************************************************************************/
#define countof(p) (sizeof (p) / sizeof *(p))
#define endof(p) ((p) + countof(p))

typedef char *(*err_context_t)(void *userdata, char *ptr, char *end);

typedef struct err_frame_t {
	jmp_buf unwind;
	struct err_frame_t *parent;
	err_context_t context;
	void *userdata;
} err_frame_t;

#define err_try(stack, context, userdata) \
(!setjmp(_err_try1((stack)))
#define err_try_in(stack, context, userdata) \
(!setjmp(_err_try2((stack), (context), (userdata))->unwind))

void DECL_NO_RETURN DECL_VARARGS err_throw(const char *format, ...);
void DECL_VARARGS err_format(const char *format, ...);
err_frame_t *err_unwind(void);
err_frame_t *_err_try1(err_frame_t *frame);
err_frame_t *_err_try2(err_frame_t *frame, err_context_t context, void *userdata);

extern char err_message[];

char *file_open(const char *name);
void file_close(char *buffer);

const char *PathOf(char *path, char *name, size_t space);

#endif