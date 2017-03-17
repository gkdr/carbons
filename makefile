PURPLE_PLUGIN_DIR=~/.purple/plugins
PIDGIN_DIR=./pidgin-2.11.0
PURPLE_PLUGIN_SRC_DIR=$(PIDGIN_DIR)/libpurple/plugins

LDIR=./lib
BDIR=./build
SDIR=./src
HDIR=./headers

HEADERS=-I$(HDIR)/jabber

PKGCFG_C=$(shell pkg-config --cflags glib-2.0 purple)  $(shell xml2-config --cflags)
PKGCFG_L=$(shell pkg-config --libs purple glib-2.0) $(shell xml2-config --libs) -L$(shell pkg-config --variable=plugindir purple)

CFLAGS=-std=c11 -Wall -g -Wstrict-overflow -D_XOPEN_SOURCE=700 -D_BSD_SOURCE $(PKGCFG_C) $(HEADERS)

ifneq ("$(wildcard /etc/redhat-release)","")
	LFLAGS= -ldl -lm $(PKGCFG_L) -lxmpp
else
	LFLAGS= -ldl -lm $(PKGCFG_L) -ljabber
endif


all: $(BDIR)/carbons.so

$(BDIR):
	mkdir -p build
	
$(BDIR)/carbons.so: $(SDIR)/carbons.c $(BDIR)
	gcc $(CFLAGS) -fPIC -c $(SDIR)/carbons.c -o $(BDIR)/carbons.o
	gcc -fPIC -shared $(CFLAGS) $(BDIR)/carbons.o -o $@ $(LFLAGS)
	
install: $(BDIR)/carbons.so
	mkdir -p $(PURPLE_PLUGIN_DIR)
	cp $(BDIR)/carbons.so $(PURPLE_PLUGIN_DIR)/carbons.so

.PHONY: clean
clean:
	rm -rf $(BDIR)
