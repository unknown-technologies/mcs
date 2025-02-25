#include <string.h>
#include <mcs/sha256.h>

#define ROL(a, b)	(((a) << (b)) | ((a) >> (32-(b))))
#define ROR(a, b)	(((a) >> (b)) | ((a) << (32-(b))))

#define CH(x, y, z)	(((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z)	(((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x)		(ROR((x), 2) ^ ROR((x), 13) ^ ROR((x), 22))
#define EP1(x)		(ROR((x), 6) ^ ROR((x), 11) ^ ROR((x), 25))
#define SIG0(x)		(ROR((x), 7) ^ ROR((x), 18) ^ ((x) >> 3))
#define SIG1(x)		(ROR((x), 17) ^ ROR((x), 19) ^ ((x) >> 10))

static const u32 k[64] = {
	0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5,
	0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
	0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3,
	0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174,
	0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC,
	0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
	0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7,
	0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967,
	0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13,
	0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85,
	0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3,
	0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
	0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5,
	0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3,
	0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208,
	0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2
};

void SHA256Transform(SHA256* ctx, const u8* data)
{
	u32 a, b, c, d, e, f, g, h, t1, t2, m[64];
	int i, j;

	for(i = 0, j = 0; i < 16; i++, j += 4) {
		m[i] =    (data[j] << 24)
			| (data[j + 1] << 16)
			| (data[j + 2] << 8)
			| (data[j + 3]);
	}
	for(; i < 64; i++) {
		m[i] =    SIG1(m[i - 2])
			+ m[i - 7]
			+ SIG0(m[i - 15])
			+ m[i - 16];
	}

	a = ctx->state[0];
	b = ctx->state[1];
	c = ctx->state[2];
	d = ctx->state[3];
	e = ctx->state[4];
	f = ctx->state[5];
	g = ctx->state[6];
	h = ctx->state[7];

	for(i = 0; i < 64; i++) {
		t1 = h + EP1(e) + CH(e, f, g) + k[i] + m[i];
		t2 = EP0(a) + MAJ(a, b, c);
		h = g;
		g = f;
		f = e;
		e = d + t1;
		d = c;
		c = b;
		b = a;
		a = t1 + t2;
	}

	ctx->state[0] += a;
	ctx->state[1] += b;
	ctx->state[2] += c;
	ctx->state[3] += d;
	ctx->state[4] += e;
	ctx->state[5] += f;
	ctx->state[6] += g;
	ctx->state[7] += h;
}

void SHA256Init(SHA256* ctx)
{
	ctx->datalen = 0;
	ctx->bitlen = 0;
	ctx->state[0] = 0x6A09E667;
	ctx->state[1] = 0xBB67AE85;
	ctx->state[2] = 0x3C6EF372;
	ctx->state[3] = 0xA54FF53A;
	ctx->state[4] = 0x510E527F;
	ctx->state[5] = 0x9B05688C;
	ctx->state[6] = 0x1F83D9AB;
	ctx->state[7] = 0x5BE0CD19;
}

void SHA256Update(SHA256* ctx, const void* data, size_t len)
{
	size_t i;

	const u8* bytes = (const u8*) data;

	for(i = 0; i < len; i++) {
		ctx->data[ctx->datalen++] = bytes[i];
		if(ctx->datalen == 64) {
			SHA256Transform(ctx, ctx->data);
			ctx->bitlen += 512;
			ctx->datalen = 0;
		}
	}
}

void SHA256Final(SHA256* ctx, u8* hash)
{
	unsigned int i = ctx->datalen;

	/* Pad whatever data is left in the buffer. */
	if(ctx->datalen < 56) {
		ctx->data[i++] = 0x80;
		while(i < 56) {
			ctx->data[i++] = 0x00;
		}
	} else {
		ctx->data[i++] = 0x80;
		while(i < 64) {
			ctx->data[i++] = 0x00;
		}
		SHA256Transform(ctx, ctx->data);
		memset(ctx->data, 0, 56);
	}

	/* Append to the padding the total message's length in bits and
	 * transform. */
	ctx->bitlen += ctx->datalen * 8;
	ctx->data[63] = (u8)  ctx->bitlen;
	ctx->data[62] = (u8) (ctx->bitlen >>  8);
	ctx->data[61] = (u8) (ctx->bitlen >> 16);
	ctx->data[60] = (u8) (ctx->bitlen >> 24);
	ctx->data[59] = (u8) (ctx->bitlen >> 32);
	ctx->data[58] = (u8) (ctx->bitlen >> 40);
	ctx->data[57] = (u8) (ctx->bitlen >> 48);
	ctx->data[56] = (u8) (ctx->bitlen >> 56);
	SHA256Transform(ctx, ctx->data);

	/* Since this implementation uses little endian byte ordering
	 * and SHA uses big endian, reverse all the bytes when copying
	 * the final state to the output hash. */
	for(i = 0; i < 8; i++) {
		u32 tmp = ctx->state[i];
		*(hash++) = (u8) (tmp >> 24);
		*(hash++) = (u8) (tmp >> 16);
		*(hash++) = (u8) (tmp >>  8);
		*(hash++) = (u8)  tmp;
	}
}
