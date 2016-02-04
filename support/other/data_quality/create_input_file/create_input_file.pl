#!/usr/bin/perl

my $junk1;
my $junk2;
my $file;
my $flows;
my @rest;
my %dist;

my $line = 0;

open(FH, "output.txt");
while (<FH>)
{
  chomp();
  if (/^Processing file/)
  {
    ($junk1, $junk2, $file, @rest) = split(/\s+/, $_);
  }
  if (/^Flower TOTAL FLOWS/)
  {
    s/,//;
    ($junk1, $flows) = split(/\s+:\s+/, $_);
    $flows += 10000;
    print("/var/sc08data/bulk/$file:$flows\n");
    $line++;
    if ($ARGV[0] && $line >= $ARGV[0])
    {
      print("GREAT\n");
      exit(0);
    }
  }
}
close(FH);
