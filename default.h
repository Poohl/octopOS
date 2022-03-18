
#ifndef DEFAULT_H
#define DEFAULT_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef char byte;
typedef unsigned char ubyte;
typedef signed char sbyte;

typedef unsigned int uint;

typedef void (*void_void_func_ptr)();

#define ARR_LEN(ARR) \
	(sizeof(ARR) / sizeof(ARR[0]))

void *memset(void *s, int c, size_t n) ;

void *memcpy(void *dest, const void *src, size_t n);

#endif
