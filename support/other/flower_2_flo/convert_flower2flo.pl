#!/usr/bin/perl

# Purpose/Motivation
#
#    Flo does not handle GRE tunnels
#    CRISP wants to read in FLOWER data and write out FLO-like data

#    Steps:
#      1. Read in FLOWER data file
#      2. Parse FLOWER data and produce FLO data
#      3. Write FLO data as output

#    Process:
#       See mapping of fields in Flower2Flo

BEGIN
{
  use File::Basename;
  my $lib_dir = dirname($0);
  push(@INC, $lib_dir);
}

use Getopt::Long;
use Flower2Flo;

my $HELP         = 0;
my $RECORD_NUM   = 0;
my $INFILE       = '';
my $OUTFILE      = '';

sub init()
{
  Getopt::Long::Configure("bundling");
  GetOptions("h+"  => \$HELP,
             "i=s" => \$INFILE,
             "n=i" => \$RECORD_NUM,
             "o=s" => \$OUTFILE
            );

  if ($HELP || ($INFILE eq '') || ($OUTFILE eq ''))
  {
    usage();
    exit(0);
  }

  return;
}

sub usage()
{
  print("\n");
  print("  Usage: $0 [h] [n line_number_of_file] -i input_file -o output_file\n\n");
  print("    -h = Help\n");
  print("    -n = Process record at line number in input_file\n");
  print("    -i = Process FLOWER data in the input_file\n");
  print("    -o = Create flo data in the output_file\n");
  print("\n");

  return;
}

sub main()
{
  open(FHI,   $INFILE)   || die("ERROR: Failed to open input  file, $INFILE:  $!\n");
  open(FHO, ">$OUTFILE") || die("ERROR: Failed to open output file, $OUTFILE: $!\n");

  while (<FHI>)
  {
    next unless(/^0|^1/);   # Only process Data Records
    if (($RECORD_NUM == 0) || ($RECORD_NUM > 0 && $. == $RECORD_NUM))
    {
      my $record = new Flower2Flo(split(',', $_));
      print(FHO $record->convert2Flo($.));
    }
  }

  close(FHO);
  close(FHI);
  return;
}

init();
main();

exit(0);