# hello-ospf
This program sends a simple, customizable OSPF Hello Packet.

To compile and run, simply enter:
   make
   sudo ./hello

Root or sudo privilege is required to actually send an OSPF hello packet.
If you find this annoying, you can "setuid" with these commands:
   sudo chown root:root hello
   sudo chmod u+s hello

For a list of supported command-line arguments, type:
   ./hello --help

You'll see a lot of options for tweaking your packet. Setting some OSPF options may result in invalid packet (at current version), such as the "L" flag.
