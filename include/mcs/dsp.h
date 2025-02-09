#ifndef __DSP_H__
#define __DSP_H__

#include <mcs/types.h>

typedef struct {
	u32	num_samples;
	u32	num_adpcm_nibbles;
	u32	sample_rate;

	u16	loop_flag;
	u16	format;
	u32	sa;	/* loop start address */
	u32	ea;	/* loop end address */
	u32	ca;	/* current address */

	u16	coef[16];

	/* start context */
	u16	gain;
	u16	ps;
	u16	yn1;
	u16	yn2;

	/* loop context */
	u16	lps;
	u16	lyn1;
	u16	lyn2;

	u16	pad[11];
} DSPHeader;

typedef struct {
	/* start context */
	s16	coef[16];
	u16	ps;
	s16	yn1;
	s16	yn2;

	/* loop context */
	u16	loop_ps;
	s16	loop_yn1;
	s16	loop_yn2;

	u32	sa;
	u32	ea;
	u32	ca;

	BOOL	loop;
	const u8* data;
} ADPCM;

void	DSPInit(ADPCM* adpcm, const DSPHeader* header);
unsigned int DSPDecode(ADPCM* ctx, s16* out, unsigned int samples,
		unsigned int stride);

#endif
