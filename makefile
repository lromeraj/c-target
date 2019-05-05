SRCDIR = ./src
INCDIR = ./inc
OBJDIR = ./obj

CFILES = $(wildcard $(SRCDIR)/*.c)
OFILES = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(CFILES))

all: clean ctarget

ctarget: $(OFILES)
	$(CC) -Wall $(OFILES) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -Wall -I $(INCDIR) -c -o $@ $^

install: ctarget
	sudo cp ctarget /usr/local/bin/

clean:
	rm -rf obj/*
	rm -rf ctarget
