# carbons 0.1.3
Experimental XEP-0280: Message Carbons plugin for libpurple (Pidgin, Finch, etc.)

## Installation
### Linux, macOS
1. Install the dependencies (`libpurple-dev`, `libglib2.0-dev`, `libxml2-dev`)
2. `git clone https://github.com/gkdr/carbons.git`
3. `cd carbons`
4. `make`
5. A final `make install` should copy the plugin into your libpurple plugin dir.

### Windows
Thanks to EionRobb, you can find a compiled dll to put in your plugin folder here: https://eion.robbmob.com/xmpp-carbons/

If you want to compile it yourself, you can make use of the `Makefile.mingw` also provided by EionRobb.
For this, you have to set up a build environment as described in https://developer.pidgin.im/wiki/BuildingWinPidgin .

## Usage
As some servers might not support message carbons or for some reason disallow enabling them, you have to activate them individually for every account.
To do this, open any chat window from that account and type `/carbons on`. You should see the result immediately (success or error). It will then enable carbons at every startup.
If you want to deactivate the plugin again, type `/carbons off`.

## Caveats
I did this in one afternoon, so if something doesn't work, don't hesitate to open an issue!

Also note that this only synchronizes messages of devices that are online at the same time - for history synchronization, MAM is needed.
