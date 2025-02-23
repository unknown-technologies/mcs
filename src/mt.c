#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

#include <mcs.h>

BOOL	MTOpen(MT* mt, const char* filename);
void	MTProcess(MT* mt, struct input_event* event);

#ifdef DEBUG
static const char* EVGetTypeName(u16 type)
{
	switch(type) {
		case EV_SYN:
			return "SYN";
		case EV_KEY:
			return "KEY";
		case EV_REL:
			return "REL";
		case EV_ABS:
			return "ABS";
		case EV_MSC:
			return "MSC";
		case EV_SW:
			return "SW";
		case EV_LED:
			return "LED";
		case EV_SND:
			return "SND";
		case EV_REP:
			return "REP";
		case EV_FF:
			return "FF";
		case EV_PWR:
			return "PWR";
		case EV_FF_STATUS:
			return "FF_STATUS";
		default:
			return "???";
	}
}

static const char* EVGetABSName(u16 code)
{
	switch(code) {
		case ABS_X:
			return "ABS_X";
		case ABS_Y:
			return "ABS_Y";
		case ABS_Z:
			return "ABS_Z";
		case ABS_RX:
			return "ABS_RX";
		case ABS_RY:
			return "ABS_RY";
		case ABS_RZ:
			return "ABS_RZ";
		case ABS_THROTTLE:
			return "ABS_THROTTLE";
		case ABS_RUDDER:
			return "ABS_RUDDER";
		case ABS_WHEEL:
			return "ABS_WHEEL";
		case ABS_GAS:
			return "ABS_GAS";
		case ABS_BRAKE:
			return "ABS_BRAKE";
		case ABS_HAT0X:
			return "ABS_HAT0X";
		case ABS_HAT0Y:
			return "ABS_HAT0Y";
		case ABS_HAT1X:
			return "ABS_HAT1X";
		case ABS_HAT1Y:
			return "ABS_HAT1Y";
		case ABS_HAT2X:
			return "ABS_HAT2X";
		case ABS_HAT2Y:
			return "ABS_HAT2Y";
		case ABS_HAT3X:
			return "ABS_HAT3X";
		case ABS_HAT3Y:
			return "ABS_HAT3Y";
		case ABS_PRESSURE:
			return "ABS_PRESSURE";
		case ABS_DISTANCE:
			return "ABS_DISTANCE";
		case ABS_TILT_X:
			return "ABS_TILT_X";
		case ABS_TILT_Y:
			return "ABS_TILT_Y";
		case ABS_TOOL_WIDTH:
			return "ABS_TOOL_WIDTH";
		case ABS_VOLUME:
			return "ABS_VOLUME";
		case ABS_PROFILE:
			return "ABS_PROFILE";
		case ABS_MISC:
			return "ABS_MISC";
		case ABS_MT_SLOT:
			return "ABS_MT_SLOT";
		case ABS_MT_TOUCH_MAJOR:
			return "ABS_MT_TOUCH_MAJOR";
		case ABS_MT_TOUCH_MINOR:
			return "ABS_MT_TOUCH_MINOR";
		case ABS_MT_WIDTH_MAJOR:
			return "ABS_MT_WIDTH_MAJOR";
		case ABS_MT_WIDTH_MINOR:
			return "ABS_MT_WIDTH_MINOR";
		case ABS_MT_ORIENTATION:
			return "ABS_MT_ORIENTATION";
		case ABS_MT_POSITION_X:
			return "ABS_MT_POSITION_X";
		case ABS_MT_POSITION_Y:
			return "ABS_MT_POSITION_Y";
		case ABS_MT_TOOL_TYPE:
			return "ABS_MT_TOOL_TYPE";
		case ABS_MT_BLOB_ID:
			return "ABS_MT_BLOB_ID";
		case ABS_MT_TRACKING_ID:
			return "ABS_MT_TRACKING_ID";
		case ABS_MT_PRESSURE:
			return "ABS_MT_PRESSURE";
		case ABS_MT_DISTANCE:
			return "ABS_MT_DISTANCE";
		case ABS_MT_TOOL_X:
			return "ABS_MT_TOOL_X";
		case ABS_MT_TOOL_Y:
			return "ABS_MT_TOOL_Y";
		default:
			return "???";
	}
}
#endif

BOOL MTInit(MT* mt)
{
	char buf[256];
	FILE* f = fopen("/proc/bus/input/devices", "rt");
	if(!f) {
		printf("Failed to open device list: %s\n", strerror(errno));
		return FALSE;
	}

	char name[256] = { 0 };
	char handler[256] = { 0 };
	char dev[256] = { 0 };

	while(fgets(buf, sizeof(buf), f)) {
		buf[255] = 0;
		switch(*buf) {
			case 'I':
				/* new descriptor, ignore */
				memset(name, 0, sizeof(name));
				memset(handler, 0, sizeof(handler));
				break;
			case 'N':
				/* name */
				memset(name, 0, sizeof(name));
				char* start = strstr(buf, "Name=");
				if(start) {
					memcpy(name, start + 6,
							strlen(start + 6));
					char* c = strchr(name, '"');
					if(c) {
						*c = 0;
					}
				}
				break;
			case 'H':
				/* handler */
				memcpy(handler, buf, sizeof(buf));
				break;
			case 'B':
				if(strstr(buf, "ABS=")) {
					/* found it */
					char* event = strstr(handler,
							"event");
					if(event) {
						char* c = strchr(event, ' ');
						if(c) {
							*c = 0;
						} else {
							c = strchr(event, '\n');
							if(c) {
								*c = 0;
							}
						}
						snprintf(dev, sizeof(dev),
								"/dev/input/%s",
								event);
						goto found;
					}
				}
				break;
		}
	}

	fclose(f);
	return FALSE;

found:
	fclose(f);

	printf("name: \"%s\", device=%s\n", name, dev);
	return MTOpen(mt, dev);
}

BOOL MTOpen(MT* mt, const char* filename)
{
	mt->fd = open(filename, O_RDONLY | O_NONBLOCK);
	if(mt->fd < 0) {
		printf("Failed to open input device: %s\n", strerror(errno));
		return FALSE;
	}

	struct input_absinfo slots;
	if(ioctl(mt->fd, EVIOCGABS(ABS_MT_SLOT), &slots)) {
		printf("ioctl failed (EVIOCGABS): %s\n", strerror(errno));
		return FALSE;
	}

	struct input_absinfo axis_x;
	if(ioctl(mt->fd, EVIOCGABS(ABS_X), &axis_x)) {
		printf("ioctl failed (EVIOCGABS): %s\n", strerror(errno));
		return FALSE;
	}

	struct input_absinfo axis_y;
	if(ioctl(mt->fd, EVIOCGABS(ABS_Y), &axis_y)) {
		printf("ioctl failed (EVIOCGABS): %s\n", strerror(errno));
		return FALSE;
	}

	int width = axis_x.maximum - axis_x.minimum + 1;
	int height = axis_y.maximum - axis_y.minimum + 1;

	mt->width = width;
	mt->height = height;
	mt->axis_x_min = axis_x.minimum;
	mt->axis_y_min = axis_y.minimum;
	mt->slot_cnt = slots.maximum - slots.minimum + 1;
	mt->slot_min = slots.minimum;
	mt->slots = (MT_SLOT*) malloc(mt->slot_cnt * sizeof(MT_SLOT));
	memset(mt->slots, 0, mt->slot_cnt * sizeof(MT_SLOT));
	mt->slot = NULL;

	printf("Multitouch support: %d point\n", mt->slot_cnt);
	printf("Touch Size: %dx%d\n", mt->width, mt->height);

	/* start with slot 0 */
	mt->slot = MTGetSlot(mt, 0);

	return TRUE;
}

#define	BUFSZ 128
void MTPoll(MT* mt)
{
	if(mt->fd < 0) {
		return;
	}

	for(int i = 0; i < mt->slot_cnt; i++) {
		mt->slots[i].fresh = FALSE;
		mt->slots[i].dx = 0;
		mt->slots[i].dy = 0;
	}

	struct input_event events[BUFSZ];

	errno = 0;
	ssize_t sz = read(mt->fd, events, sizeof(events));
	if(sz == 0) {
		return;
	} else if(sz == -1 && errno == EAGAIN) {
		return;
	} else if(sz < 0) {
		printf("read failed: %s\n", strerror(errno));
		return;
	}

	size_t eventcnt = sz / sizeof(struct input_event);
	for(size_t i = 0; i < eventcnt; i++) {
		MTProcess(mt, &events[i]);
	}
}

MT_SLOT* MTGetSlot(MT* mt, int id)
{
	int idx = id - mt->slot_min;
	if(idx < 0 || idx >= mt->slot_cnt) {
		printf("ERROR: Invalid slot ID %d\n", idx);
		exit(1);
	}
	return &mt->slots[idx];
}

void MTProcess(MT* mt, struct input_event* event)
{
#ifdef DEBUG
	const char* name = EVGetTypeName(event->type);
	const char* code = "???";
	if(event->type == EV_ABS) {
		code = EVGetABSName(event->code);
	}
	printf("event: type=%d [%s], code=%d [%s], value=%d\n",
			event->type, name, event->code, code, event->value);
#endif

	if(event->type == EV_ABS) {
		switch(event->code) {
			case ABS_MT_SLOT:
				mt->slot = MTGetSlot(mt, event->value);
				break;
			case ABS_MT_TRACKING_ID:
				if(mt->slot) {
					mt->slot->new_id = event->value;
					mt->slot->new_active =
						event->value != -1;
				}
				break;
			case ABS_MT_POSITION_X:
				if(mt->slot) {
					mt->slot->new_x = event->value -
						mt->axis_x_min;
				}
				break;
			case ABS_MT_POSITION_Y:
				if(mt->slot) {
					mt->slot->new_y = event->value -
						mt->axis_y_min;
				}
				break;
		}
	} else if(event->type == EV_SYN) {
		/* update ALL slots here */
		for(unsigned int i = 0; i < mt->slot_cnt; i++) {
			MT_SLOT* slot = &mt->slots[i];
			if(slot->id != slot->new_id ||
					slot->active != slot->new_active) {
				/* a fresh contact point */
				slot->fresh = TRUE;
				slot->dx = 0;
				slot->dy = 0;
			} else {
				/* an existing contact point */
				slot->dx = slot->new_x - slot->x;
				slot->dy = slot->new_y - slot->y;
			}

			slot->id = slot->new_id;
			slot->active = slot->new_active;
			slot->x = slot->new_x;
			slot->y = slot->new_y;
		}
	}
}

void MTClose(MT* mt)
{
	if(mt->fd >= 0) {
		close(mt->fd);
		mt->fd = -1;

	}

	if(mt->slots) {
		free(mt->slots);
		mt->slots = NULL;
	}
}
