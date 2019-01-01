PURPLE_PLUGIN_DIR=~/.purple/plugins
#PIDGIN_DIR=./pidgin-2.11.0
#PURPLE_PLUGIN_SRC_DIR=$(PIDGIN_DIR)/libpurple/plugins

CC ?= gcc
PKG_CONFIG ?= pkg-config

LDIR=./lib
BDIR=./build
SDIR=./src
HDIR=./headers
TDIR=./test
PURPLE_DIR=$(shell $(PKG_CONFIG) --variable=plugindir purple)

GLIB_CFLAGS ?= $(shell $(PKG_CONFIG) --cflags glib-2.0)
GLIB_LDFLAGS ?= $(shell $(PKG_CONFIG) --libs glib-2.0)

LIBPURPLE_CFLAGS ?= $(shell $(PKG_CONFIG) --cflags purple)
LIBPURPLE_LDFLAGS ?= $(shell $(PKG_CONFIG) --cflags purple) -L$(PURPLE_DIR)

XML2_CONFIG ?= xml2-config
XML2_CFLAGS ?= $(shell $(XML2_CONFIG) --cflags)
XML2_LDFLAGS ?= $(shell $(XML2_CONFIG) --libs)


HEADERS=-I$(HDIR)/jabber

PKGCFG_C=$(GLIB_CFLAGS) \
	 $(LIBPURPLE_CFLAGS) \
	 $(XML2_CFLAGS)

PKGCFG_L=$(GLIB_LDFLAGS) \
	 $(LIBPURPLE_LDFLAGS) \
	 $(XML2_LDFLAGS)

CFLAGS=-std=c11 -Wall -g -Wstrict-overflow -D_XOPEN_SOURCE=700 -D_BSD_SOURCE -D_DEFAULT_SOURCE $(PKGCFG_C) $(HEADERS)
CFLAGS_C= $(CFLAGS) -fPIC -shared
PLUGIN_CPPFLAGS=-DPURPLE_PLUGINS

ifneq ("$(wildcard /etc/redhat-release)","")
	LJABBER?=-lxmpp
else
	LJABBER?=-ljabber
endif

LFLAGS= -ldl -lm $(PKGCFG_L) $(LJABBER)
LFLAGS_T= $(LFLAGS) -lpurple -lcmocka -Wl,-rpath,$(PURPLE_DIR) -Wl,--wrap=purple_account_get_username

all: $(BDIR)/carbons.so

$(BDIR):
	mkdir -p build

$(BDIR)/%.o: $(SDIR)/%.c $(BDIR)
	$(CC) $(CFLAGS_C) $(PLUGIN_CPPFLAGS) -c $(SDIR)/$*.c -o $@

$(BDIR)/carbons.so: $(BDIR)/carbons.o
	$(CC) $(CFLAGS_C) $(PLUGIN_CPPFLAGS) $(BDIR)/carbons.o -o $@ $(LFLAGS)
$(BDIR)/carbons.a: $(BDIR)/carbons.o
	$(AR) rcs $@ $^

install: $(BDIR)/carbons.so
	mkdir -p $(PURPLE_PLUGIN_DIR)
	cp $(BDIR)/carbons.so $(PURPLE_PLUGIN_DIR)/carbons.so

.PHONY: test
test: $(TDIR)/test_carbons.c $(BDIR)
	$(CC) $(CFLAGS) -c $< -o $(BDIR)/$@.o
	$(CC) $(CFLAGS) $(PURPLE_DIR)/libjabber.so.0 $(BDIR)/$@.o -o $(BDIR)/$@ $(LFLAGS_T)
	-$(BDIR)/$@

.PHONY: clean
clean:
	rm -rf $(BDIR)
