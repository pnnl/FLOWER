#!/usr/bin/perl -w

use strict;
use warnings;
use 5.010;

#  Represents one line from Flower
package Flower2Flo;

sub new
{
  my $class = $_[0];
  my $self  =
  {
    '_FILE_OR_WIRE'       =>  $_[1],
    'SITE'                =>  $_[2],
    'TIMET'               =>  $_[3],
    'DTM'                 =>  $_[4],
    'DURATION'            =>  $_[4],
    '_VLAN_ID'            => ($_[5] eq '') ? 0 : $_[5],
    '_PROTO'              =>  $_[6],
    'SRC_ADDR_V4'         =>  $_[7],
    'DST_ADDR_V4'         =>  $_[8],
    'SRC_ADDR_V6'         =>  $_[9],
    'DST_ADDR_V6'         =>  $_[10],
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
    'SRC_TCP_OPTS'        => ($_[24] eq '') ? 0 : $_[24],
    'DST_TCP_OPTS'        => ($_[25] eq '') ? 0 : $_[25],
    'SRC_TCP_FLAGS'       => lc(($_[26] eq '') ? '00' : $_[26]),
    'DST_TCP_FLAGS'       => lc(($_[27] eq '') ? '00' : $_[27]),
    'TCP_EARLY_LATE_FLAG' => lc($_[28]),
    'SRC_FIRST_TCP_SEQ'   => ($_[29] eq '') ? 0 : $_[29],
    'DST_FIRST_TCP_SEQ'   => ($_[30] eq '') ? 0 : $_[30],
    'SRC_LAST_TCP_SEQ'    => ($_[31] eq '') ? 0 : $_[31],
    'DST_LAST_TCP_SEQ'    => ($_[32] eq '') ? 0 : $_[32],
    'SRC_TCP_RETRANS'     => ($_[33] eq '') ? 0 : $_[33],
    'DST_TCP_RETRANS'     => ($_[34] eq '') ? 0 : $_[34],
    '_FRAGMENT_TYPE'      =>  $_[40],
    'MORE_FRAG'           =>  0,
    'CONT_FRAG'           =>  0,
    'FORCE'               =>  0,
    '_CLASS'              =>  $class,
  };

  # Map F to 1,0; Map N to 0,1; Map L to 0,0?; Map A to 0,0 and update flo field 33 to 1
  if ('A' eq $self->{'_FRAGMENT_TYPE'} && 1 == $self->{'_FILE_OR_WIRE'})
  {
     #$self->{'FORCE'} = 1;
  }
  if ('F' eq $self->{'_FRAGMENT_TYPE'})
  {
     $self->{'MORE_FRAG'} = 1;
  }
  if ('N' eq $self->{'_FRAGMENT_TYPE'})
  {
     $self->{'CONT_FRAG'} = 1;
  }
  if (1 == $self->{'_PROTO'})
  {
    $self->{'SRC_PAYLOAD'} = 0;
  }

  bless($self, $class);
  return($self);
}

sub make_ipv4_addr($) {
  my($self, $ip) = @_;
  my($oct1, $oct2, $oct3, $oct4) = split('.', $ip);
  return('');
}

sub make_cpp_ipv4_addr($) {
  my($self, $ip) = @_;
  my $output;

  $output  = 0 x (12 - length($ip)) . $ip;
  my($oct1, $oct2, $oct3, $oct4) = $output =~ m/^(\d\d\d)(\d\d\d)(\d\d\d)(\d\d\d)$/;
  $oct1 =~ s/^0{1,2}//;
  $oct2 =~ s/^0{1,2}//;
  $oct3 =~ s/^0{1,2}//;
  $oct4 =~ s/^0{1,2}//;
  
  $output .= ',' . $oct1;
  $output .= ',' . $oct2;
  $output .= ',' . $oct3;
  $output .= ',' . $oct4;

  return($output);
}

sub convert2Flo($) {
  my($self, $line_num, $condition) = @_;

  #   Process to post-process FLOWER data to produce a facsimile of flo30 data:
  #       Ignore entire record if FLOWER field 9 or 10 exist (IPv6 only data or IPv6 is inner most tunnel data)
  #       Ignore FLOWER field 1 (wire vs file)
  #       Move FLOWER field 3 to flo field 1
  #       Take FLOWER field3 and convert to human readable form (YYYYMMDDHHmmss.uuuuuu) and make it flo field 2
  #       Move FLOWER field 2 to flo field 3
  #       Make flo field 4 be "flo30"
  #       Ignore FLOWER field 5 (vlan)
  #       Move FLOWER field 6 to flo field 5
  #       Modify FLOWER field 7 to prepend leading 0s if necessary
  #       Move FLOWER field 7 to flo field 6
  #       Use FLOWER field 7 to create flo fields 7, 8, 9, and 10
  #       Use a blank field for flo field 11 (first seen source country code)
  #       Modify FLOWER field 8 to prepend leading 0s if necessary
  #       Move FLOWER field 8 to flo field 12
  #       Use FLOWER field 8 to create flo fields 13, 14, 15, and 16
  #       Ignore FLOWER fields 9 and 10 (IPv6)
  #       Use a blank field for flo field 17 (first seen dest country code)
  #       Move FLOWER field 19 to flo field 18
  #       Move FLOWER field 20 to flo field 19
  #       Modify FLOWER field 40 to create values for flo fields 20 and 21 (Map 'F' to 1,0; Map 'N' to 0,1; Map 'L' to 0,0?; Map 'A' to 0,0)
  #       Move FLOWER field 4 to flo field 22
  #       Move FLOWER field 11 to flo field 23 (if FLOWER field is empty use "0")
  #       Move FLOWER field 12 to flo field 24 (if FLOWER field is empty use "0")
  #       Move FLOWER field 13 to flo field 25 (if FLOWER field is empty use "0")
  #       Move FLOWER field 14 to flo field 26 (if FLOWER field is empty use "0")
  #       Move FLOWER field 15 to flo field 27 (if FLOWER field is empty use "0")
  #       Move FLOWER field 16 to flo field 28 (if FLOWER field is empty use "0")
  #       Move FLOWER field 17 to flo field 29 (if FLOWER field is empty use "0")
  #       Move FLOWER field 18 to flo field 30 (if FLOWER field is empty use "0")
  #       Ignore FLOWER fields 21, 22, and 23 (ICMP)
  #       Move FLOWER field 24 to flo field 31 (if FLOWER field is empty use "0")
  #       Move FLOWER field 25 to flo field 32 (if FLOWER field is empty use "0")
  #       Modify FLOWER field 40 to create values for flo field 33 (If value is 'A' set to 1)
  #       Copy FLOWER field 13 to flo field 34 (if FLOWER field is empty use "0")
  #       Copy FLOWER field 14 to flo field 35 (if FLOWER field is empty use "0")
  #       Copy FLOWER field 13 to flo field 36 (if FLOWER field is empty use "0")
  #       Copy FLOWER field 14 to flo field 37 (if FLOWER field is empty use "0")
  #       Use a blank field for flo field 38 (first seen source site)
  #       Use a blank field for flo field 39 (first seen dest site)
  #       Move FLOWER field 26 to flo field 40 (if FLOWER field is empty use "00")
  #       Move FLOWER field 27 to flo field 41 (if FLOWER field is empty use "00")
  #       Move FLOWER field 28 to flo field 42
  #       Move FLOWER field 29 to flo field 43 (if FLOWER field is empty use "0")
  #       Move FLOWER field 30 to flo field 44 (if FLOWER field is empty use "0")
  #       Move FLOWER field 31 to flo field 45 (if FLOWER field is empty use "0")
  #       Move FLOWER field 32 to flo field 46 (if FLOWER field is empty use "0")
  #       Ignore FLOWER fields 33, 34, 35, 36, 37, 38, 39 (FLOWER tunnel wrapper)
  #       Ignore FLOWER field 4

  my $record;
  my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = gmtime($self->{'TIMET'});
  $mon++;
  $year += 1900;
  
  return '' if ($self->{'SRC_ADDR_V6'} || $self->{'DST_ADDR_V6'});
  return '' if ($self->{'SRC_ADDR_V4'} =~ /^\s*$/ || $self->{'DST_ADDR_V4'} =~ /^\s*$/);

  $record .=       $self->{'TIMET'};
  $record .= ',' . sprintf("%04d%02d%02d%02d%02d%02d.", $year, $mon, $mday, $hour, $min, $sec) . (split /\./, $self->{'TIMET'})[1];
  $record .= ',' . $self->{'SITE'} . ','        . 'flo30'                        . ',' . $self->{'_PROTO'};
  $record .= ',' . $self->make_cpp_ipv4_addr($self->{'SRC_ADDR_V4'}) . ',';
  $record .= ',' . $self->make_cpp_ipv4_addr($self->{'DST_ADDR_V4'}) . ',';
  $record .= ',' . $self->{'_SRC_PORT'}         . ',' . $self->{'_DST_PORT'};
  $record .= ',' . $self->{'MORE_FRAG'}         . ',' . $self->{'CONT_FRAG'}     . ',' . $self->{'DURATION'};
  $record .= ',' . $self->{'SRC_PAYLOAD'}       . ',' . $self->{'DST_PAYLOAD'};
  $record .= ',' . $self->{'SRC_BYTES'}         . ',' . $self->{'DST_BYTES'};
  $record .= ',' . $self->{'SRC_PACKETS'}       . ',' . $self->{'DST_PACKETS'};
  $record .= ',' . $self->{'SRC_IP_OPTS'}       . ',' . $self->{'DST_IP_OPTS'};
  $record .= ',' . $self->{'SRC_TCP_OPTS'}      . ',' . $self->{'DST_TCP_OPTS'}  . ',' . $self->{'FORCE'};
  $record .= ',' . $self->{'SRC_BYTES'}         . ',' . $self->{'DST_BYTES'};
  $record .= ',' . $self->{'SRC_BYTES'}         . ',' . $self->{'DST_BYTES'}     . ',,';
  $record .= ',' . $self->{'SRC_TCP_FLAGS'}     . ',' . $self->{'DST_TCP_FLAGS'} . ',' . $self->{'TCP_EARLY_LATE_FLAG'};
  $record .= ',' . $self->{'SRC_FIRST_TCP_SEQ'} . ',' . $self->{'DST_FIRST_TCP_SEQ'};
  $record .= ',' . $self->{'SRC_LAST_TCP_SEQ'}  . ',' . $self->{'DST_LAST_TCP_SEQ'}    . "\n";

  return($record);
}
1;
