#!/usr/bin/perl -w

use strict;
use warnings;
use Flow;
use List::Util qw[min max];


# one output line for Flower
package Flower;

our @ISA = qw(Flow);

my $max_key_len = 5;
my @map;


sub max($$)
{
  my($a, $b) = @_;

  my $max = ($a > $b) ? $a : $b;

  return($max);
}


sub new
{
  my $class = $_[0];
  my $self  =
  {
    'FILE_OR_WIRE'        => ($_[1]  eq   '') ? '' : $_[1],
    'SITE'                => ($_[2]  eq   '') ? '' : $_[2],
    'TIMET'               => ($_[3]  eq   '') ? '' : $_[3],
    'DURATION'            => ($_[4]  eq   '') ? '' : $_[4],
    'VLAN_ID'             => ($_[5]  eq   '') ? '' : $_[5],
    'PROTO'               => ($_[6]  eq   '') ? '' : $_[6],
    'SRC_ADDR_V4'         => ($_[7]  eq   '') ? '' : $_[7],
    'DST_ADDR_V4'         => ($_[8]  eq   '') ? '' : $_[8],
    'SRC_ADDR_V6'         => ($_[9]  eq   '') ? '' : $_[9],
    'DST_ADDR_V6'         => ($_[10] eq   '') ? '' : $_[10],
    'SRC_PAYLOAD'         => ($_[11] eq   '') ? '' : $_[11],
    'DST_PAYLOAD'         => ($_[12] eq   '') ? '' : $_[12],
    'SRC_BYTES'           => ($_[13] eq   '') ? '' : $_[13],
    'DST_BYTES'           => ($_[14] eq   '') ? '' : $_[14],
    'SRC_PACKETS'         => ($_[15] eq   '') ? '' : $_[15],
    'DST_PACKETS'         => ($_[16] eq   '') ? '' : $_[16],
    'SRC_IP_OPTS'         => ($_[17] eq   '') ? '' : $_[17],
    'DST_IP_OPTS'         => ($_[18] eq   '') ? '' : $_[18],
    'SRC_PORT'            => ($_[19] eq   '') ? '' : $_[19],
    'DST_PORT'            => ($_[20] eq   '') ? '' : $_[20],
    'SRC_ICMP_FLAGS'      => ($_[21] eq   '') ? '' : $_[21],
    'DST_ICMP_FLAGS'      => ($_[22] eq   '') ? '' : $_[22],
    'ICMP_EARLY_LATE_FLG' => ($_[23] eq   '') ? '' : $_[23],
    'SRC_TCP_OPTS'        => ($_[24] eq   '') ? '' : $_[24],
    'DST_TCP_OPTS'        => ($_[25] eq   '') ? '' : $_[25],
    'SRC_TCP_FLAGS'       => ($_[26] eq   '') ? '' : $_[26],
    'DST_TCP_FLAGS'       => ($_[27] eq   '') ? '' : $_[27],
    'TCP_EARLY_LATE_FLAG' => ($_[28] eq   '') ? '' : $_[28],
    'SRC_FIRST_TCP_TS'    => ($_[29] eq   '') ? '' : $_[29],
    'SRC_FIRST_TCP_SEQ'   => ($_[30] eq   '') ? '' : $_[30],
    'DST_FIRST_TCP_TS'    => ($_[31] eq   '') ? '' : $_[31],
    'DST_FIRST_TCP_SEQ'   => ($_[32] eq   '') ? '' : $_[32],
    'SRC_LAST_TCP_TS'     => ($_[33] eq   '') ? '' : $_[33],
    'SRC_LAST_TCP_SEQ'    => ($_[34] eq   '') ? '' : $_[34],
    'DST_LAST_TCP_TS'     => ($_[35] eq   '') ? '' : $_[35],
    'DST_LAST_TCP_SEQ'    => ($_[36] eq   '') ? '' : $_[36],
    'SRC_TCP_RETRANS'     => ($_[37] eq   '') ? '' : $_[37],
    'DST_TCP_RETRANS'     => ($_[38] eq   '') ? '' : $_[38],
    'TUNNEL_DEPTH'        => ($_[39] eq   '') ? '' : $_[39],
    'TUNNEL_SADDR_V4'     => ($_[40] eq   '') ? '' : $_[40],
    'TUNNEL_DADDR_V4'     => ($_[41] eq   '') ? '' : $_[41],
    'TUNNEL_SADDR_V6'     => ($_[42] eq   '') ? '' : $_[42],
    'TUNNEL_DADDR_V6'     => ($_[43] eq   '') ? '' : $_[43],
    'TUNNEL_PROTO'        => ($_[44] eq   '') ? '' : $_[44],
    'TUNNEL_SPORT'        => ($_[45] eq   '') ? '' : $_[45],
    'TUNNEL_DPORT'        => ($_[46] eq   '') ? '' : $_[46],
    'FRAGMENT_TYPE'       => ($_[47] eq   '') ? '' : $_[47],
    'FRAGMENT'            => ($_[48] eq   '') ? '' : $_[48],
    'ANOMALY'             => ($_[49] eq "\n") ? '' : $_[49],
  };

  bless($self, $class);

  @map = ();

  push(@map, 'FILE_OR_WIRE');
  push(@map, 'SITE');
  push(@map, 'TIMET');
  push(@map, 'DURATION');
  push(@map, 'VLAN_ID');
  push(@map, 'PROTO');
  push(@map, 'SRC_ADDR_V4');
  push(@map, 'DST_ADDR_V4');
  push(@map, 'SRC_ADDR_V6');
  push(@map, 'DST_ADDR_V6');
  push(@map, 'SRC_PAYLOAD');
  push(@map, 'DST_PAYLOAD');
  push(@map, 'SRC_BYTES');
  push(@map, 'DST_BYTES');
  push(@map, 'SRC_PACKETS');
  push(@map, 'DST_PACKETS');
  push(@map, 'SRC_IP_OPTS');
  push(@map, 'DST_IP_OPTS');
  push(@map, 'SRC_PORT');
  push(@map, 'DST_PORT');
  push(@map, 'SRC_ICMP_FLAGS');
  push(@map, 'DST_ICMP_FLAGS');
  push(@map, 'ICMP_EARLY_LATE_FLG');
  push(@map, 'SRC_TCP_OPTS');
  push(@map, 'DST_TCP_OPTS');
  push(@map, 'SRC_TCP_FLAGS');
  push(@map, 'DST_TCP_FLAGS');
  push(@map, 'TCP_EARLY_LATE_FLAG');
  push(@map, 'SRC_FIRST_TCP_TS');
  push(@map, 'SRC_FIRST_TCP_SEQ');
  push(@map, 'DST_FIRST_TCP_TS');
  push(@map, 'DST_FIRST_TCP_SEQ');
  push(@map, 'SRC_LAST_TCP_TS');
  push(@map, 'SRC_LAST_TCP_SEQ');
  push(@map, 'DST_LAST_TCP_TS');
  push(@map, 'DST_LAST_TCP_SEQ');
  push(@map, 'SRC_TCP_RETRANS');
  push(@map, 'DST_TCP_RETRANS');
  push(@map, 'TUNNEL_DEPTH');
  push(@map, 'TUNNEL_SADDR_V4');
  push(@map, 'TUNNEL_DADDR_V4');
  push(@map, 'TUNNEL_SADDR_V6');
  push(@map, 'TUNNEL_DADDR_V6');
  push(@map, 'TUNNEL_PROTO');
  push(@map, 'TUNNEL_SPORT');
  push(@map, 'TUNNEL_DPORT');
  push(@map, 'FRAGMENT_TYPE');
  push(@map, 'FRAGMENT');
  push(@map, 'ANOMALY');

  return($self);
}


sub getKeys
{
  my $self = shift;
  my @elements;

  foreach (sort(keys(%{$self})))
  {
    push(@elements, $_);
  }

  return(@elements);
}


sub findMax
{
  my $self = shift;

  foreach (sort(keys(%{$self})))
  {
    $max_key_len = max($max_key_len, length($_));
  }

  return;
}


sub printRecord($$)
{
  my($self, $line_num, $condition) = @_;
  my $idx    = 0;
  my $result = 0;

  findMax($self);

  if ($condition ne '')
  {
    my($field, $operator, $value) = split(/\s+/, $condition);

    my $code = '$self->{' . $field . '}' . " $operator $value";

    $result  = eval($code);

    if ($@)
    {
      print("ERROR: Bad expression ($code), $@\n");
      exit(1);
    }

    if ($result == 0)
    {
      return;
    }
  }

  print("=" x 79 . "\n");
  print("Line Number: $line_num\n");
  print("=" x 79 . "\n");

  foreach (@map)
  {
    print(" " x (2 - length(++$idx)) . "$idx: $_ " . " " x ($max_key_len - length($_)) . " = " . $self->{$_} . "\n");
  }
  return;
}

1;
