#!/usr/bin/perl -w

use strict;
use warnings;


# one output line for Flower
package Flow;


sub isIpv4
{
  my $self = shift;
  return($self->{'SRC_ADDR_V4'} ne '');
}


sub isIpv6
{
  my $self = shift;
  return($self->{'SRC_ADDR_V4'} eq '');
}


sub keylist
{
  my $self = shift;
  #my @elements;
  #foreach (sort(keys(%{$self})))
  #{
  #  push(@elements, $_);
  #}
  #return(@elements);
  return(getFieldList());
}


sub getFieldList
{
  my @fields = (
    'TIMET',
    'DURATION',
    '_VLAN_ID',
    '_PROTO',
    'SRC_ADDR_V4',
    'DST_ADDR_V4',
    'SRC_PAYLOAD',
    'DST_PAYLOAD',
    'SRC_BYTES',
    'DST_BYTES',
    'SRC_PACKETS',
    'DST_PACKETS',
    'SRC_IP_OPTS',
    'DST_IP_OPTS',
    '_SRC_PORT',
    '_DST_PORT',
    'SRC_TCP_OPTS',
    'DST_TCP_OPTS',
    'SRC_TCP_FLAGS',
    'DST_TCP_FLAGS',
    'TCP_EARLY_LATE_FLAG',
    'SRC_FIRST_TCP_SEQ',
    'DST_FIRST_TCP_SEQ',
    'SRC_LAST_TCP_SEQ',
    'DST_LAST_TCP_SEQ',
    'FRAGMENT_TYPE',
    '_FRAGMENT',
  );
  return(@fields);
}


sub getFlow
{
  my $self = shift;
  my @elements;
  my @fields = getFieldList();

  foreach (@fields)
  {
    if (exists($self->{$_}))
    {
      if ($self->{$_})
      {
        push(@elements, $self->{$_});
      }
      else
      {
        push(@elements, "");
      }
    }
    else
    {
      push(@elements, "");
    }
  }

  return(join(',', @elements));
}


sub getKey
{
  my $self = shift;
  return(
          '0' x (12 - length($self->{'SRC_ADDR_V4'})) . 
          $self->{'SRC_ADDR_V4'} . '_' .
          '0' x (12 - length($self->{'DST_ADDR_V4'})) . 
          $self->{'DST_ADDR_V4'} . '_' .
          $self->{'_SRC_PORT'}   . '_' .
          $self->{'_DST_PORT'}   . '_' .
          $self->{'_PROTO'}
        );
}


sub isFlower
{
  my $self = shift;
  return($self->{'_CLASS'} eq 'Flower');
}


sub isFlo
{
  my $self = shift;
  return($self->{'_CLASS'} eq 'Flo');
}


sub getClassName
{
  my $self = shift;
  return($self->{'_CLASS'});
}


sub getSrcBytes
{
  my $self = shift;
  return($self->{'SRC_BYTES'});
}


sub getDstBytes
{
  my $self = shift;
  return($self->{'DST_BYTES'});
}


sub getSrcPkts
{
  my $self = shift;
  return($self->{'SRC_PACKETS'});
}


sub getDstPkts
{
  my $self = shift;
  return($self->{'DST_PACKETS'});
}


sub getVlanId
{
  my $self = shift;
  return($self->{'_VLAN_ID'});
}


sub isVLAN
{
  my $self = shift;
  return($self->{'_VLAN_ID'} != 0);
}


sub isICMP
{
  my $self = shift;
  return($self->{'_PROTO'} == 1);
}


sub isTCP
{
  my $self = shift;
  return($self->{'_PROTO'} == 6);
}


sub isUDP
{
  my $self = shift;
  return($self->{'_PROTO'} == 17);
}


sub isOTHER
{
  my $self = shift;
  return(($self->{'_PROTO'} != 1) && ($self->{'_PROTO'} != 6) && ($self->{'_PROTO'} != 17));
}

1;
