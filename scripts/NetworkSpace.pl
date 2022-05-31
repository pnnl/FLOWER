#!/usr/bin/perl

#  Crack open all the files that match the pattern and sort them by ip address or most used

use strict; # 'vars';
use Carp;
use Data::Dumper qw(Dumper); $Data::Dumper::Sortkeys = 1;
use File::Basename;
use FileHandle;
use Getopt::Long qw(:config no_ignore_case);
use Socket qw( inet_aton );

my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime();
$year = $year+1900;
$mon++;  #  Increment month by 1 because it is 0 based

my $HELP          =                                            0;
my $VERBOSE       =                                            0;
my $DATA_DIR      =                               '/data/flower';
my $OUTPUT_FILE   =                                           '';
my $LEVEL         =                                          'C';
my @COPY_ARGS     =                                        @ARGV;
my $MAX_OUTPUT    =                                           10;
my $SORT_BY_IP    =                                            0;
my $FILE_PATTERN  =  sprintf('%04s%02s%02s', $year, $mon, $mday);
my @DATA_FILES;


GetOptions(
        'h+'      => \$HELP,
        'v+'      => \$VERBOSE,
        'd:s'     => \$DATA_DIR,
        'l:s'     => \$LEVEL,
        'o:s'     => \$OUTPUT_FILE,
        'm:s'     => \$MAX_OUTPUT,
        'p:s'     => \$FILE_PATTERN,
        's:s'     => \$SORT_BY_IP,
);


sub usage($)
{
  my($message) = @_;

  print("\n$0 " . join(' ', @COPY_ARGS) . "\n") if (! $HELP);
  print("\n  Usage: $0 -d data_dir -o output_file\n\n");
  print("    -d = Data directory ($DATA_DIR)\n");
  print("    -o = Output file ($OUTPUT_FILE)\n");
  print("    -l = Level B or C ($LEVEL)\n");
  print("    -m = Maximum number of lines to print ($MAX_OUTPUT) NOTE: 0 = all\n");
  print("    -p = File pattern ($FILE_PATTERN)\n");
  print("    -s = Sort by IP or Counts ($SORT_BY_IP) NOTE: 0 = Counts, 1 = IP\n");
  print("    -h = Help\n");
  print("\n\n");
  if (0 != length($message))
  {
    croak("ERROR: $message\n");
  }
  exit(0);
}

sub compare
{
  my($a1,$a2,$a3) = split(',', $a);
  my($b1,$b2,$b3) = split(',', $b);

  if ($a1 <  $b1) { return -1; }
  if ($a1 == $b1)
  {
    if ($a2 <  $b2) { return -1; }
    if ($a2 == $b2)
    {
      if ($a3 <  $b3) { return -1; }
      if ($a3 == $b3) { return  0; }
    }
  }
  return 1;
}

sub commify
{
  my $text = reverse $_[0];
  $text =~ s/(\d\d\d)(?=\d)(?!\d*\.)/$1,/g;
  return scalar reverse $text;
}

usage('') if (1 == $HELP);
usage('') if ("$LEVEL" ne "B" && "$LEVEL" ne "C");

my $OH;
if (0 != length($OUTPUT_FILE))
{
  open($OH, ">$OUTPUT_FILE") || usage("ERROR: Unable to open file, $OUTPUT_FILE: $!");
}
else
{
  $OH = *STDOUT
}

opendir(DIR, $DATA_DIR) || usage("ERROR: Unable to open directory, $DATA_DIR: $!");
my @DATA_FILES = grep { /^$FILE_PATTERN/ } readdir(DIR);
closedir(DIR);

print("@DATA_FILES\n");

my $count;
$count     = 100000;
$count     = 0;
my $num_files = scalar(@DATA_FILES);
print "Files to process: $num_files\n";

my $bkey;
my $ckey;
my($oct1,$oct2,$oct3,$oct4);
my($saddr,$daddr,$tsaddr,$tdaddr);
my @fields;
my @indices = (6, 7, 39, 40);
my %ipSpace;
my $dkey       = \$bkey;

if ('C' eq $LEVEL)
{
  $dkey        = \$ckey;
}

foreach my $FILE (sort(@DATA_FILES))
{
  open(FH, $FILE) || croak("ERROR: Can't open file, $FILE: $!\n");
  while (<FH>)
  {
    next unless (/^0/);
    chomp();
    @fields = split(',', $_);
    $saddr  = $fields[6];
    $daddr  = $fields[7];
    $tsaddr = $fields[39];
    $tdaddr = $fields[40];
    foreach my $idx (@indices)
    {
      ($oct1,$oct2,$oct3,$oct4) = split(/\./, $fields[$idx]);
      next unless ($oct1 =~ /\d+/);
      $bkey = "$oct1.$oct2";
      $ckey = "$oct1.$oct2.$oct3";
      $ipSpace{$LEVEL}{$$dkey}++;
    }
    #last if (($count > 0 && $. > $count));
  }
  close(FH);

  #last;
}

my $count = 0;
my @keys;

if ($SORT_BY_IP)
{
  @keys = sort compare keys %{$ipSpace{$LEVEL}};
}
else
{
  @keys = sort { $ipSpace{$LEVEL}{$b} <=> $ipSpace{$LEVEL}{$a} } %{$ipSpace{$LEVEL}};
}

foreach my $key (@keys) {
  if ($MAX_OUTPUT)
  {
    printf($OH "%11s = %s\n", $key, commify($ipSpace{$LEVEL}{$key}));
    $count++;
    last if ($count >= $MAX_OUTPUT)
  }
}


#print $OH <<EOF;
#EOF
if (0 == length($OUTPUT_FILE))
{
  close($OH);
}

exit(0);
