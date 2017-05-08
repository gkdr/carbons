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
PLUGIN_CPPFLAGS=-DPURPLEPLUGINS

ifneq ("$(wildcard /etc/redhat-release)","")
	LJABBER=-lxmpp
else
	LJABBER=-ljabber
endif
LFLAGS= -ldl -lm $(PKGCFG_L) $(LJABBER)

all: $(BDIR)/carbons.so

$(BDIR):
	mkdir -p build

$(BDIR)/%.o: $(SDIR)/%.c $(BDIR)
	$(CC) $(CFLAGS) $(PLUGIN_CPPFLAGS )-c $(SDIR)/$*.c -o $@

$(BDIR)/carbons.so: $(BDIR)/carbons.o
	$(CC) -fPIC -shared $(CFLAGS) $(PLUGIN_CPPFLAGS) $(BDIR)/carbons.o -o $@ $(LFLAGS)
$(BDIR)/carbons.a: $(BDIR)/carbons.o
	$(AR) rcs $@ $^

install: $(BDIR)/carbons.so
	mkdir -p $(PURPLE_PLUGIN_DIR)
	cp $(BDIR)/carbons.so $(PURPLE_PLUGIN_DIR)/carbons.so

.PHONY: clean
clean:
	rm -rf $(BDIR)
