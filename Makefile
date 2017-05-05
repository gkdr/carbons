PURPLE_PLUGIN_DIR=~/.purple/plugins
PIDGIN_DIR=./pidgin-2.11.0
PURPLE_PLUGIN_SRC_DIR=$(PIDGIN_DIR)/libpurple/plugins

CC ?= gcc
PKG_CONFIG ?= pkg-config
XML2_CONFIG ?= xml2-config

LDIR=./lib
BDIR=./build
SDIR=./src
HDIR=./headers

HEADERS=-I$(HDIR)/jabber

LIBPURPLE_CFLAGS=$(shell $(PKG_CONFIG) --cflags purple)
LIBPURPLE_LDFLAGS=$(shell $(PKG_CONFIG) --cflags purple) \
		    -L$(shell $(PKG_CONFIG) --variable=plugindir purple)

PKGCFG_C=$(shell $(PKG_CONFIG) --cflags glib-2.0) \
		 $(LIBPURPLE_CFLAGS) \
		 $(shell $(XML2_CONFIG) --cflags)

PKGCFG_L=$(shell $(PKG_CONFIG) --libs glib-2.0) \
		 $(LIBPURPLE_LDFLAGS) \
		 $(shell $(XML2_CONFIG) --libs) \

CFLAGS=-std=c11 -Wall -g -Wstrict-overflow -D_XOPEN_SOURCE=700 -D_BSD_SOURCE $(PKGCFG_C) $(HEADERS)

ifneq ("$(wildcard /etc/redhat-release)","")
	LJABBER=-lxmpp
else
	LJABBER=-ljabber
endif
LFLAGS= -ldl -lm $(PKGCFG_L) $(LJABBER)

all: $(BDIR)/carbons.so

$(BDIR):
	mkdir -p build
	
$(BDIR)/carbons.so: $(SDIR)/carbons.c $(BDIR)
	$(CC) $(CFLAGS) -fPIC -c $(SDIR)/carbons.c -o $(BDIR)/carbons.o
	$(CC) -fPIC -shared $(CFLAGS) $(BDIR)/carbons.o -o $@ $(LFLAGS)
	
install: $(BDIR)/carbons.so
	mkdir -p $(PURPLE_PLUGIN_DIR)
	cp $(BDIR)/carbons.so $(PURPLE_PLUGIN_DIR)/carbons.so

.PHONY: clean
clean:
	rm -rf $(BDIR)
