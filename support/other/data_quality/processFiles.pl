#!/usr/bin/perl -w

use Benchmark;
use File::Basename;
use File::Find qw(finddepth);
use FileManip;
use Flo;
use Flower;
use Getopt::Long;
use InitSetup;
use Validate;

use strict;
use warnings;

my %flowerStats = (
                    'numFiles'     => 0,
                    'rawFileBytes' => 0,
                    'percentIPv4'  => 0,
                    'percentIPv6'  => 0,
                    'packetsIPv4'  => 0,
                    'packetsIPv6'  => 0,
                    'bytesIPv4'    => 0,
                    'bytesIPv6'    => 0,
                    'payloadIPv4'  => 0,
                    'payloadIPv6'  => 0,
                    'srcHostsIPv4' => 0,
                    'srcHostsIPv6' => 0,
                    'dstHostsIPv4' => 0,
                    'dstHostsIPv6' => 0,
                    'flowsIPv4'    => 0,
                    'flowsIPv6'    => 0
                  );

my %OUTPUT_FILES;

my $DEBUG        = 0;
my $HELP         = 0;
my $SAFE         = 0;
my $INFILE       = '';
my $PROCESS      = 0;
my $VLAN         = 0;


sub init()
{
  Getopt::Long::Configure("bundling");
  GetOptions("d+" => \$main::DEBUG, "h+" => \$HELP, "p+" => \$PROCESS, "s+" => \$SAFE, "v+" => \$VLAN, "i=s" => \$INFILE);

  if ($HELP)
  {
    usage();
    exit(0);
  }
}

init();

my $FLOWER_HOME  = $ENV{'FLOWER_HOME'} || "DOS";
my($NUM)         = ($INFILE =~ m/.*?_(\d{2,2})/);
my $TMP_DIR      = getTmpDir() . '/' . $NUM;
my $OUT_DIR      = "$TMP_DIR/data";
my $LOG_DIR      = "$TMP_DIR/log";
my $STATS_DIR    = "$TMP_DIR/stats";
my $TMP_FILE_DIR = "$TMP_DIR/tmpfiles";

my $flo_cmd;
my $flower_cmd;

$flower_cmd .= "$FLOWER_HOME/bin/flower";
$flower_cmd .= ' --site-name=pnnl_dev';
$flower_cmd .= ' --cache-timeout=1000000';
$flower_cmd .= ' --cache-forceout=1000000';
$flower_cmd .= ' --summary-forceout=1000000';
$flower_cmd .= " --config-file=$FLOWER_HOME/support/other/data_quality/flower.conf";
$flower_cmd .= ' --output-file-ext=flower';
$flower_cmd .= " --output-data-dir=$TMP_FILE_DIR";
$flo_cmd    .= '/opt/cpp/sbin/flo';
$flo_cmd    .= ' -3';
$flo_cmd    .= ' --sfo=1000000';
$flo_cmd    .= ' --sit=1000000';
$flo_cmd    .= ' --site=pnl_dev';
$flo_cmd    .= ' --nosem';
$flo_cmd    .= " --data=$TMP_FILE_DIR";
$flo_cmd    .= ' -r';


sub make_path($;$)
{
  my ($inpath, $perms) = @_;
  my $path             = '';
  my @dirs             = split('/', $inpath);
  my $ok               = 1;
  my $dir;

  return(1)   if ( -e $inpath );

  $perms   = oct('0755')  if (! $perms);
  $dirs[0] = '/'          if ($dirs[0] eq '');

  foreach $dir (@dirs)
  {
    $path .= '/'   if ($path);
    $path .= $dir;
    if (! -e $path && ! (mkdir($path) && chmod($perms, $path)))
    {
      $ok  = 0;
      last;
    }
  }

  return($ok);
}


sub myMkDir($)
{
  my($dir) = @_;

  if (! -e $dir)
  {
    make_path($dir) || die("ERROR: Can't make directory, $dir: $!");
  }
  elsif (! -d $dir)
  {
    die("ERROR: Directory, $dir, already exists but it is not a directory");
  }

  return;
}


sub mkdirs()
{
  myMkDir($TMP_DIR);
  myMkDir($TMP_FILE_DIR);
  myMkDir($STATS_DIR);

  return;
}


sub usage()
{
  print("\n");
  print("  Usage: $0 [dhpsv] [i input_file]\n\n");
  print("    -d = Turn on debug output\n");
  print("    -h = Help\n");
  print("    -p = Process pcap files\n");
  print("    -s = Copy each pcap file to a temporary directory before processing\n");
  print("    -v = Process VLAN data\n");
  print("    -i = Process files listed in the input_file\n");
  print("\n");
}


sub findNewFiles()
{
  my @filenames;

  opendir(DH, $TMP_FILE_DIR) || die("ERROR: Failed to open directory, $TMP_FILE_DIR: $!");
  foreach my $file (readdir(DH))
  {
    # skip directories
    next if (-d $file);
    push(@filenames, "$TMP_FILE_DIR/$file");
  }
  closedir(DH);

  return(@filenames);
}


sub myMove($$)
{
  my($file1, $file2) = @_;

  if (move($file1, $file2))
  {
    delete($OUTPUT_FILES{basename($file1)});
    $OUTPUT_FILES{basename($file2)} += 1;
  }

  return;
}


sub printLog($$$)
{
  my($command, $file, $output) = @_;

  my $filename  = "$LOG_DIR/$file";
  $filename     =~ s/\.pcap$/./;
  $filename    .= $command;

  open(FH, ">$filename") || die("ERROR: Can't open log file, $filename: $!");
  print(FH $output);
  close(FH);

  return;
}


sub printStats($$$$$$)
{
  my($file, $flower_outfile, $flo_outfile, $flower_time, $flo_time, $output) = @_;

  my $filename        =  "$STATS_DIR/" . basename($file);
  $filename           =~ s/\.pcap$/.stats/;
  my($flower_cputime) =  ($flower_time =~ m/.*?\s+[=]\s+(\d+\.\d+)\s+CPU/);
  my($flo_cputime)    =  ($flo_time    =~ m/.*?\s+[=]\s+(\d+\.\d+)\s+CPU/);
  my($lock)           =  $filename . ".lock";

  open(FH, ">$lock") || die("ERROR: Can't open stats file, $filename: $!");
  print(FH "# Flower OutputFile:$flower_outfile\n");
  print(FH "# Flo    OutputFile:$flo_outfile\n");
  print(FH "# Flower Time:$flower_time\n");
  print(FH "# Flo    Time:$flo_time\n");
  print(FH "Flower Time:$flower_cputime\n");
  print(FH "Flo    Time:$flo_cputime\n");
  print(FH $output);
  close(FH);
  rename($lock, $filename);

  return;
}


sub processFilename($$$)
{
  my($cmd, $command, $pcapFile) = @_;
  my $file        = basename($pcapFile);
  my($prefix)     = ($file =~ m/^(.*\.)pcap$/);
  my $newFile     = '';
  my $output;

  print(STDOUT "Processing $cmd: $file\n");

  my $startMark = new Benchmark;
  my $result=`$command $pcapFile > /dev/null 2>&1`;
  my $endMark   = new Benchmark;

  my($outputFile) = findNewFiles();

  my $times = timestr(timediff($endMark, $startMark));
  print(STDOUT "  $cmd = $times\n");

  return($times, $outputFile);
}


sub getFiles()
{
  my @filenames;
  my @files;

  if ($INFILE)
  {
    open(FH, $INFILE) || die("ERROR: Failed to open file, $INFILE: $!\n");
    while (<FH>)
    {
      next if (m/^\s*$|^\s*[#]/);
      chomp();
      push(@filenames, $_);
    }
    close(FH);
  }
  else
  {
    die("ERROR: Missing input file, $INFILE\n");
  }

  foreach my $line (@filenames)
  {
    my($filename, $count) = split(':', $line);
    # skip directories
    next if (-d $filename);
    if ($filename =~ m/\.pcap$/)
    {
      if (-e $filename)
      {
        push(@files, $line);
      }
      else
      {
        die("ERROR: File not found, $filename\n");
      }
    }
    else
    {
      die("ERROR: File is not PCAP, $filename\n");
    }
  }

  return(@files);
}


sub main
{
  my $file;
  my $flower_outfile;
  my $flo_outfile;
  my $flower_time = 'N/A';
  my $flo_time    = 'N/A';
  my $output;

#my $idx = 0;            # While developing and debugging
#last if ($idx++ >= 1);  # While developing and debugging

  foreach my $line (getFiles())
  {
    my($pcapFile, $count) = split(':', $line);
    $output = '';

    if ($PROCESS)
    {
      print(STDOUT "\n\nProcessing file: " . basename($pcapFile) . " at " . scalar(localtime(time())) . "\n");
      #my $tmp_cmd = $flower_cmd . " --max-flowcache-size=" . $count;
      my $tmp_cmd = $flower_cmd . " --max-flowcache-size=" . "200000";
print("CMD:$tmp_cmd\n");
print("CMD:$flo_cmd\n");
      ($flower_time, $flower_outfile) = processFilename("Flower", $tmp_cmd, $pcapFile);
      ($flo_time,    $flo_outfile)    = processFilename("Flo",    $flo_cmd, $pcapFile);
    }

    $output = validateFiles($flower_outfile, $flo_outfile, $pcapFile);
    printStats($pcapFile, $flower_outfile, $flo_outfile, $flower_time, $flo_time, $output);
    unlink($flower_outfile) || warn("Can't unlink $flower_outfile: $!");
    unlink($flo_outfile)    || warn("Can't unlink $flo_outfile: $!");
  }
  print(STDOUT "-----------------------------------------------------------------------\n");
  print(STDOUT "Warnings\n");
  print(STDOUT getWarnings());
  print(STDOUT "-----------------------------------------------------------------------\n");
  print(STDOUT "Exceptions\n");
  print(STDOUT getExampleExceptions());

  return;
}


mkdirs();
use bigint;
main();


exit(0);
