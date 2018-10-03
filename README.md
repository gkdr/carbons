# carbons 0.2.0
Experimental XEP-0280: Message Carbons plugin for libpurple (Pidgin, Finch, etc.)

## Installation
### Linux
1. Install the dependencies (`libpurple-dev`, `libglib2.0-dev`, `libxml2-dev`)
2. `git clone https://github.com/gkdr/carbons.git`
3. `cd carbons`
4. `make`
5. A final `make install` should copy the plugin into your libpurple plugin dir.

### MacOS

Install dependencies using Homebrew.

```
brew install glib libxml2
```

Get a copy of the libpurple soure (from Pidgin), and prepare it so we can use it
during the build.

```
hg clone https://bitbucket.org/pidgin/main pidgin
cd pidgin
hg checkout v2.10.12
./configure $(./configure --help | grep -i -- --disable | awk '{ print $1 }')
```

```
make LIBPURPLE_CFLAGS=-I${PWD}/pidgin/libpurple LIBPURPLE_LDFLAGS=/Applications/Adium.app/Contents/Frameworks/libpurple.framework/libpurple LJABBER=
```

### Windows
Thanks to EionRobb, you can find a compiled dll to put in your plugin folder here: https://eion.robbmob.com/xmpp-carbons/

If you want to compile it yourself, you can make use of the `Makefile.mingw` also provided by EionRobb.
For this, you have to set up a build environment as described in https://developer.pidgin.im/wiki/BuildingWinPidgin .

## Usage
Unlike in previous versions, there is no need to interact with the plugin. On startup, it sends a feature discovery request to the server and automatically enables message carbons if the server lists it among its supported features.

If something is not working right, consult the debug window.

## Caveats
Note that this only synchronizes messages of devices that are online at the same time - for history synchronization, MAM is needed.
