#ifndef __AES256_H__
#define __AES256_H__

#include <stddef.h>
#include <mcs/types.h>

/* block length in bytes - AES is 128b block only */
#define	AES_BLOCKLEN	16

#define AES_KEYLEN	32	/* key length in bytes */
#define AES_keyExpSize	240

typedef struct {
	u8	round_key[AES_keyExpSize];
	u8	iv[AES_BLOCKLEN];
} AES256;

void	AES256Init(AES256* ctx, const u8* key);
void	AES256InitIV(AES256* ctx, const u8* key, const u8* iv);
void	AES256SetIV(AES256* ctx, const u8* iv);

/* buffer size is exactly AES_BLOCKLEN bytes; 
 * you need only AES_init_ctx as IV is not used in ECB 
 * NB: ECB is considered insecure for most uses
 */
void	AES256EncryptECB(const AES256* ctx, void* buf);
void	AES256DecryptECB(const AES256* ctx, void* buf);

/* buffer size MUST be multiple of AES_BLOCKLEN;
 * Suggest https://en.wikipedia.org/wiki/Padding_(cryptography)#PKCS7 for
 * padding scheme
 * NOTES: you need to set IV in ctx via AES256InitIV() or AES256SetIV()
 *        no IV should ever be reused with the same key 
 */
void	AES256EncryptCBC(AES256* ctx, void* buf, size_t length);
void	AES256DecryptCBC(AES256* ctx, void* buf, size_t length);

/* Same function for encrypting as for decrypting. 
 * IV is incremented for every block, and used after encryption as
 * XOR-compliment for output
 * Suggesting https://en.wikipedia.org/wiki/Padding_(cryptography)#PKCS7
 * for padding scheme
 * NOTES: you need to set IV in ctx with AES256InitIV() or AES256SetIV()
 *        no IV should ever be reused with the same key 
 */
void AESXCryptCTR(AES256* ctx, void* buf, size_t length);

#endif
