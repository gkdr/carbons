# carbons 0.2.2
[![Build status](https://ci.appveyor.com/api/projects/status/0t32ouomatf2teld/branch/master?svg=true)](https://ci.appveyor.com/project/gkdr/carbons/branch/dev)
[![codecov](https://codecov.io/gh/gkdr/carbons/branch/master/graph/badge.svg)](https://codecov.io/gh/gkdr/carbons)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/gkdr/carbons.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/gkdr/carbons/context:cpp)

Experimental [XEP-0280: Message Carbons](https://xmpp.org/extensions/xep-0280.html) plugin for libpurple (Pidgin, Finch, etc.)

It enables you to have a consistent view of both sent and received messages between all devices which are online at the same time.

## Installation
You can find compiled versions of this plugin for both Linux and Windows on [the GitHub releases page](https://github.com/gkdr/carbons/releases).


To get the latest build for both operating systems you can head over to [the artifacts page of this project's current build on AppVeyor](https://ci.appveyor.com/project/gkdr/carbons/build/artifacts).

## Usage
On startup, it sends a feature discovery request to the server and automatically enables message carbons if the server lists it among its supported features.

If something is not working right, consult the debug window.


## Building it yourself

### Linux
1. Install the dependencies (`libpurple-dev`, `libglib2.0-dev`, `libxml2-dev`)
2. `git clone https://github.com/gkdr/carbons.git`
3. `cd carbons`
4. `make`
5. A final `make install` should copy the plugin into your libpurple plugin dir, or `make install-home` to copy it into `~/.purple/plugins`.

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
You can make use of the `Makefile.mingw` by EionRobb.
For this, you have to set up a build environment as described in https://developer.pidgin.im/wiki/BuildingWinPidgin.

## Caveats
Note that this only synchronizes messages of devices that are online at the same time - for history synchronization, MAM is needed.
There is currently no libpurple plugin for this.
