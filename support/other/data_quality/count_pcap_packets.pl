#!/usr/bin/perl

use strict;


sub getPacketCount($)
{
  my($file) = @_;

  my @lines  = ();
  my @raw    = ();
  $.         = 0;

  open(PH, "/usr/sbin/tcpdump -A -n -nn -r $file 2>&1 |") || die("Can't find tcpdump");
  while (<PH>)
  {
    if (/^\d{2,2}:\d{2,2}:\d{2,2}\.\d{6,6}\s+/)
    {
      push(@lines, $.);
    }
    push(@raw, $_);
  }
  close(PH);

  return(scalar(@lines));
}


sub usage($)
{
  my($message) = @_;

  print("\n  ERROR: $message\n");
  print("\n     Usage: $0 file\n");
  print("\n");

  exit(2);
}


if (@ARGV == 1)
{
  if (-f $ARGV[0])
  {
    print(getPacketCount($ARGV[0]) . "\n");
  }
  else
  {
    usage("File, $ARGV[1], not found.");
  }
}
else
{
  usage("Wrong number of arguments.");
}

exit(0);
