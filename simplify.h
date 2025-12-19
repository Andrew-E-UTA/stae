#ifndef _STAE_SIMPLIFY_H
#define _STAE_SIMPLIFY_H

#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#define nullptr ((void*)0)

#define inRange(n, min, max) size_t n = min; n < max; n++

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float   f32;
typedef double  f64;
#endif//_STAE_SIMPLIFY_H