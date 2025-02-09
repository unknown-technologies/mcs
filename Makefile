#!/bin/make
export	PREFIX	:=
export	CC	:=	$(PREFIX)gcc
export	LD	:=	$(CC)
export	OBJCOPY	:=	$(PREFIX)objcopy
export	NM	:=	$(PREFIX)nm
export	SIZE	:=	$(PREFIX)size
export	BIN2O	:=	bin2o
export	GLSLANG	:=	glslang

#-------------------------------------------------------------------------------
.SUFFIXES:
#-------------------------------------------------------------------------------
TARGET		:=	mcs
INCLUDES	:=	include
SOURCES		:=	src
GLSLSOURCES	:=	glsl
FONTSOURCES	:=	fonts
DSPSOURCES	:=	dsp
BUILD		:=	build

ASAN		:=	#-fsanitize=address

CFLAGS		:=	-O3 -g -Wall -std=c99 \
			-ffunction-sections -fdata-sections \
			$(INCLUDE) -DUNIX \
			-D_XOPEN_SOURCE=500 -D_DEFAULT_SOURCE \
			-DGL_GLEXT_PROTOTYPES \
			$(ASAN)

LDFLAGS		:=	-Wl,-x -Wl,--gc-sections $(ASAN)
LIBS		:=	-lm -ldrm -lgbm -lEGL -lGLESv2 -lasound -pthread

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
GLSLFILES	:=	$(foreach dir,$(GLSLSOURCES),$(notdir $(wildcard $(dir)/*.glsl)))
FONTFILES	:=	$(foreach dir,$(FONTSOURCES),$(notdir $(wildcard $(dir)/*.fnt)))
DSPFILES	:=	$(foreach dir,$(DSPSOURCES),$(notdir $(wildcard $(dir)/*.dsp)))

ifneq ($(BUILD),$(notdir $(CURDIR)))
#-------------------------------------------------------------------------------
export	DEPSDIR	:=	$(CURDIR)/$(BUILD)
export	OFILES	:=	$(CFILES:.c=.o) \
			$(GLSLFILES:.glsl=.o) \
			$(FONTFILES:.fnt=.o) \
			$(DSPFILES:.dsp=.o)
export	VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(GLSLSOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(FONTSOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DSPSOURCES),$(CURDIR)/$(dir)) \
			$(CURDIR)
export	INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
				-I$(CURDIR)/$(BUILD) \
				-I/usr/include/libdrm
export	OUTPUT	:=	$(CURDIR)/$(TARGET)

.PHONY: $(BUILD) clean all

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@echo "[CLEAN]"
	@rm -rf $(BUILD) $(OFILES) $(OUTPUT)

all: $(BUILD)

else

#-------------------------------------------------------------------------------
# main target
#-------------------------------------------------------------------------------
.PHONY: all

all: $(TARGET)

%.o: %.c
	@echo "[CC]    $(notdir $@)"
	@$(CC) -MMD -MP -MF $(DEPSDIR)/$*.d $(CFLAGS) -c $< -o $@
	@#$(CC) -S $(CFLAGS) -fverbose-asm -o $(@:.o=.s) $< # create assembly file

%.o: %.glsl
	@echo "[GLSL]  $(notdir $@)"
	@$(GLSLANG) $<
	@$(BIN2O) -t -l$(subst .,_,$(basename $@)) -i$< -o$@

%.o: %.fnt
	@echo "[FONT]  $(notdir $@)"
	@$(BIN2O) -t -l$(subst -,_,$(subst .,_,$(basename $@))) -i$< -o$@

%.o: %.dsp
	@echo "[DSP]   $(notdir $@)"
	@$(BIN2O) -t -l$(subst .,_,$(basename $@)) -i$< -o$@

$(TARGET): $(OFILES)
	@echo "[LD]    $(notdir $@)"
	@$(LD) $(LDFLAGS) -o $@ -Wl,-Map=$@.map $(OFILES) $(LIBS)

-include $(DEPSDIR)/*.d

#-------------------------------------------------------------------------------
endif
#-------------------------------------------------------------------------------
