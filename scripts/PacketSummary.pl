#!/usr/bin/perl

#  Parse all the FLOWER output files for the metrics records and sum them up.

use strict; # 'vars';
#use warnings;
use Carp;
use Data::Dumper qw(Dumper); $Data::Dumper::Sortkeys = 1;
use File::Basename;
use FileHandle;
use Getopt::Long qw(:config no_ignore_case);
use Term::ProgressBar;
use Time::Local;

my $HELP          =                     0;
my $VERBOSE       =                     0;
my $DATA_DIR      =        '/data/flower';
my $OUTPUT_FILE   =                    '';
my @COPY_ARGS     =                 @ARGV;
my @DATA_FILES;
my $YEAR          = 1900 + (localtime)[5];
my $MONTH         =                    '';
my $DAY           =                    '';
my $HOUR          =                    '';

sub usage($)
{
  my($message) = @_;

  print("\n$0 " . join(' ', @COPY_ARGS) . "\n") if (! $HELP);
  print("\n  Usage: $0 -d data_dir -o output_file\n\n");
  print("    -d = Data directory ($DATA_DIR)\n");
  print("    -o = Output file ($OUTPUT_FILE)\n");
  print("    -y = Year  ($YEAR)\n");
  print("    -m = Month ($MONTH)\n");
  print("    -D = Day   ($DAY)\n");
  print("    -H = Hour  ($HOUR)\n");
  print("    -h = Help\n");
  print("\n\n");
  if (0 != length($message))
  {
    croak("ERROR: $message\n");
  }
  exit(0);
}

GetOptions(
        'h+'      => \$HELP,
        'v+'      => \$VERBOSE,
        'd:s'     => \$DATA_DIR,
        'o:s'     => \$OUTPUT_FILE,
        'y:s'     => \$YEAR,
        'm:s'     => \$MONTH,
        'D:s'     => \$DAY,
        'H:s'     => \$HOUR,
)  or die(usage("Unable to parse command line arguments\n"));

sub commify {
  my $text = reverse $_[0];
  $text =~ s/(\d\d\d)(?=\d)(?!\d*\.)/$1,/g;
  return scalar reverse $text;
}

usage('') if (1 == $HELP);
my $OH;
if (0 != length($OUTPUT_FILE))
{
  open($OH, ">$OUTPUT_FILE") || usage("Unable to open file, $OUTPUT_FILE: $!");
}
else
{
  $OH = *STDOUT
}

opendir(DIR, $DATA_DIR) || usage("Unable to open directory, $DATA_DIR: $!");
@DATA_FILES = grep { /^$YEAR$MONTH$DAY$HOUR.*\.dat$/ } readdir(DIR);
closedir(DIR);

#
#  Individual File Count variables
#
my $LINE;
my $packet_captured;
my $processing_time;
my $packets_per_second;
my $bytes_received;
my $packets_received;
my $bad_eth_bytes;
my $bad_eth_pkts;
my $bad_ipv4_bytes;
my $bad_ipv4_pkts;
my $bad_ipv6_bytes;
my $bad_ipv6_pkts;
my $good_ipv4_bytes;
my $good_ipv4_pkts;
my $good_ipv6_bytes;
my $good_ipv6_pkts;
my $max_frag_flows;
my $max_norm_flows;
my $merged_flow_count;
my $summary_flow_count;

#
#  Total Count variables
#
my $tpacket_captured;
my $tprocessing_time;
my $tpackets_per_second;
my $tbytes_received;
my $tpackets_received;
my $tbad_eth_bytes;
my $tbad_eth_pkts;
my $tbad_ipv4_bytes;
my $tbad_ipv4_pkts;
my $tbad_ipv6_bytes;
my $tbad_ipv6_pkts;
my $tgood_ipv4_bytes;
my $tgood_ipv4_pkts;
my $tgood_ipv6_bytes;
my $tgood_ipv6_pkts;
my $tmax_frag_flows;
my $tmax_norm_flows;
my $tmerged_flow_count;
my $tsummary_flow_count;
my $tline_count;
my $line_count =  0;

my $count     = 0;
my $num_files = scalar(@DATA_FILES);
print "Files to process: $num_files\n";
#my $PROGRESS  = Term::ProgressBar->new({
#                                         name   => 'File Processor',
#                                         count  => $num_files,
#                                         fh     => \*STDERR,
#                                       });

$/ = "\n";
foreach my $FILE (sort(@DATA_FILES))
{
  my ($YYYY, $MM, $DD, $REST) = ($FILE =~ /(\d{4})(\d{2})(\d{2})(.*)/);
  #open(FH, $FILE) || croak("ERROR: Can't open file, $FILE: $!\n");
  #$line_count += tr/\n/\n/ while sysread(FH, $_, 2 ** 20);
  #close(FH);
#print ("FILE: $FILE\n");
  open(FH, "/usr/bin/tail -5 $FILE|") || croak("ERROR: Can't open file, $FILE: $!\n");
  while (<FH>)
  {
    next unless (/^3/);
    chomp();
    s/#//g;
    $LINE = $_;
#print ("LINE: $LINE\n");
    ($packet_captured)     = $LINE =~ /.*?:pc(\d+)/;
    $tpacket_captured     += $packet_captured;
    ($processing_time)     = $LINE =~ /.*?:ppt(.*?):pps/;
    $tprocessing_time     += $processing_time;
    ($packets_per_second)  = $LINE =~ /.*?:pps(\d+)/;
    $tpackets_per_second  += $packets_per_second;
    ($bytes_received)      = $LINE =~ /.*?:tbr(\d+)/;
    $tbytes_received      += $bytes_received;
    ($packets_received)    = $LINE =~ /.*?:tpr(\d+)/;
    $tpackets_received    += $packets_received;
    ($bad_eth_bytes)       = $LINE =~ /.*?:ebbc(\d+)/;
    $tbad_eth_bytes       += $bad_eth_bytes;
    ($bad_eth_pkts)        = $LINE =~ /.*?:ebpc(\d+)/;
    $tbad_eth_pkts        += $bad_eth_pkts;
    ($bad_ipv4_bytes)      = $LINE =~ /.*?:v4bbc(\d+)/;
    $tbad_ipv4_bytes      += $bad_ipv4_bytes;
    ($bad_ipv4_pkts)       = $LINE =~ /.*?:v4bpc(\d+)/;
    $tbad_ipv4_pkts       += $bad_ipv4_pkts;
    ($bad_ipv6_bytes)      = $LINE =~ /.*?:v6bbc(\d+)/;
    $tbad_ipv6_bytes      += $bad_ipv6_bytes;
    ($bad_ipv6_pkts)       = $LINE =~ /.*?:v6bpc(\d+)/;
    $tbad_ipv6_pkts       += $bad_ipv6_pkts;
    ($good_ipv4_bytes)     = $LINE =~ /.*?:v4gbc(\d+)/;
    $tgood_ipv4_bytes     += $good_ipv4_bytes;
    ($good_ipv4_pkts)      = $LINE =~ /.*?:v4gpc(\d+)/;
    $tgood_ipv4_pkts      += $good_ipv4_pkts;
    ($good_ipv6_bytes)     = $LINE =~ /.*?:v6gbc(\d+)/;
    $tgood_ipv6_bytes     += $good_ipv6_bytes;
    ($good_ipv6_pkts)      = $LINE =~ /.*?:v6gpc(\d+)/;
    $tgood_ipv6_pkts      += $good_ipv6_pkts;
    ($max_frag_flows)      = $LINE =~ /.*?:mff(\d+)/;
    $tmax_frag_flows      += $max_frag_flows;
    ($max_norm_flows)      = $LINE =~ /.*?:mnf(\d+)/;
    $tmax_norm_flows      += $max_norm_flows;
    ($merged_flow_count)   = $LINE =~ /.*?:mfc(\d+)/;
    $tmerged_flow_count   += $merged_flow_count;
    ($summary_flow_count)  = $LINE =~ /.*?:sfc(\d+)/;
    $tsummary_flow_count  += $summary_flow_count;
    last if (/^\s*0/);
  }
  close(FH);

  if (0 != $VERBOSE)
  {
    #print("FILE: $FILE $YYYY $MM $DD\n");
    #print("LINE: $LINE\n");
    print <<EOF;

      FILE:               $FILE $YYYY $MM $DD
      Packets captured:   $packet_captured
      Processing time:    $processing_time
      Packets per second: $packets_per_second
      Bytes received:     $bytes_received
      Packets received:   $packets_received
      Bad Eth Bytes:      $bad_eth_bytes
      Bad Eth Pkts:       $bad_eth_pkts
      Bad Ipv4 Bytes:     $bad_ipv4_bytes
      Bad Ipv4 Pkts:      $bad_ipv4_pkts
      Bad Ipv6 Bytes:     $bad_ipv6_bytes
      Bad Ipv6 Pkts:      $bad_ipv6_pkts
      Good Ipv4 Bytes:    $good_ipv4_bytes
      Good Ipv4 Pkts:     $good_ipv4_pkts
      Good Ipv6 Bytes:    $good_ipv6_bytes
      Good Ipv6 Pkts:     $good_ipv6_pkts
      Max Frag Flows:     $max_frag_flows
      Max Norm Flows:     $max_norm_flows
      Merged Flow Count:  $merged_flow_count
      Summary Flow Count: $summary_flow_count
      Line Count:         $line_count

EOF
  }

  ++$count;
  #last if ($count > 10);
  #$PROGRESS->update();

}
#$PROGRESS->update($num_files);
#print $OH "Finished\n";

#      Total Packets captured:   $tpacket_captured
#      Total Processing time:    $tprocessing_time
#      Total Packets per second: $tpackets_per_second

$tbytes_received     = commify($tbytes_received);
$tpackets_received   = commify($tpackets_received);
$tbad_eth_bytes      = commify($tbad_eth_bytes);
$tbad_eth_pkts       = commify($tbad_eth_pkts);
$tbad_ipv4_bytes     = commify($tbad_ipv4_bytes);
$tbad_ipv4_pkts      = commify($tbad_ipv4_pkts);
$tbad_ipv6_bytes     = commify($tbad_ipv6_bytes);
$tbad_ipv6_pkts      = commify($tbad_ipv6_pkts);
$tgood_ipv4_bytes    = commify($tgood_ipv4_bytes);
$tgood_ipv4_pkts     = commify($tgood_ipv4_pkts);
$tgood_ipv6_bytes    = commify($tgood_ipv6_bytes);
$tgood_ipv6_pkts     = commify($tgood_ipv6_pkts);
$tmax_frag_flows     = commify($tmax_frag_flows);
$tmax_norm_flows     = commify($tmax_norm_flows);
$tmerged_flow_count  = commify($tmerged_flow_count);
$tsummary_flow_count = commify($tsummary_flow_count);
$tline_count         = commify($tline_count);

  print $OH <<EOF;
  Total Bytes received:     $tbytes_received
  Total Packets received:   $tpackets_received
  Total Bad Eth Bytes:      $tbad_eth_bytes
  Total Bad Eth Pkts:       $tbad_eth_pkts
  Total Bad Ipv4 Bytes:     $tbad_ipv4_bytes
  Total Bad Ipv4 Pkts:      $tbad_ipv4_pkts
  Total Bad Ipv6 Bytes:     $tbad_ipv6_bytes
  Total Bad Ipv6 Pkts:      $tbad_ipv6_pkts
  Total Good Ipv4 Bytes:    $tgood_ipv4_bytes
  Total Good Ipv4 Pkts:     $tgood_ipv4_pkts
  Total Good Ipv6 Bytes:    $tgood_ipv6_bytes
  Total Good Ipv6 Pkts:     $tgood_ipv6_pkts
  Total Max Frag Flows:     $tmax_frag_flows
  Total Max Norm Flows:     $tmax_norm_flows
  Total Merged Flow Count:  $tmerged_flow_count
  Total Summary Flow Count: $tsummary_flow_count
  Total Line Count:         $tline_count
EOF
if (0 == length($OUTPUT_FILE))
{
  close($OH);
}

exit(0);
