#include <string.h>
#include <unistd.h>

#include <mcs.h>

void ARTDMXInit(ArtDMX* dmx, u8 sequence, u8 physical, u16 universe,
		u16 paramcnt, u8* parameters)
{
	memcpy(dmx->header, "Art-Net\0", 8);
	memset(dmx->parameters, 0, 512);
	dmx->opcode = U16L(OpDMX);
	dmx->protover = U16B(0x0E);
	dmx->sequence = sequence;
	dmx->physical = physical;
	dmx->universe = U16L(universe);
	dmx->paramcnt = U16B(paramcnt);
	memcpy(dmx->parameters, parameters, paramcnt);
}

BOOL ARTDMXSend(ArtDMX* dmx, int fd)
{
	size_t size = sizeof(ArtDMX) - 512 + U16B(dmx->paramcnt);
	ssize_t result = write(fd, dmx, size);
	return result == size;
}
