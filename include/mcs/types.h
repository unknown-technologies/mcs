#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdint.h>

typedef uint8_t		u8;
typedef uint16_t	u16;
typedef uint32_t	u32;
typedef uint64_t	u64;

typedef int8_t		s8;
typedef int16_t		s16;
typedef int32_t		s32;
typedef int64_t		s64;

typedef float		f32;
typedef double		f64;

typedef	int		BOOL;
#define	TRUE		1
#define	FALSE		0

#if 0
/* defined in xf86drmMode.h */
#ifndef __cplusplus
typedef int		bool;
#define	true		1
#define	false		0
#endif
#endif

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define	U16B(x)		(x)
#define	U32B(x)		(x)
#define	U64B(x)		(x)
#define	U16L(x)		__builtin_bswap16(x)
#define	U32L(x)		__builtin_bswap32(x)
#define	U64L(x)		__builtin_bswap64(x)
#else
#define	U16B(x)		__builtin_bswap16(x)
#define	U32B(x)		__builtin_bswap32(x)
#define	U64B(x)		__builtin_bswap64(x)
#define	U16L(x)		(x)
#define	U32L(x)		(x)
#define	U64L(x)		(x)
#endif

#endif
