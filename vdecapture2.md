# NAME

vdecapture2 -- Capture vde traffic in pcap format


# SYNOPSIS

`vdecapture2` [-o <*output*>] [-h] [<*VNL*>]


# DESCRIPTION

`vdecapture2` captures packet data from a live VDE network and saves it
to an output file in pcap format.

This is a reimplementation of the original vdecapture(1) (VirtualSquare). 
Less options or none, but more resilient on writing on fifo(7) that are
closed and reopen by readers.


# OPTIONS

  `-o` *output*
: Pcap file (default '-', meaning stdout) 

  `-h`
: Display a short help message and exit

  *VNL*
: Virtual Network Locator (e.g. vde:///tmp/vde.ctl)


# SEE ALSO
vde_plug(1), vdecapture(1), vdetcpcapture(1), vdereplay(1)
