#!/usr/bin/perl
# -w

# Input format: 205166076012_140221251250_10341_53_17

use Getopt::Long;


my $FRAGMENT = 0;
my $HELP     = 0;
my $DEBUG    = 0;
my $FLOW_KEY = '';
my $INFILE   = '';


sub setFlowKey
{
  my($flow_key) = @_;

  my $count = split('_', $flow_key);

  if (5 != $count)
  {
    print("\n");
    print("  ERROR: Invalid flow key\n");
    usage();
    exit(1);
  }

  $FLOW_KEY = $flow_key;

  return;
}


sub usage()
{
  print("\n");
  print("  Usage: $0 [dhf] flow_key -i infile.pcap\n\n");
  print("    -d = Turn on debug output\n");
  print("    -f = Fragmented\n");
  print("    -h = Help\n");
  print("    -i = Pcap Filename\n");
  print("\n");
  print("  Example: $0 130020010005_130030020010_13405_80_17 -i capture.pcap\n");
  print("\n");
  print("    where flow_key is src-ip_dst-ip_src-port_dst-port_protocol\n");
  print("\n");

  return;
}


sub init()
{
  Getopt::Long::Configure("bundling");
  Getopt::Long::Configure("permute");
  GetOptions ("d+" => \$main::DEBUG, "f+" => \$FRAGMENT, "h+" => \$HELP, "i=s" => \$INFILE, '<>' => \&setFlowKey);

  if ($HELP)
  {
    usage();
    exit(0);
  }

  if (! $FLOW_KEY)
  {
    print("\n");
    print("  ERROR: Missing flow key\n");
    usage();
    exit(1);
  }

  return;
}


sub getIp($)
{
  my($ip) = @_;

  my($oct1, $oct2, $oct3, $oct4) = $ip =~ m/(\d{3,3})(\d{3,3})(\d{3,3})(\d{3,3})/;

  ($oct1) =~ s/^[0]*([0-9])/$1/;
  ($oct2) =~ s/^[0]*([0-9])/$1/;
  ($oct3) =~ s/^[0]*([0-9])/$1/;
  ($oct4) =~ s/^[0]*([0-9])/$1/;

  return(join('.', $oct1, $oct2, $oct3, $oct4));
}


sub main()
{
  my($src_ip, $dst_ip, $src_port, $dst_port, $protocol, $protoname);

  ($src_ip, $dst_ip, $src_port, $dst_port, $protocol) = split('_', $FLOW_KEY);
  $src_ip = getIp($src_ip);
  $dst_ip = getIp($dst_ip);
  $main::DEBUG && print("SRC IP   :$src_ip:\n");
  $main::DEBUG && print("DST IP   :$dst_ip:\n");
  $main::DEBUG && print("SRC PORT :$src_port:\n");
  $main::DEBUG && print("DST PORT :$dst_port:\n");
  $main::DEBUG && print("PROTOCOL :$protocol:\n");

  if ($protocol == 17)
  {
    $protoname = 'udp';
  }

  if ($protocol == 6)
  {
    $protoname = 'tcp';
  }

  if ($protocol == 1)
  {
    $protoname = 'icmp';
  }

  my $output;

  if (($src_port && $dst_port) && ! $FRAGMENT)  # We have a SRC Port and DST Port
  {
    $output = "ip.proto == $protocol && ((ip.src == $src_ip && ip.dst == $dst_ip && $protoname.srcport == $src_port && $protoname.dstport == $dst_port) || (ip.dst == $src_ip && ip.src == $dst_ip && $protoname.dstport == $src_port && $protoname.srcport == $dst_port))";
  }
  elsif ($src_port && ! $FRAGMENT)            # We have a SRC Port
  {
    $output = "ip.proto == $protocol && ((ip.src == $src_ip && ip.dst == $dst_ip && $protoname.srcport == $src_port) || (ip.dst == $src_ip && ip.src == $dst_ip && $protoname.dstport == $src_port))";
  }
  elsif ($dst_port && ! $FRAGMENT)            # We have a DST Port
  {
    $output = "ip.proto == $protocol && ((ip.src == $src_ip && ip.dst == $dst_ip && $protoname.dstport == $dst_port) || (ip.dst == $src_ip && ip.src == $dst_ip && $protoname.srcport == $dst_port))";
  }
  else                         # We DONT have SRC or DST Port
  {
    $output = "ip.proto == $protocol && ((ip.src == $src_ip && ip.dst == $dst_ip) || (ip.dst == $src_ip && ip.src == $dst_ip))";
  }

  print("tshark -r $INFILE -R \"$output\" -w $FLOW_KEY.pcap\n");
  return;
}


init();
main();

exit(0);
