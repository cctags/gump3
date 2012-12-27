APP=gump3.exe

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

all: $(APP)

$(APP): $(OBJS)
	gcc -o $(APP) $(OBJS) $(addprefix -l,$(LIBS))

.SUFFIXES: .c

.c.o:
	gcc $(CFLAGS) -MMD -MF $*.d~ -c -o $@ $<

clean:
	rm -f *.o *.d~ $(APP)

-include $(DEPS)
