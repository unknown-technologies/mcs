#include <mcs.h>
#include <stdio.h>

#define	ORDER				2
#define	COEF_SCALING_BIT		11
#define	COEF_SCALING			2048

void DSPInit(ADPCM* adpcm, const DSPHeader* header)
{
	for(unsigned int i = 0; i < 16; i++) {
		adpcm->coef[i] = U16B(header->coef[i]);
	}

	adpcm->ps  = U16B(header->ps);
	adpcm->yn1 = U16B(header->yn1);
	adpcm->yn2 = U16B(header->yn2);

	adpcm->loop_ps  = U16B(header->lps);
	adpcm->loop_yn1 = U16B(header->lyn1);
	adpcm->loop_yn2 = U16B(header->lyn2);

	adpcm->sa = U32B(header->sa);
	adpcm->ea = U32B(header->ea);
	adpcm->ca = U32B(header->ca);

	adpcm->loop = header->loop_flag != 0;

	adpcm->data = (const u8*) &header[1];
}

unsigned int DSPDecode(ADPCM* ctx, s16* out, unsigned int samples,
		unsigned int stride)
{
	if(!ctx->loop && (ctx->ca >= ctx->ea)) {
		return 0;
	}

	if(!samples) {
		return 0;
	}

	/* initialize decoder */
	unsigned int decoded = 0;
	s16 yn1 = ctx->yn1;
	s16 yn2 = ctx->yn2;

	s16* output = out;
	u32  ca = ctx->ca;
	u8   ps = ctx->ps;

	while(decoded < samples) {
		if(ca >= ctx->ea) {
			if(ctx->loop) {
				ca = ctx->sa;
				ps = ctx->loop_ps;
				yn1 = ctx->loop_yn1;
				yn2 = ctx->loop_yn2;
			} else {
				break;
			}
		}

		/* frame header */
		if((ca & 0x0F) == 0) {
			ps = ctx->data[ca / 2];
			ca += 2;
		}

		s16 gain = 1 << (ps & 0x0F);
		s16 ci   = ps >> 4;

		u8 data = ctx->data[ca / 2];
		s16 nibble = (ca & 1) ? (data & 0x0F) : (data >> 4);

		/* fix negative value */
		nibble = ((s16) (nibble << 12)) >> 12;

		/* decode the sample */
		s32 mac = (s32) nibble * (s32) gain * COEF_SCALING;

		mac += (s32) yn1 * (s32) ctx->coef[ci * ORDER];
		mac += (s32) yn2 * (s32) ctx->coef[ci * ORDER + 1];

		/* rounding and cut off */
		mac += (COEF_SCALING >> 1);
		mac >>= COEF_SCALING_BIT;

		/* clamping */
		if(mac >  32767) mac =  32767;
		if(mac < -32768) mac = -32768;

		decoded++;
		ca++;

		*output = (s16) mac;
		output += stride;

		yn2 = yn1;
		yn1 = (s16) mac;
	}

	ctx->ca = ca;
	ctx->ps = ps;
	ctx->yn1 = yn1;
	ctx->yn2 = yn2;

	return decoded;
}
