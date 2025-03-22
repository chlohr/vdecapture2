# vdecapture2

`vdecapture2` captures packet data from a live VDE network and saves it
to an output file in pcap format.

This is a reimplementation of the original vdecapture(1) (VirtualSquare). 
Less options or none, but more resilient on writing on fifo(7) that are
closed and reopen by readers.


## Install

get the source code, from the root of the source tree run:
```
$ make
$ sudo make install
```


## Usage

`vdecapture2` [-o <*output*>] [-h] [<*VNL*>]


### Options

  `-o` *output*
: Pcap file (default '-', meaning stdout) 

  `-h`
: Display a short help message and exit

  *VNL*
: Virtual Network Locator (e.g. vde:///tmp/vde.ctl)


## Example

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


## See Also

[vde_plug](https://wiki.virtualsquare.org/#/tutorials/vdebasics),
[vdecapture](https://github.com/virtualsquare/vdecapture),
[vdetcpcapture](https://github.com/chlohr/vdetcpcapture),
[vdereplay](https://github.com/chlohr/vdereplay)
