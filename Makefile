CC := g++
CFLAGS := -Wall -pedantic -Os
SERVER_SRCFILES := $(shell git ls-files src/) example-server.cc
OUTDIR := build/out/Default
SERVER_TARGET := example-server

all: $(SERVER_TARGET)

clean:
	rm -rf build/out/ build/src/
	rm -f $(SERVER_TARGET)

build:
	./configure

$(OUTDIR)/$(SERVER_TARGET): $(SERVER_SRCFILES) build
	make -C build $(SERVER_TARGET)

$(SERVER_TARGET): $(OUTDIR)/$(SERVER_TARGET)
	@if [ ! -e "$@" ]; then echo -n "Creating ./$@ symlink..."; ln -sf $< $@; echo " done!"; fi

.PHONY: all clean
