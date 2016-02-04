
DEVELOPERS NOTE:

This directory has a collection of PCAP files that have a single packet
that ends in .pcap and a corresponding .txt file that is a text export
from Wireshark (with all headers expanded).

The generate_test_packets.pl script reads the *.txt files and generates
the packets.hpp file with C functions. There will be two function per
.txt file and a function that returns a map of function names. The pair
of functions per .pcap file consist of a function to return a string
that represent a network flow for the .pcap file, and a function to
return a shared_ptr to a Packet (packet data in the pcap file).

In ../t_PacketParser.cpp, there is a function to call the function which
returns the map of pair of functions per .pcap file. The function
iterates over the map, parses the shared_ptr to the Packet data and
compares it to the string representing the network Flow of a single
packet.

To add a new Packet:

  - Find an interesting packet and save the packet from Wireshark as
    a .pcap file.
  - Export that data a a Text file making sure to expand all protocol
    headers.
  - Run generate_test_packets.pl to add that packet data to packets.hpp.
  - cd ..
  - touch t_PacketParser.cpp && make t_PacketParser && ./t_PacketParser
  - If no errors then all is good
  - Else you will need to do some work on generate_test_packets.pl 
    and/or ../../PacketParser.cpp to resolve the errors.
