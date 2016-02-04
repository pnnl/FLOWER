#!/usr/bin/perl -w

use strict;
use warnings;
use Flow;


# one output line for Flo
package Flo;

our @ISA = qw(Flow);

sub new
{
  my $class = $_[0];
  my $self  =
  {
    'TIMET'                   =>  $_[1],                      # timet
   #'DTM'                     =>  $_[2],                      # dtm
    'DURATION'                =>  $_[22],                     # dur
   #'SITE'                    =>  $_[3],                      # site
    '_VLAN_ID'                =>  0,                          # Not in Flo
    '_PROTO'                  =>  $_[5],                      # proto
    'SRC_ADDR_V4'             =>  $_[6],                      # fssaddr
    'DST_ADDR_V4'             =>  $_[12],                     # fsdaddr
    'SRC_PAYLOAD'             => ($_[23] eq '') ? 0 : $_[23], # fssdata
    'DST_PAYLOAD'             => ($_[24] eq '') ? 0 : $_[24], # fsddata
    'SRC_BYTES'               => ($_[25] eq '') ? 0 : $_[25], # fssbyte
    'DST_BYTES'               => ($_[26] eq '') ? 0 : $_[26], # fsdbyte
    'SRC_PACKETS'             => ($_[27] eq '') ? 0 : $_[27], # fsspkt
    'DST_PACKETS'             => ($_[28] eq '') ? 0 : $_[28], # fsdpkt
    'SRC_IP_OPTS'             => ($_[29] eq '') ? 0 : $_[29], # fssiopt
    'DST_IP_OPTS'             => ($_[30] eq '') ? 0 : $_[30], # fsdiopt
    '_SRC_PORT'               =>  $_[18],                     # fssport
    '_DST_PORT'               =>  $_[19],                     # fsdport
    'SRC_TCP_OPTS'            => ($_[31] eq '') ? 0 : $_[31], # fsstopt
    'DST_TCP_OPTS'            => ($_[32] eq '') ? 0 : $_[32], # fsdtopt
    'SRC_TCP_FLAGS'           =>  $_[40],                     # fssflags
    'DST_TCP_FLAGS'           =>  $_[41],                     # fsdflags
    'TCP_EARLY_LATE_FLAG'     =>  $_[42],                     # flags
    'SRC_FIRST_TCP_SEQ'       =>  $_[43],                     # fssfseq
    'DST_FIRST_TCP_SEQ'       =>  $_[44],                     # fsdfseq
    'SRC_LAST_TCP_SEQ'        =>  $_[45],                     # fsslseq
    'DST_LAST_TCP_SEQ'        =>  $_[46],                     # fsdseq
    'FRAGMENT_TYPE'           =>  0,                          # interpreted from cf,mf,and force
    '_FRAGMENT'               =>  0,
    'FLOWKEY'                 =>  '',
    '_CLASS'                  =>  $class,
  };

  my $mf                      =   $_[20];                     # part of FRAGMENT_TYPE
  my $cf                      =   $_[21];                     # part of FRAGMENT_TYPE
  my $force                   =   $_[33];                     # part of FRAGMENT_TYPE
  my $position                =  ($mf << 2) + ($cf << 1) + $force;
  my @flags                   =  ("C", "A", "L", "A", "F", "A", "N", "A");
  $self->{'FRAGMENT_TYPE'}    =   $flags[$position];
  $self->{'DST_LAST_TCP_SEQ'} =~  s/\s+$//;

  bless($self, $class);
  return($self);
}


1;
