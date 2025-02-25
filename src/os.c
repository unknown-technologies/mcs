#include <stdio.h>
#include <string.h>
#include <mcs.h>

static u8 machine_id[16];
static BOOL machine_id_valid = FALSE;

static u8 parse_hex_digit(char digit)
{
	if(digit >= '0' && digit <= '9') {
		return digit - '0';
	} else if(digit >= 'A' && digit <= 'F') {
		return digit - 'A' + 10;
	} else if(digit >= 'a' && digit <= 'f') {
		return digit - 'a' + 10;
	} else {
		return 0;
	}
}

static u8 parse_hex_byte(const char* word)
{
	return (parse_hex_digit(word[0]) << 4) |
		parse_hex_digit(word[1]);
}

void OSGetMachineID(u8* id)
{
	if(machine_id_valid) {
		memcpy(id, machine_id, sizeof(machine_id));
		return;
	}

	FILE* f = fopen("/etc/machine-id", "rt");
	if(!f) {
		printf("Failed to retrieve machine-id\n");
		return;
	}

	u8* ptr = machine_id;
	for(unsigned int i = 0; i < 16; i++) {
		char buf[2];
		if(fread(buf, 2, 1, f) != 1) {
			printf("Failed to read machine-id\n");
			fclose(f);
			return;
		}
		*(ptr++) = parse_hex_byte(buf);
	}

	fclose(f);

	memcpy(id, machine_id, sizeof(machine_id));
}
