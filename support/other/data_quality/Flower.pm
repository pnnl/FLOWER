#!/usr/bin/perl -w

use strict;
use warnings;
use Flow;


# one output line for Flower
package Flower;

our @ISA = qw(Flow);

sub new
{
  my $class = $_[0];
  my $self  =
  {
    '_FILE_OR_WIRE'       =>  $_[1],
   #'SITE'                =>  $_[2],
    'TIMET'               =>  $_[3],
   #'DTM'                 =>  $_[4],
    'DURATION'            =>  $_[4],
    '_VLAN_ID'            => ($_[5] eq '') ? 0 : $_[5],
    '_PROTO'              =>  $_[6],
    'SRC_ADDR_V4'         =>  $_[7],
    'DST_ADDR_V4'         =>  $_[8],
   #'SRC_ADDR_V6'         =>  $_[9],
   #'DST_ADDR_V6'         =>  $_[10],
    'SRC_PAYLOAD'         => ($_[11] eq '') ? 0 : $_[11],
    'DST_PAYLOAD'         => ($_[12] eq '') ? 0 : $_[12],
    'SRC_BYTES'           => ($_[13] eq '') ? 0 : $_[13],
    'DST_BYTES'           => ($_[14] eq '') ? 0 : $_[14],
    'SRC_PACKETS'         => ($_[15] eq '') ? 0 : $_[15],
    'DST_PACKETS'         => ($_[16] eq '') ? 0 : $_[16],
    'SRC_IP_OPTS'         => ($_[17] eq '') ? 0 : $_[17],
    'DST_IP_OPTS'         => ($_[18] eq '') ? 0 : $_[18],
    '_SRC_PORT'           => ($_[19] eq '') ? 0 : $_[19],
    '_DST_PORT'           => ($_[20] eq '') ? 0 : $_[20],
   #'SRC_ICMP_FLAGS'      =>  $_[21],
   #'DST_ICMP_FLAGS'      =>  $_[22],
   #'ICMP_EARLY_LATE_FLG' =>  $_[23],
    'SRC_TCP_OPTS'        => ($_[24] eq '') ? 0 : $_[24],
    'DST_TCP_OPTS'        => ($_[25] eq '') ? 0 : $_[25],
    'SRC_TCP_FLAGS'       => lc($_[26]),
    'DST_TCP_FLAGS'       => lc($_[27]),
    'TCP_EARLY_LATE_FLAG' => lc($_[28]),
    'SRC_FIRST_TCP_SEQ'   =>  $_[29],
    'DST_FIRST_TCP_SEQ'   =>  $_[30],
    'SRC_LAST_TCP_SEQ'    =>  $_[31],
    'DST_LAST_TCP_SEQ'    =>  $_[32],
    'SRC_TCP_RETRANS'     => ($_[33] eq '') ? 0 : $_[34],
    'DST_TCP_RETRANS'     => ($_[34] eq '') ? 0 : $_[34],
   #'TUNNEL_DEPTH'        =>  $_[35],
   #'TUNNEL_SADDR_V4'     =>  $_[36],
   #'TUNNEL_DADDR_V4'     =>  $_[37],
   #'TUNNEL_SADDR_V6'     =>  $_[38],
   #'TUNNEL_DADDR_V6'     =>  $_[39],
   #'TUNNEL_PROTO'        =>  $_[40],
   #'TUNNEL_SPORT'        =>  $_[41],
   #'TUNNEL_DPORT'        =>  $_[42],
    'FRAGMENT_TYPE'       =>  $_[43],
    '_FRAGMENT'           =>  $_[44],
    '_ANOMALY'            =>  $_[45],
    'FLOWKEY'             =>  '',
    '_CLASS'              =>  $class,
  };

  bless($self, $class);
  return($self);
}

1;
