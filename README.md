# DWM Jonwin

My dwm configuration with flake.

dwm is an extremely fast, small, and dynamic window manager for X.

<img src="img.png" alt="dwm image">

## NixOS Install

Add this repo as a flake input in your flake.nix:

    inputs = {
        dwm = {
            url = "github:jonwin1/dwm-jonwin";
            inputs.nixpkgs.follows = "nixpkgs";
        };
    };

Then add the following to your configuration.nix to enable dwm:

    services = {
        xserver = {
            windowManager.dwm = {
                enable = true;
                package = inputs.dwm.packages."x86_64-linux".default;
            };
        };
    };

## Regular Install

### Requirements

In order to build dwm you need the Xlib header files.

### Installation

Edit config.mk to match your local setup (dwm is installed into
the /usr/local namespace by default).

Afterwards enter the following command to build and install dwm (if
necessary as root):

    make clean install

### Running dwm

Add the following line to your .xinitrc to start dwm using startx:

    exec dwm

In order to connect dwm to a specific display, make sure that
the DISPLAY environment variable is set correctly, e.g.:

    DISPLAY=foo.bar:1 exec dwm

(This will start dwm on display :1 of the host foo.bar.)

In order to display status info in the bar, you can do something
like this in your .xinitrc:

    while xsetroot -name "`date` `uptime | sed 's/.*,//'`"
    do
    	sleep 1
    done &
    exec dwm

