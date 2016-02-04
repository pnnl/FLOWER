#!/usr/bin/perl -w

use Benchmark;
use File::Basename;
use File::Find qw(finddepth);
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

my $ENSIP_HOME  = $ENV{'ENSIP_HOME'} || "DOS";
my $TMP_DIR     = getTmpDir();
my $INFILE      = '';
my $DEBUG       = 0;


sub init()
{
  Getopt::Long::Configure("bundling");
  GetOptions ("d+" => \$main::DEBUG, "i=s" => \$INFILE);

  return;
}


sub getFiles()
{
  my @filenames;
  my @files;


  finddepth(
             sub {
                   if ($File::Find::name =~ m/\.stats$/)
                   {
                     push(@files, $File::Find::name);
#print("FILE:" . $File::Find::name . ":\n");
                   }
                   return;
                 },
             $TMP_DIR
           );

  foreach my $filename (@filenames)
  {
    if (-f $filename)
    {
      push(@files, $filename);
    }
    else
    {
      die("ERROR: File not found, $filename\n");
    }
  }

  return(@files);
}


sub main
{
  my $output;
  my %stats;
  my $total = 0;

  foreach my $filename (getFiles())
  {
    open(FH, $filename) || die("ERROR: Failed to open file, $filename: $!\n");
    while (<FH>)
    {
      next if (m/^\s*$|^\s*[#]/);
      chomp();
      my ($key, $value) = split(':', $_);
      $value =~ s/,//g;
      $stats{$key} += $value;
      if ($key =~ m/^Flower TOTAL FLOWS/)
      {
        $total += $value;
      }
    }
    close(FH);
    $stats{'0_FILES PROCESSED'}++;
  }

  foreach (sort(keys(%stats)))
  {
    if (/^__/)
    {
      my $key = $_;
      s/^__//;
      write_percent($_, $stats{$key}, $total);   
    }
    else
    {
      write_total($_, $stats{$_});   
    }
  }

  return;
}

init();
use bigint;
main();


exit(0);
