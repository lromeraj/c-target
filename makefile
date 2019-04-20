SRCDIR = ./src
INCDIR = ./inc
OBJDIR = ./obj

CFILES = $(wildcard $(SRCDIR)/*.c)
OFILES = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(CFILES))

all: ctarget

ctarget: $(OFILES)
	$(CC) -Wall $(OFILES) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -Wall -I $(INCDIR) -c -o $@ $^

install: ctarget
	cp ctarget /usr/local/bin/

clean: ctarget
	./ctarget -e ctarget -cl
