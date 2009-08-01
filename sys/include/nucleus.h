#ifndef __NUCLEUS_H
#define __NUCLEUS_H

#include <config.h>
#include <compiler.h>
#include <types.h>
#include <errno.h>

extern volatile u64 ticks;

extern struct datetime ipltime;

/* The beginning of it all... */
extern void start(u64 __memsize);

/* borrowed from Linux */
#define container_of(ptr, type, member) ({                      \
         const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
         (type *)( (char *)__mptr - offsetof(type,member) );})

/* borrowed from Linux */
#define offsetof(type, member) __builtin_offsetof(type,member)

static inline void lpswe(void *psw)
{
	asm volatile(
		"	lpswe	0(%0)\n"
	: /* output */
	: /* input */
	  "a" (psw)
	: /* clobbered */
	  "cc"
	);
}

#define BUG()		do { \
				asm volatile(".byte 0x00,0x00" : : : "memory"); \
			} while(0)
#define BUG_ON(cond)	do { \
				if (unlikely(cond)) \
					BUG(); \
			} while(0)

/*
 * This should be as simple as a cast, but unfortunately, the BUG_ON check
 * is there to make sure we never submit a truncated address to the channels
 *
 * In the future, the io code should check if IDA is necessary, and in that
 * case allocate an IDAL & set the IDA ccw flag. Other parts of the system
 * that require 31-bit address should do whatever their equivalent action
 * is.
 */
static inline u32 ADDR31(void *ptr)
{
	u64 ip = (u64) ptr;

	BUG_ON(ip & ~0x7fffffffull);

	return (u32) ip;
}

/*
 * string.h equivalents
 */
#define memset(s,c,n)	__builtin_memset((s),(c),(n))
#define memcpy(d,s,l)	__builtin_memcpy((d),(s),(l))
extern size_t strnlen(const char *s, size_t count);
extern int strcmp(const char *cs, const char *ct);
extern int strncmp(const char *cs, const char *ct, int len);
extern int strcasecmp(const char *s1, const char *s2);
extern char *strncpy(char *dest, const char *src, size_t count);

static inline unsigned char toupper(unsigned char c)
{
	/*
	 * TODO: This would break if we ever tried to compile within an EBCDIC
	 * environment
	 */
	if ((c >= 'a') && (c <= 'z'))
		c += 'A'-'a';
	return c;
}

static inline unsigned char tolower(unsigned char c)
{
	/*
	 * TODO: This would break if we ever tried to compile within an EBCDIC
	 * environment
	 */
	if ((c >= 'A') && (c <= 'Z'))
		c -= 'A'-'a';
	return c;
}

/*
 * stdio.h equivalents
 */
struct console;

extern int vprintf(struct console *con, const char *fmt, va_list args)
        __attribute__ ((format (printf, 2, 0)));
extern int snprintf(char *buf, int len, const char *fmt, ...)
        __attribute__ ((format (printf, 3, 4)));
extern int con_printf(struct console *con, const char *fmt, ...)
        __attribute__ ((format (printf, 2, 3)));
extern int vsnprintf(char *buf, size_t size, const char *fmt, va_list args)
        __attribute__ ((format (printf, 3, 0)));

/*
 * stdarg.h equivalents
 */
#define va_start(ap, last)	__builtin_va_start(ap, last)
#define va_arg(ap, type)	__builtin_va_arg(ap, type)
#define va_end(ap)		__builtin_va_end(ap)

/*
 * min/max/clamp/min_t/max_t/clamp_t borrowed from Linux
 */

/*
 * min()/max()/clamp() macros that also do
 * strict type-checking.. See the
 * "unnecessary" pointer comparison.
 */
#define min(x, y) ({				\
	typeof(x) _min1 = (x);			\
	typeof(y) _min2 = (y);			\
	(void) (&_min1 == &_min2);		\
	_min1 < _min2 ? _min1 : _min2; })

#define max(x, y) ({				\
	typeof(x) _max1 = (x);			\
	typeof(y) _max2 = (y);			\
	(void) (&_max1 == &_max2);		\
	_max1 > _max2 ? _max1 : _max2; })

/**
 * clamp - return a value clamped to a given range with strict typechecking
 * @val: current value
 * @min: minimum allowable value
 * @max: maximum allowable value
 *
 * This macro does strict typechecking of min/max to make sure they are of the
 * same type as val.  See the unnecessary pointer comparisons.
 */
#define clamp(val, min, max) ({			\
	typeof(val) __val = (val);		\
	typeof(min) __min = (min);		\
	typeof(max) __max = (max);		\
	(void) (&__val == &__min);		\
	(void) (&__val == &__max);		\
	__val = __val < __min ? __min: __val;	\
	__val > __max ? __max: __val; })

/*
 * ..and if you can't take the strict
 * types, you can specify one yourself.
 *
 * Or not use min/max/clamp at all, of course.
 */
#define min_t(type, x, y) ({			\
	type __min1 = (x);			\
	type __min2 = (y);			\
	__min1 < __min2 ? __min1: __min2; })

#define max_t(type, x, y) ({			\
	type __max1 = (x);			\
	type __max2 = (y);			\
	__max1 > __max2 ? __max1: __max2; })

/**
 * clamp_t - return a value clamped to a given range using a given type
 * @type: the type of variable to use
 * @val: current value
 * @min: minimum allowable value
 * @max: maximum allowable value
 *
 * This macro does no typechecking and uses temporary variables of type
 * 'type' to make all the comparisons.
 */
#define clamp_t(type, val, min, max) ({		\
	type __val = (val);			\
	type __min = (min);			\
	type __max = (max);			\
	__val = __val < __min ? __min: __val;	\
	__val > __max ? __max: __val; })

#endif