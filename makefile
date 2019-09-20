SRCDIR = ./src
INCDIR = ./inc
OBJDIR = ./obj

CFILES = $(wildcard $(SRCDIR)/*.c)
OFILES = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(CFILES))

INSTALL_DIR = /usr/share/ctarget/

all: clean ctarget

ctarget: $(OFILES)
	$(CC) -Wall $(OFILES) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -Wall -I $(INCDIR) -c -o $@ $^

install: ctarget
	sudo mkdir -p $(INSTALL_DIR)
	sudo cp -r src/ $(INSTALL_DIR)/src
	sudo cp -r inc/ $(INSTALL_DIR)/inc
	sudo cp ctarget $(INSTALL_DIR)/ctarget

	sudo rm -f /usr/local/bin/ctarget
	sudo ln -s $(INSTALL_DIR)/ctarget /usr/local/bin/ctarget

clean:
	rm -rf obj/*
	rm -rf ctarget
