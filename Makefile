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

PURPLE_PLUGIN_DIR ?= $(shell $(PKG_CONFIG) --variable=plugindir purple)
PURPLE_HOME_PLUGIN_DIR=$(HOME)/.purple/plugins

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

FLAGS+=-std=c11 -Wall -g -Wstrict-overflow -D_XOPEN_SOURCE=700 -D_BSD_SOURCE -D_DEFAULT_SOURCE
CFLAGS=$(FLAGS) $(PKGCFG_C) $(HEADERS)
CFLAGS_C= $(CFLAGS) -fPIC -shared
CFLAGS_T= $(CFLAGS) -O0
PLUGIN_CPPFLAGS=-DPURPLE_PLUGINS

ifneq ("$(wildcard /etc/redhat-release)","")
	LJABBER?=-lxmpp
else
	LJABBER?=-ljabber
endif

LFLAGS= -ldl -lm $(PKGCFG_L) $(LJABBER) -Wl,-rpath,$(PURPLE_DIR)
LFLAGS_T= $(LFLAGS) -lpurple -lcmocka -Wl,-rpath,$(PURPLE_DIR) \
	-Wl,--wrap=purple_account_is_connected \
	-Wl,--wrap=purple_account_get_connection \
	-Wl,--wrap=purple_account_get_protocol_id \
	-Wl,--wrap=purple_account_get_username \
	-Wl,--wrap=purple_accounts_get_handle \
	-Wl,--wrap=purple_accounts_get_all_active \
	-Wl,--wrap=purple_debug_error \
	-Wl,--wrap=purple_debug_warning \
	-Wl,--wrap=purple_connection_get_account \
	-Wl,--wrap=purple_connection_get_protocol_data \
	-Wl,--wrap=purple_find_conversation_with_account \
	-Wl,--wrap=purple_conversation_new \
	-Wl,--wrap=purple_conversation_write \
	-Wl,--wrap=purple_plugins_find_with_id \
	-Wl,--wrap=purple_signal_connect \
	-Wl,--wrap=purple_signal_connect_priority \
	-Wl,--wrap=jabber_add_feature \
	-Wl,--wrap=jabber_iq_send

all: $(BDIR)/carbons.so

$(BDIR):
	mkdir -p build

$(BDIR)/%.o: $(SDIR)/%.c $(BDIR)
	$(CC) $(CFLAGS_C) $(PLUGIN_CPPFLAGS) -c $(SDIR)/$*.c -o $@

$(BDIR)/carbons.so: $(BDIR)/carbons.o
	$(CC) $(CFLAGS_C) $(PLUGIN_CPPFLAGS) $(BDIR)/carbons.o -o $@ $(LFLAGS)
$(BDIR)/carbons.a: $(BDIR)/carbons.o
	$(AR) rcs $@ $^

$(BDIR)/carbons.dll: $(BDIR)/carbons.o
	$(CC) $(CFLAGS_C) $(PLUGIN_CPPFLAGS) $(BDIR)/carbons.o -o $@ $(LFLAGS)

WIN_CC ?= i686-w64-mingw32-gcc-win32
WIN32_DEV_DIR ?= win32_dev
GLIB_DIR ?= glib-2.28.8
GLIB_PATH = ./$(WIN32_DEV_DIR)/$(GLIB_DIR)
WIN_PIDGIN_PATH = ./$(WIN32_DEV_DIR)/pidgin-2.13.0-win32bin
LIBXML2_DIR ?= libxml2-2.9.2_daa1
LIBXML2_PATH = ./$(WIN32_DEV_DIR)/$(LIBXML2_DIR)
WIN_HEADERS ?= -I$(GLIB_PATH)/include/glib-2.0 \
	-I$(GLIB_PATH)/lib/glib-2.0/include \
	-I/usr/include/libpurple \
	-I./headers/jabber \
	-I$(LIBXML2_PATH)/include/libxml2
WIN_CFLAGS += $(FLAGS) $(WIN_HEADERS) $(PLUGIN_CPPFLAGS) -fPIC -shared

WIN_LFLAGS = -L$(GLIB_PATH)/lib -lglib-2.0 -L$(WIN_PIDGIN_PATH) -lpurple -ljabber -L$(LIBXML2_PATH)/lib -lxml2 -static-libgcc

windeps:
	mkdir -p $(WIN32_DEV_DIR)
	wget -nc -P $(WIN32_DEV_DIR) https://ftp.gnome.org/mirror/gnome.org/binaries/win32/glib/2.28/glib-dev_2.28.8-1_win32.zip
	unzip -n $(WIN32_DEV_DIR)/glib-dev_2.28.8-1_win32.zip -d $(GLIB_PATH)
	-wget -nc -O $(WIN32_DEV_DIR)/pidgin-2.13.0-win32-bin.zip https://sourceforge.net/projects/pidgin/files/Pidgin/2.13.0/pidgin-2.13.0-win32-bin.zip/download
	unzip -n $(WIN32_DEV_DIR)/pidgin-2.13.0-win32-bin.zip -d $(WIN32_DEV_DIR)
	wget -nc -P $(WIN32_DEV_DIR) https://developer.pidgin.im/static/win32/libxml2-2.9.2_daa1.tar.gz
	-tar xvzkf $(WIN32_DEV_DIR)/libxml2-2.9.2_daa1.tar.gz --directory $(WIN32_DEV_DIR)

win: $(SDIR)/carbons.c $(BDIR) windeps
	$(WIN_CC) $(WIN_CFLAGS) -c ./src/carbons.c -o $(BDIR)/carbons_win.o
	$(WIN_CC) $(WIN_CFLAGS) $(BDIR)/carbons_win.o -o $(BDIR)/carbons.dll $(WIN_LFLAGS)

install: $(BDIR)/carbons.so
	install -Dm0644 $(BDIR)/carbons.so $(DESTDIR)$(PURPLE_PLUGIN_DIR)/carbons.so

install-home: $(BDIR)/carbons.so
	install -Dm0644 $(BDIR)/carbons.so $(PURPLE_HOME_PLUGIN_DIR)/carbons.so

.PHONY: test
test: $(TDIR)/test_carbons.c $(BDIR)
	$(CC) $(CFLAGS_T) -c $< -o $(BDIR)/$@.o
	$(CC) $(CFLAGS_T) --coverage  -c $(SDIR)/carbons.c -o $(BDIR)/carbons_coverage.o
	$(CC) $(CFLAGS_T) --coverage $(PURPLE_DIR)/libjabber.so.0 $(BDIR)/$@.o $(BDIR)/carbons_coverage.o -o $(BDIR)/$@ $(LFLAGS_T)
	-$(BDIR)/$@ 2>&1 | grep -Ev ".*CRITICAL.*" | tr -s '\n' # filter annoying and irrelevant glib output

.PHONY: coverage
coverage: test
	gcovr -r . --html --html-details -o build/coverage.html
	gcovr -r . -s

.PHONY: clean
clean:
	rm -rf $(BDIR)

.PHONY: clean-all
clean-all: clean
	rm -rf $(WIN32_DEV_DIR)
