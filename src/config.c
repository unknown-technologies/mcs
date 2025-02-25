#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/random.h>
#include <mcs.h>

#define	CFG_TYPE_TIMEZONE	1
#define	CFG_TYPE_SERVER		2
#define	CFG_TYPE_USERNAME	3
#define	CFG_TYPE_AUTH		4
#define	CFG_TYPE_END		0xFF

typedef struct {
	u8*		data;
	size_t		size;
	size_t		ptr;
} MEMSTREAM;

static const char* paths[] = {
	"/var/lib/mcs/config.dat",
	"/var/lib/misc/mcs.dat",
	"./mcs.dat",
	NULL
};

static BOOL MEMRead(MEMSTREAM* stream, void* out, size_t size)
{
	size_t end = stream->ptr + size;
	if(stream->ptr >= stream->size) {
		return FALSE;
	} else if(end > stream->size) {
		return FALSE;
	} else {
		memcpy(out, stream->data + stream->ptr, size);
		stream->ptr += size;
		return TRUE;
	}
}

static BOOL MEMWrite(MEMSTREAM* stream, const void* in, size_t size)
{
	size_t end = stream->ptr + size;
	if(stream->ptr >= stream->size) {
		return FALSE;
	} else if(end > stream->size) {
		return FALSE;
	} else {
		memcpy(stream->data + stream->ptr, in, size);
		stream->ptr += size;
		return TRUE;
	}
}

static void MEMSkip(MEMSTREAM* stream, size_t size)
{
	stream->ptr += size;
}

void CFGDestroy(CFG* cfg)
{
	if(cfg->timezone) {
		free((void*) cfg->timezone);
		cfg->timezone = NULL;
	}
	if(cfg->server) {
		free((void*) cfg->server);
		cfg->server = NULL;
	}
	if(cfg->username) {
		free((void*) cfg->username);
		cfg->username = NULL;
	}
	if(cfg->auth) {
		free((void*) cfg->auth);
		cfg->auth = NULL;
	}
}

void CFGLoadDefaults(CFG* cfg)
{
	memset(cfg, 0, sizeof(CFG));
	cfg->timezone = strdup("UTC");

	CFGiProcess(cfg);
}

void CFGRead(CFG* cfg)
{
	u8 machineid[16];
	OSGetMachineID(machineid);

	SHA256 sha;
	SHA256Init(&sha);
	SHA256Update(&sha, machineid, sizeof(machineid));

	u8 key[32];
	SHA256Final(&sha, key);

	const char* path = NULL;

	for(const char** p = paths; *p; p++) {
		if(!access(*p, R_OK)) {
			path = *p;
			break;
		}
	}

	if(path) {
		printf("Reading config from %s\n", path);

		struct stat statbuf;
		if(stat(path, &statbuf)) {
			printf("Cannot get metadata of config file: %s\n",
					strerror(errno));
			CFGLoadDefaults(cfg);
			return;
		}

		size_t size = statbuf.st_size;
		if(size % 16) {
			printf("Invalid config size\n");
			CFGLoadDefaults(cfg);
			return;
		}

		void* buf = malloc(size);
		if(!buf) {
			printf("Failed to allocate memory\n");
			CFGLoadDefaults(cfg);
			return;
		}

		memset(buf, 0, size);

		FILE* f = fopen(path, "rb");
		if(!f) {
			printf("Failed to read config, using defaults\n");
			CFGLoadDefaults(cfg);
			return;
		}

		if(fread(buf, statbuf.st_size, 1, f) != 1) {
			free(buf);
			printf("Config corrupt, using defaults\n");
			CFGLoadDefaults(cfg);
			return;
		} else {
			/* decrypt using AES256 */
			u8* data = buf + 16;
			size_t data_size = size - 16;
			AES256 aes;
			AES256InitIV(&aes, key, buf);
			AES256DecryptCBC(&aes, data, data_size);
			CFGiParse(cfg, data, data_size);
		}

		free(buf);
		fclose(f);
	} else {
		printf("Config not found, using defaults\n");
		CFGLoadDefaults(cfg);
		return;
	}
}

void CFGWrite(CFG* cfg)
{
	u8 machineid[16];
	OSGetMachineID(machineid);

	SHA256 sha;
	SHA256Init(&sha);
	SHA256Update(&sha, machineid, sizeof(machineid));

	u8 key[32];
	SHA256Final(&sha, key);

	size_t size = 5;
	if(cfg->timezone) {
		size += 5 + strlen(cfg->timezone);
	}
	if(cfg->server) {
		size += 5 + strlen(cfg->server);
	}
	if(cfg->username) {
		size += 5 + strlen(cfg->username);
	}
	if(cfg->auth) {
		size += 5 + CFG_AUTH_SIZE;
	}

	/* pad to AES256 block size */
	if(size % 16) {
		size += 16 - (size % 16);
	}

	size_t payload_size = size;

	size += SHA256_BLOCK_SIZE + 16;

	/* generate config data */
	MEMSTREAM stream = {
		.data = (u8*) malloc(size),
		.size = size,
		.ptr = 0
	};

	if(!stream.data) {
		printf("Failed to allocate memory\n");
		return;
	}

	memset(stream.data, 0, size);

	u8 iv[16];
	memset(iv, 0, sizeof(iv));
	if(getrandom(iv, sizeof(iv), 0) != sizeof(iv)) {
		printf("Failed to get randomness\n");
	}

	MEMWrite(&stream, iv, sizeof(iv));
	MEMSkip(&stream, SHA256_BLOCK_SIZE);

	if(cfg->timezone) {
		u8 type = CFG_TYPE_TIMEZONE;
		u32 len = strlen(cfg->timezone);
		MEMWrite(&stream, &type, sizeof(type));
		MEMWrite(&stream, &len, sizeof(len));
		MEMWrite(&stream, cfg->timezone, len);
	}

	if(cfg->server) {
		u8 type = CFG_TYPE_SERVER;
		u32 len = strlen(cfg->server);
		MEMWrite(&stream, &type, sizeof(type));
		MEMWrite(&stream, &len, sizeof(len));
		MEMWrite(&stream, cfg->server, len);
	}

	if(cfg->username) {
		u8 type = CFG_TYPE_USERNAME;
		u32 len = strlen(cfg->username);
		MEMWrite(&stream, &type, sizeof(type));
		MEMWrite(&stream, &len, sizeof(len));
		MEMWrite(&stream, cfg->username, len);
	}

	if(cfg->auth) {
		u8 type = CFG_TYPE_AUTH;
		u32 len = CFG_AUTH_SIZE;
		MEMWrite(&stream, &type, sizeof(type));
		MEMWrite(&stream, &len, sizeof(len));
		MEMWrite(&stream, cfg->auth, len);
	}

	u8 type = CFG_TYPE_END;
	u32 len = 0;
	MEMWrite(&stream, &type, sizeof(type));
	MEMWrite(&stream, &len, sizeof(len));

	void* data = stream.data + 16;
	SHA256Init(&sha);
	SHA256Update(&sha, data + SHA256_BLOCK_SIZE, payload_size);
	SHA256Final(&sha, data);

	AES256 aes;
	AES256InitIV(&aes, key, iv);
	AES256EncryptCBC(&aes, data, size - 16);

	const char* path = NULL;

	FILE* f = NULL;
	for(const char** p = paths; *p; p++) {
		f = fopen(*p, "wb");
		if(f) {
			path = *p;
			break;
		}
	}

	if(!path) {
		free(stream.data);
		printf("No writable path for config found\n");
		return;
	}

	printf("Writing config to %s\n", path);
	if(fwrite(stream.data, stream.size, 1, f) != 1) {
		printf("Failed to write config\n");
	}
	fclose(f);
	free(stream.data);
}

void CFGiParse(CFG* cfg, void* data, size_t size)
{
	MEMSTREAM stream = {
		.data = data,
		.size = size,
		.ptr = 0
	};

	u8 stored_hash[SHA256_BLOCK_SIZE];
	u8 real_hash[SHA256_BLOCK_SIZE];
	if(!MEMRead(&stream, stored_hash, sizeof(stored_hash))) {
		goto error;
	}

	SHA256 sha256;
	SHA256Init(&sha256);
	SHA256Update(&sha256, stream.data + stream.ptr,
			stream.size - stream.ptr);
	SHA256Final(&sha256, real_hash);

	if(memcmp(stored_hash, real_hash, SHA256_BLOCK_SIZE)) {
		printf("Config data is corrupt\n");
		goto error;
	}

	while(stream.ptr < stream.size) {
		u8 type;
		if(!MEMRead(&stream, &type, sizeof(type))) {
			goto error;
		}

		u32 len;
		if(!MEMRead(&stream, &len, sizeof(len))) {
			goto error;
		}

		switch(type) {
			case CFG_TYPE_TIMEZONE:
				if(cfg->timezone) {
					free((void*) cfg->timezone);
				}
				cfg->timezone = (const char*)
					malloc(len + 1);
				((char*) cfg->timezone)[len] = 0;
				if(!cfg->timezone) {
					goto error;
				}
				if(!MEMRead(&stream, (void*) cfg->timezone,
							len)) {
					goto error;
				}
				break;
			case CFG_TYPE_SERVER:
				if(cfg->server) {
					free((void*) cfg->server);
				}
				cfg->server = (const char*) malloc(len + 1);
				((char*) cfg->server)[len] = 0;
				if(!cfg->server) {
					goto error;
				}
				if(!MEMRead(&stream, (void*) cfg->server,
							len)) {
					goto error;
				}
				break;
			case CFG_TYPE_USERNAME:
				if(cfg->username) {
					free((void*) cfg->username);
				}
				cfg->username = (const char*)
					malloc(len + 1);
				((char*) cfg->username)[len] = 0;
				if(!cfg->username) {
					goto error;
				}
				if(!MEMRead(&stream, (void*) cfg->username,
							len)) {
					goto error;
				}
				break;
			case CFG_TYPE_AUTH:
				if(cfg->auth) {
					free((void*) cfg->auth);
				}
				cfg->auth = (const u8*) malloc(len);
				if(!cfg->auth) {
					goto error;
				}
				if(!MEMRead(&stream, (void*) cfg->auth,
							len)) {
					goto error;
				}
				break;
			case CFG_TYPE_END:
				goto end;
			default:
				printf("Unknown type: 0x%02X (len %d)\n",
						type, len);
				MEMSkip(&stream, len);
				break;
		}
	}

end:
	CFGiProcess(cfg);
	return;

error:
	printf("Failed to parse config\n");
	CFGLoadDefaults(cfg);
}

void CFGiProcess(CFG* cfg)
{
	if(!cfg->timezone) {
		printf("No timezone set, defaulting to UTC!\n");
		cfg->timezone  = strdup("UTC");
	}

	char buf[128];
	snprintf(buf, sizeof(buf), "/usr/share/zoneinfo/%s",
			cfg->timezone);
	if(!access(buf, R_OK)) {
		setenv("TZ", cfg->timezone, 1);
		tzset();
	} else {
		printf("Failed to set timezone to \"%s\"\n",
				cfg->timezone);
		if(cfg->timezone) {
			free((void*) cfg->timezone);
			cfg->timezone = NULL;
		}
	}
}

BOOL CFGSetTimeZone(CFG* cfg, const char* tz)
{
	char buf[128];
	snprintf(buf, sizeof(buf), "/usr/share/zoneinfo/%s", tz);
	if(!access(buf, R_OK)) {
		if(cfg->timezone) {
			free((void*) cfg->timezone);
		}
		cfg->timezone = strdup(tz);
		setenv("TZ", cfg->timezone, 1);
		tzset();
		return TRUE;
	} else {
		return FALSE;
	}
}
