CC      ?= gcc
CFLAGS  ?= -O2 -Wall -Wextra -pedantic -std=c99
PREFIX  ?= /usr/local
BINDIR  ?= $(PREFIX)/bin
TARGET  := pal
SRC     := src/pal.c

.PHONY: all clean install uninstall run

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $<

run: $(TARGET)
	@./$(TARGET)

install: $(TARGET)
	@install -Dm755 $(TARGET) $(DESTDIR)$(BINDIR)/$(TARGET)

uninstall:
	@rm -f $(DESTDIR)$(BINDIR)/$(TARGET)

clean:
	@rm -rf $(TARGET) build
