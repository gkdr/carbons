# carbons
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

## Usage
As some servers might not support message carbons, you have to activate them individually for every account so you can see if it is possible.
To do this, open any chat window from that account and type `/carbons on`. You should see the result immediately.
If you want to deactivate it again, type `/carbons off`.

## Caveats
I did this in one afternoon, so if something doesn't work, don't hesitate to open an issue!
