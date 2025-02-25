#ifndef __SHA256_H__
#define __SHA256_H__

#include <stddef.h>
#include <mcs/types.h>

#define SHA256_BLOCK_SIZE 32

typedef struct {
	u8		data[64];
	u32		state[8];
	u64		bitlen;
	unsigned int	datalen;
} SHA256;

void	SHA256Init(SHA256* ctx);
void	SHA256Update(SHA256* ctx, const void* data, size_t len);
void	SHA256Final(SHA256* ctx, u8* hash);

#endif
