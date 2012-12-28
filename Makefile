APP=gump3.exe

CC=gcc
RM=rm -f
CP=cp -v

SOURCE := $(wildcard *.c)
OBJS := $(patsubst %.c,%.o,$(SOURCE))
DEPS := $(patsubst %.c,%.d~,$(SOURCE))

CFLAGS=-Wall
LIBS=winmm

ifdef DEBUG
CFLAGS += -DDEBUG
else
CFLAGS += -DNDEBUG
endif

CFLAGS += -MMD -MF

all: $(APP)

$(APP): $(OBJS)
	$(CC) -o $(APP) $(OBJS) $(addprefix -l,$(LIBS))

.SUFFIXES: .c

.c.o:
	$(CC) $(CFLAGS) $*.d~ -c -o $@ $<

clean:
	$(RM) $(OBJS) $(DEPS) $(APP)

install:
	$(CP) $(APP) $(HOME)/../bin/

-include $(DEPS)
