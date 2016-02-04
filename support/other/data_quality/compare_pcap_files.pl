#!/usr/bin/perl

use strict;


my $max_packets = 30000;


sub getDump($$)
{
  my($file, $num_records_from_end) = @_;

  my $tshark   = "/usr/sbin/tshark";
  my $pattern  = "";
  my $tversion = "";
  my $result   = "";
  my @lines    = ();
  my @raw      = ();
  $.           = 0;

  open(PH, "$tshark -v |") || die("Can't open pipe to $tshark: $!");
  while (<PH>)
  {
    if (/^TShark\s+\d+\./)
    {
      ($tversion) = m/^TShark\s+(\d+)\./;
      last;
    }
  }
  close(PH);

  if ($tversion >= 1)
  {
    $pattern = '^\s*\d+\s+\d+.*?\.\d{6,6}\s+';
  }
  else
  {
    $pattern = '^\s*\d+\s+\d+\.\d{6,6}\s+';
  }

  #my $tcp_dump_pattern = '^.*?\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}.*';
  #open(PH, "/usr/sbin/tcpdump -A -n -nn -r $file 2>&1 |") || die("Can't open pipe to tshark: $!");

  open(PH, "$tshark -x -r $file 2>&1 |") || die("Can't open pipe to $tshark: $!");
  while (<PH>)
  {
    if (/$pattern/)
    {
      push(@lines, $.);
    }
    push(@raw, $_);
  }
  close(PH);

  print("RECORDS:" . scalar(@lines) . ", FILE: $file\n");

  if ($num_records_from_end > scalar(@lines))
  { 
    $num_records_from_end = scalar(@lines); 
  }

  my @reverse = reverse(@lines);
  my $stop = scalar(@raw) - $reverse[$num_records_from_end - 1] + 1;
  my $idx  = 0;
  foreach (reverse(@raw))
  {
    if ($_ !~ /^reading from/)
    {
      my $line = $_;
      if (/$pattern/)
      {
        next;
      }
      $result .= $line;
    }
    $idx++;
    last if ($idx >= $stop);
  }

  return($result);
}


sub compare($$$)
{
  my($num_records, $first, $second) = @_;

  my $result1 = getDump($first,  $num_records);
  my $result2 = getDump($second, $num_records);
  if ((length($result1) > 0) && ($result1 eq $result2))
  {
    print("SAME\n");
    exit(0);
  }
  else
  {
    open(ONE, ">/var/tmp/out1") || die("Can't open /var/tmp/out1: $!");
    open(TWO, ">/var/tmp/out2") || die("Can't open /var/tmp/out2: $!");
    print(ONE $result1);
    print(TWO $result2);
    close(ONE);
    close(TWO);
    print("DIFFER - diff /var/tmp/out1 /var/tmp/out2\n");
    exit(1);
  }

  return;
}


sub usage($)
{
  my($message) = @_;

  print("\n  ERROR: $message\n");
  print("\n     Usage: $0 nr file1 file2\n");
  print("\n  Where: nr = number of pcap records at the end of each file to compare\n\n");

  exit(2);
}


if (@ARGV == 3)
{
  if (! ($ARGV[0] > 0 && $ARGV[0] <= $max_packets))
  {
    usage("Number of records to compare must be between 0 and $max_packets.");
  }

  if (-f $ARGV[1] && -f $ARGV[2])
  {
    compare($ARGV[0], $ARGV[1], $ARGV[2]);
  }
  else
  {
    if (! -f $ARGV[1])
    {
      usage("File, $ARGV[1], not found.");
    }
    if (! -f $ARGV[2])
    {
      usage("File, $ARGV[2], not found.");
    }
  }
}
else
{
  usage("Wrong number of arguments.");
}

exit(3);
