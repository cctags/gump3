CCFLAGS =

ifdef DEBUG
CCFLAGS += -DDEBUG
endif

gump3.exe: main.c
	@echo Compiling $< ..
	@gcc -Wall $(CCFLAGS) $< -o $@
