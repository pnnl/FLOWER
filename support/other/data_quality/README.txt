

Before running analize.sh:

  Make sure that:
    /bin/rm -rf /var/tmp/$USER/flower

To monitor the progress of analize.sh run:
  watch -d -n 300 ./status.sh

To find problems:
  grep -A 2 -B 2 PROBLEM output_0?

The output will look something like:
  output_06-Mismatch in FlowKey: 189010036003_140221227105_25136_8080_17
  output_06-MISMATCH:SRC_PAYLOAD:Flower,82:Flo   ,65:
  output_06:PROBLEM:./gen_wireshark_filter.pl 189010036003_140221227105_25136_8080_17 -i
  /var/sc08data/bulk/2008-11-17.14.46.31.sc08isp.pcap
  output_06-Flower:1226954795.594415,0.174351,,17,189010036003,140221227105,82,264,124,306,1,1,,,25136,8080,,,,,,,,,,A,:
  output_06-Flo   :1226954795.594415,0.174351,,17,189010036003,140221227105,65,264,124,306,1,1,,,25136,8080,,,00,00,,,,,,A,:

Run the command on the PROBLEM line:
  ./gen_wireshark_filter.pl 189010036003_140221227105_25136_8080_17 -i
  /var/sc08data/bulk/2008-11-17.14.46.31.sc08isp.pcap

The output will look like:
  wireshark -r /var/sc08data/bulk/2008-11-17.14.46.31.sc08isp.pcap -R "ip.proto
  == 17 && ((ip.src == 189.10.36.3 && ip.dst == 140.221.227.105 && udp.srcport
  == 25136 && udp.dstport == 8080) || (ip.dst == 189.10.36.3 && ip.src ==
  140.221.227.105 && udp.dstport == 25136 && udp.srcport == 8080))"

If the PROBLEM is only a few packets you can use tshark instead of wireshark.
Just substitute wireshark in the command above with "tshark -V -x" like:
  tshark -V -x -r /var/sc08data/bulk/2008-11-17.14.46.31.sc08isp.pcap -R "ip.proto
  == 17 && ((ip.src == 189.10.36.3 && ip.dst == 140.221.227.105 && udp.srcport
  == 25136 && udp.dstport == 8080) || (ip.dst == 189.10.36.3 && ip.src ==
  140.221.227.105 && udp.dstport == 25136 && udp.srcport == 8080))"

If the PROBLEM is man packets, you will probably have to run wireshark but 
without the filter. After the file loads, you can apply the filter.

If a difference between flo and flower can be determined, add it to the
bottom of the KNOWN DIFFERENCEs section of the Validate.pm.

IMPORTANT: Modify Validate.pm and search for NEWRULE

The PROBLEM above is due to the difference in way flo and flower calculate
the SRC_PAYLOAD. This PROBLEM can be determined by the condition:

#  KNOWN Difference: Flo calculates UDP payload using the length
#                    in the UDP header rather than using the length
#                    in the IP header and subtracting 8

if (
     $Obj2->{'_PROTO'}                == 17                      &&
     $Obj1->{'SRC_BYTES'}             == $Obj2->{'SRC_BYTES'}    &&
     $Obj1->{'SRC_PAYLOAD'}           != $Obj2->{'SRC_PAYLOAD'}  &&
    ($Obj1->{'SRC_PAYLOAD'}
      + ($Obj1->{'SRC_PACKETS'} * 42) == $Obj1->{'SRC_BYTES'})   &&
     $Obj2->isFlo()
   )
{
  $exceptions{'UDP_SRC_PAYLOAD'}++;
  next;
}

