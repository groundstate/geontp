Installation
------------

To build geontp you will need to install Qt4, libglut and libGLU development packages.

In the `geontp` source directory:

	qmake-qt4 geontp.pro
	make

Communicating with remote NTP servers
-------------------------------------

`geontp` requires a custom agent running on the remote server to log traffic and then report that traffic back to `geontp`
when polled. The supplied agent is `trafficrep`, which uses libpcap to capture traffic on specified interfaces
and then reports back via http when queried by `geontp`.

Configuration file
------------------

`geontp` uses a configuration file `geontp.xml`. The comments in the sample file should be enough to get you going.
The search path for this is `./:~/geontp:~/.geontp:/usr/local/share/geontp:/usr/share/geontp`
All other paths are explicit.

Databases
---------

Two plain text databases are needed. One lists cities and the other  lists IP and location information for NTP clients.
The names of these files can specified in the configuration file. The city data base consists of successive lines in the format

  City_name, latitude_degrees, latitude_minutes, longitude_degrees, longitude_minutes, [X|V|R], [Capital],[Server]

where
  X means don't display the city name
  V means draw text centred vertically of the marker
  R means draw text to the right of the marker (default is to the left)
and Capital indicates the city is a capital (and the text is in caps) and Server indicates the city hosts a server.


Power management
----------------

The `tvservice` tool is used on the Raspberry Pi. This has worked fine for me on an LCD monitor. The display and the backlight go off.

On other Linuxen+x386, YMMV. With `xset`, the backlight would go off briefly and then come back on my Ubuntu 8.04 system. However, after an update to 14.04 it worked fine.  I also tried `vbetool`, there were occasional freezes of up to 30s before the monitor turned off on one box and segfaults on another box. Unfortunately there is no standard way of controlling the monitor in Linux so you may need to tinker with the code.

Known bugs/quirks
-----------------

The power on/power off logic assumes on < off.

Website
-------

There's a bit more information and screenshots at [ninepointtwoghz.org](http://ninepointtwoghz.org/scripts/geontp.php)
