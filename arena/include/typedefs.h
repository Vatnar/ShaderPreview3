#pragma once

#include <stdint.h>
typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef int8_t  S8;
typedef int16_t S16;
typedef int32_t S32;
typedef int64_t S64;
typedef S8      B8;
typedef S16     B16;
typedef S32     B32;
typedef S64     B64;

typedef float  F32;
typedef double F64;

typedef uintptr_t IPtr;

#define internal static
#define local_persist static

#define KB(x) (x * 1024)
#define MB(x) (KB(x) * 1024)

#define Min(A, B) (((A) < (B)) ? (A) : (B))
#define Max(A, B) (((A) > (B)) ? (A) : (B))
#define ClampTop(A, X) Min(A, X)
#define ClampBot(X, B) Max(X, B)
#define Clamp(A, X, B) (((X) < (A)) ? (A) : ((X) > (B)) ? (B) : (X))
