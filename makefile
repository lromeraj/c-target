SRCDIR = ./src
INCDIR = ./inc
OBJDIR = ./obj

CFILES = $(wildcard $(SRCDIR)/*.c)
OFILES = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(CFILES))

INSTALL_DIR = /usr/share/ctarget

all: clean ctarget

ctarget: $(OFILES)
	$(CC) -Wall $(OFILES) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -Wall -I $(INCDIR) -c -o $@ $^

install: ctarget
	./installit

clean:
	rm -rf obj/*
	rm -rf ctarget
