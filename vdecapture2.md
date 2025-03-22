# NAME

vdecapture2 -- Capture vde traffic in pcap format


# SYNOPSIS

`vdecapture2` [-o <*output*>] [-h] [<*VNL*>]


# DESCRIPTION

`vdecapture2` captures packet data from a live VDE network and saves it
to an output file in pcap format.

This is a reimplementation of the original vdecapture(1) (VirtualSquare). 
Fewer or no options, but more resistant to writing on fifo(7) which are
closed and reopened by readers.


# OPTIONS

  `-o` *output*
: Pcap file (default '-', meaning stdout) 

  `-h`
: Display a short help message and exit

  *VNL*
: Virtual Network Locator (e.g. vde:///tmp/vde.ctl)


# EXAMPLE

One create a fifo file.  vdecapture2 writes to it, and is connected to the
default vde plug.  tshark reads the fifo, is stopped and restarted.

```
$ mknod fifo.pcap
$ vdecapture2 -o fifo.pcap &
$ tshark -i fifo.pcap
  ^C
$ tshark -i fifo.pcap
  ^C
$ kill %%
```


# SEE ALSO
vde_plug(1), vdecapture(1), vdetcpcapture(1), vdereplay(1)
