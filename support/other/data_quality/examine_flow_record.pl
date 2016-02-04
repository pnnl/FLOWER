#!/usr/bin/perl

BEGIN
{
  use File::Basename;
  my $lib_dir = dirname($0);
  push(@INC, $lib_dir);
}

use Getopt::Long;
use FlowerUtil;


my $DEBUG        = 0;
my $HELP         = 0;
my $RECORD_NUM   = 0;
my $CONDITION    = '';
my $INFILE       = '';


sub init()
{
  Getopt::Long::Configure("bundling");
  GetOptions("d+" => \$main::DEBUG, "h+" => \$HELP, "c=s" => \$CONDITION, "i=s" => \$INFILE, "n=i" => \$RECORD_NUM);

  if ($HELP || ($INFILE eq ''))
  {
    usage();
    exit(0);
  }

  return;
}


init();

sub usage()
{
  print("\n");
  print("  Usage: $0 [dh] [n line_number_of_file] i input_file\n\n");
  print("    -d = Turn on debug output\n");
  print("    -h = Help\n");
  print("    -n = Process record at line number in input_file\n");
  print("    -i = Process files listed in the input_file\n");
  print("    -c = Use a condition to find records (e.g. -c 'TUNNEL_DEPTH -ge \"1\"')\n");
  print("\n");

  return;
}


sub main()
{
  open(FH, $INFILE) || die("ERROR: Failed to open file, $INFILE: $!\n");

  my $flower;

  while (<FH>)
  {
    next unless(/^0|^1/);   # Only process Data Records

    if (($RECORD_NUM == 0) || ($RECORD_NUM > 0 && $. == $RECORD_NUM))
    {
      $flower = new Flower(split(',', $_));
      $flower->printRecord($., $CONDITION);
    }
  }

  close(FH);
  return;
}


main();

exit(0);
