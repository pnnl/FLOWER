#!/usr/bin/perl -w

package Validate;

use strict;
use warnings;
use vars qw(@ISA @EXPORT $gLabel $gValue $gPercent $gCount $gProtocol $gFilename $gPcapFilename %gExampleExceptions %gExceptions %gExceptionCnt $gFlowkey $gWarning);
require Exporter;
@ISA    = ("Exporter");
@EXPORT = qw(getExampleExceptions getWarnings validateFiles write_percent write_total);


sub getFilename()
{
  return($gFilename);
}


sub getPcapFilename()
{
  return($gPcapFilename);
}


sub getFileLines($)
{
  my($file) = @_;
  my @lines;
  $gFilename = $file;

  open(FH, $file) || warn("Can't open file, $file: $!\n");
  while (<FH>)
  {
    chomp();
    next if (/^[2-9]\,/); # Skip Flower metadata records
    if (/\,$/)
    {
      # Need to add a '0' to Flower records ending in ',' so that
      # getFlowerFieldCount() will return the correct value.
      # Perl treats a split count differently if the line ends
      # in the delimiter. For example "a,b,c" and "a,b,c," both
      # return the same number.
      $_ .= "0";
    }
    push(@lines, $_);
  }
  close(FH);
  return(@lines);
}


sub getFloFieldCount()
{
  return(45);
}


sub getFlowerFieldCount()
{
  return(42);
}


sub isMatch($$)
{
  my ($one,$two) = @_;

  if (! defined($one) && ! defined($two))
  {
    return(1);
  }

  if (! defined($one))
  {
    $one = 0;
  }

  if (! defined($two))
  {
    $two = 0;
  }

  $one =~ s/^[0]+$//g;
  $two =~ s/^[0]+$//g;

  #if (! $one && ! $two)
  #{
  #  return(1);
  #}

  if ("$one" eq "$two")
  {
    return(1);
  }

  return(0);
}


sub buildIpv4Summary($@)
{
  my ($object, @Lines) = @_;
  my %Summary;
  my $Obj;
  my $key;

  foreach (@Lines)
  {
    my @test1 = split(',', $_);
    if ($object eq "Flower")
    {
      if ($#test1 == getFlowerFieldCount())
      {
        $Obj = new Flower(@test1);
        # Only store IPv4 data to compare with Flo
        if ($Obj->isIpv6())
        {
          # TODO: increment counter - just for FYI - not practical
          next;
        }
        elsif ($Obj->isIpv4())
        {
          # TODO: increment counter - just for FYI - not practical
        }
        else
        {
          print(STDERR "ERROR: TSNH: Not IPv4 or IPv6\n");
        }
      }
      else
      {
        print(STDERR 'ERROR BAD Flower Line: ' . $_ .  "\n");
        next;
      }
    }
    else
    {
      if ($#test1 == getFloFieldCount())
      {
        $Obj = new Flo(@test1);
      }
      else
      {
        print(STDERR 'ERROR BAD Flo    Line: ' . $_ .  "\n");
        next;
      }
    }

    $key = $Obj->getKey();
    if (exists($Summary{$key}))
    {
      print(STDOUT "Possible Duplicate Key: $key\n") if ($main::DEBUG);
      $key .= '_' . $Obj->getVlanId();
      if (exists($Summary{$key}))
      {
        print(STDERR "ERROR: Duplicate Flow: $key\n");
      }
    }
    else
    {
      $Summary{$key} = $Obj;
    }
  }
  return(%Summary);
}


sub buildIpv6Summary($@)
{
  my ($object, @Lines) = @_;
  my %Summary;
  my $Obj;
  my $key;

  foreach (@Lines)
  {
    if ($object eq "Flower")
    {
      # Only store IPv4 data to compare with Flo
      $Obj = new Flower(split(',', $_));
      next if ($Obj->isIpv4());
    }
    else
    {
      next;
    }
    $key = $Obj->getKey();
    $Summary{$key} = $Obj;
  }
  return(%Summary);
}


sub validateFiles($$$)
{
  my($file1, $file2, $file3) = @_;

  my @FloLines;
  my @FlowerLines;

  $gPcapFilename = $file3;
  $gWarning      = '';

  print(STDOUT "Reading files: $file1, $file2\n");

  my @temp1 = getFileLines($file1);
  my @temp2 = getFileLines($file2);

  my @test1 = split(',', $temp1[0]);
  my @test2 = split(',', $temp2[0]);

  if ($#test1 < 1)
  {
    warn("ERROR: Flower or Flo file, test1 = $file1, has no data");
    return;
  }

  if ($#test2 < 1)
  {
    warn("ERROR: Flower or Flo file, test2 = $file2, has no data");
    return;
  }

  if ($#test1 == getFloFieldCount() && $#test2 == getFlowerFieldCount())
  {
    @FloLines    = @temp1;
    @FlowerLines = @temp2;
  }
  elsif ($#test2 == getFloFieldCount() && $#test1 == getFlowerFieldCount())
  {
    @FloLines    = @temp2;
    @FlowerLines = @temp1;
  }
  else
  {
    warn("ERROR: bad Flower or Flo file, test1 = $#test1, test2 = $#test2");
    return;
  }

  print(STDOUT "Building Summaries...\n");

  my %FlowerSummaryV4 = buildIpv4Summary("Flower", @FlowerLines);
  my %FloSummary      = buildIpv4Summary("Flo",    @FloLines);
  my %FlowerSummaryV6 = buildIpv6Summary("Flower", @FlowerLines);

  my %Object1;
  my %Object2;

  if (scalar(keys(%FlowerSummaryV4)) != scalar(keys(%FloSummary)))
  {
    $gWarning .= "\nWARNING: Flower Summary Count " . (scalar(keys(%FlowerSummaryV4))) . ' != Flo Summary Count ' . (scalar(keys(%FloSummary))) . " for file $gPcapFilename\n";
  }

  if (scalar(keys(%FlowerSummaryV4)) >= scalar(keys(%FloSummary)))
  {
    %Object1 = %FlowerSummaryV4;
    %Object2 = %FloSummary;
  }
  else
  {
    %Object1 = %FloSummary;
    %Object2 = %FlowerSummaryV4;
  }

  return(processData(\%Object1, \%Object2));
}


sub protocolSplitter($$$$$$$$$$$)
{
  my(
      $Objref,
      $allKeys,
      $vlanKeys,
      $v_icmpKeys,
      $v_otherKeys,
      $v_tcpKeys,
      $v_udpKeys,
      $icmpKeys,
      $otherKeys,
      $tcpKeys,
      $udpKeys
    ) = @_;

  my $Object = getObjName($Objref);
  my $srcBytes;
  my $dstBytes;
  my $srcPkts;
  my $dstPkts;

  foreach my $flowkey (@${allKeys})
  {
    if (${$Objref}{$flowkey}->isVLAN() && ${$Objref}{$flowkey}->isFlower())
    {
      push(@${vlanKeys},    $flowkey);
      push(@${v_icmpKeys},  $flowkey) if (${$Objref}{$flowkey}->isICMP());
      push(@${v_otherKeys}, $flowkey) if (${$Objref}{$flowkey}->isOTHER());
      push(@${v_tcpKeys},   $flowkey) if (${$Objref}{$flowkey}->isTCP());
      push(@${v_udpKeys},   $flowkey) if (${$Objref}{$flowkey}->isUDP());
    }
    else
    {
      push(@${icmpKeys},    $flowkey) if (${$Objref}{$flowkey}->isICMP());
      push(@${otherKeys},   $flowkey) if (${$Objref}{$flowkey}->isOTHER());
      push(@${tcpKeys},     $flowkey) if (${$Objref}{$flowkey}->isTCP());
      push(@${udpKeys},     $flowkey) if (${$Objref}{$flowkey}->isUDP());
    }
    $srcBytes += ${$Objref}{$flowkey}->getSrcBytes();
    if (${$Objref}{$flowkey}->getDstBytes())
    {
      $dstBytes += ${$Objref}{$flowkey}->getDstBytes();
    }
    $srcPkts  += ${$Objref}{$flowkey}->getSrcPkts();
    if (${$Objref}{$flowkey}->getDstPkts())
    {
      $dstPkts  += ${$Objref}{$flowkey}->getDstPkts();
    }
  }

  my $output = '';
  $output .= write_percent("$Object ICMP",  scalar(@${icmpKeys}),  scalar(@${allKeys}));
  $output .= write_percent("$Object OTHER", scalar(@${otherKeys}), scalar(@${allKeys}));
  $output .= write_percent("$Object TCP",   scalar(@${tcpKeys}),   scalar(@${allKeys}));
  $output .= write_percent("$Object UDP",   scalar(@${udpKeys}),   scalar(@${allKeys}));

  if ($Object =~ /flower/i && $main::VLAN)
  {
    $output .= write_percent("$Object VLAN",   scalar(@${vlanKeys}),    scalar(@${allKeys}));
    $output .= write_percent("$Object VICMP",  scalar(@${v_icmpKeys}),  scalar(@${allKeys}));
    $output .= write_percent("$Object VOTHER", scalar(@${v_otherKeys}), scalar(@${allKeys}));
    $output .= write_percent("$Object VTCP",   scalar(@${v_tcpKeys}),   scalar(@${allKeys}));
    $output .= write_percent("$Object VUDP",   scalar(@${v_udpKeys}),   scalar(@${allKeys}));
  }
  $output .= write_total("$Object TOTAL FLOWS", scalar(@${allKeys}));
  $output .= write_total("$Object SrcBytes",    $srcBytes);
  $output .= write_total("$Object DstBytes",    $dstBytes);
  $output .= write_total("$Object TtlBytes",   ($srcBytes + $dstBytes));
  $output .= write_total("$Object SrcPkts",     $srcPkts);
  $output .= write_total("$Object DstPkts",     $dstPkts);
  $output .= write_total("$Object TtlPkts",    ($srcPkts  + $dstPkts));

  return($output);
}


sub processData($$)
{
  my($Obj1ref, $Obj2ref) = @_;

  my @allKeys1       = sort(keys(%{$Obj1ref}));
  my @allKeys2       = sort(keys(%{$Obj2ref}));

  my @icmpKeys1;
  my @otherKeys1;
  my @tcpKeys1;
  my @udpKeys1;
  my @vlanKeys1;
  my @v_icmpKeys1;
  my @v_otherKeys1;
  my @v_tcpKeys1;
  my @v_udpKeys1;

  my @icmpKeys2;
  my @otherKeys2;
  my @tcpKeys2;
  my @udpKeys2;
  my @vlanKeys2;
  my @v_icmpKeys2;
  my @v_otherKeys2;
  my @v_tcpKeys2;
  my @v_udpKeys2;

  my $output = '';

  $output .= protocolSplitter(
                               $Obj1ref,
                               \@allKeys1,
                               \@vlanKeys1,
                               \@v_icmpKeys1,
                               \@v_otherKeys1,
                               \@v_tcpKeys1,
                               \@v_udpKeys1,
                               \@icmpKeys1,
                               \@otherKeys1,
                               \@tcpKeys1,
                               \@udpKeys1
                             );

  $output .= protocolSplitter(
                               $Obj2ref,
                               \@allKeys2,
                               \@vlanKeys2,
                               \@v_icmpKeys2,
                               \@v_otherKeys2,
                               \@v_tcpKeys2,
                               \@v_udpKeys2,
                               \@icmpKeys2,
                               \@otherKeys2,
                               \@tcpKeys2,
                               \@udpKeys2
                             );

  $output .= processProtocol('ICMP',   $Obj1ref, $Obj2ref, \@icmpKeys1,    \@icmpKeys2);
  $output .= processProtocol('VICMP',  $Obj1ref, $Obj2ref, \@v_icmpKeys1,  \@icmpKeys2)   if ($main::VLAN);
  $output .= processProtocol('TCP',    $Obj1ref, $Obj2ref, \@tcpKeys1,     \@tcpKeys2);
  $output .= processProtocol('VTCP',   $Obj1ref, $Obj2ref, \@v_tcpKeys1,   \@tcpKeys2)    if ($main::VLAN);
  $output .= processProtocol('UDP',    $Obj1ref, $Obj2ref, \@udpKeys1,     \@udpKeys2);
  $output .= processProtocol('VUDP',   $Obj1ref, $Obj2ref, \@v_udpKeys1,   \@udpKeys2)    if ($main::VLAN);
  $output .= processProtocol('OTHER',  $Obj1ref, $Obj2ref, \@otherKeys1,   \@otherKeys2);
  $output .= processProtocol('VOTHER', $Obj1ref, $Obj2ref, \@v_otherKeys1, \@otherKeys2)  if ($main::VLAN);

  #$output .= printOther($Obj1ref, @otherKeys1);
  #$output .= printOther($Obj2ref, @otherKeys2);
  #$output .= printOther($Obj1ref, @v_otherKeys1) if ($main::VLAN);
  #$output .= printOther($Obj2ref, @v_otherKeys2) if ($main::VLAN);

  return($output);
}


sub processProtocol($$$$$)
{
  my($protocol, $Obj1ref, $Obj2ref, $objKeys1, $objKeys2) = @_;

  my $matches    = 0;
  my $mismatches = 0;
  my $missing    = 0;
  my $output     = '';

  my %Mismatches;
  my %AggrigateMismatches;
  %gExceptions = ();

  foreach $gFlowkey (@${objKeys1})
  {
    my $Obj1;
    my $Obj2;

    if (exists(${$Obj2ref}{$gFlowkey}))
    {
      $Obj1 = ${$Obj1ref}{$gFlowkey};
      $Obj2 = ${$Obj2ref}{$gFlowkey};
    }
    else
    {
      $missing++;
      print(STDOUT "ERROR: Missing $gFlowkey in " . getObjName($Obj2ref) . "\n") if ($main::DEBUG);
      next;
    }

    my $match        = 1;
    my $debug_output = '';
    my @Mismatches   = ();
    my $kcount       = 0;
    my $malformed    = 0;

    my $flower = $Obj1;
    my $flo    = $Obj2;
    if ($Obj1->isFlo())
    {
      $flo    = $Obj1;
      $flower = $Obj2;
    }

    foreach my $field ($Obj1->keylist())
    {
      next if ($field =~ m/^_/);
      if (exists($Obj2->{$field}))
      {
        if (! isMatch($flower->{$field}, $flo->{$field}))
        {
          #
          # skip printing noisy mismatches caused by KNOWN Differences
          #
          if ($field eq 'TIMET' || $field eq 'DURATION')
          {
            $kcount += kdiff_TIME_FLOAT($field, $flower, $flo);
            next;
          }

          if ($field =~ /^..._ADDR_V4$/)
          {
            $kcount += kdiff_ZERO_PADDED_FIRST_OCTET($field, $flower, $flo);
            next;
          }

          if ($field =~ /^..._PAYLOAD$/)
          {
            $kcount += kdiff_PAYLOAD($field, $flower, $flo);
            next;
          }

          if ($flo->{'_PROTO'} == 6)
          {
            if ($field eq 'TCP_EARLY_LATE_FLAG')
            {
              $kcount += kdiff_TCP_EARLY_LATE_FLAG($field, $flower, $flo);
              next;
            }

            if ($field =~ /_TCP_SEQ$/)
            {
              if ($field eq 'SRC_LAST_TCP_SEQ')
              {
                $kcount += kdiff_SRC_LAST_TCP_SEQ($field, $flower, $flo);
                next;
              }
              else
              {
                $kcount += kdiff_TCP_SEQ($field, $flower, $flo);
                next;
              }
            }

            if ($field =~ /^..._TCP_OPTS$/)
            {
              $kcount += kdiff_TCP_OPTS($field, $flower, $flo);
              $malformed++;
              next;
            }

            if ($field =~ /^..._TCP_FLAGS$/ && $malformed > 0)
            {
              next;
            }
          }

          if ($flower->{'_FRAGMENT'} == 1)
          {
            $kcount += kdiff_FRAGMENT($field, $flower, $flo);
            next;
          }


          # NEWRULE NEW RULE: Put new rules just above this line so
          # that you don't affect/impact previous rule results

          $debug_output .= "MISMATCH:$field:" . getObjName($Obj1ref) . "," . $flower->{$field} . ':' . getObjName($Obj2ref) . ',' . $Obj2->{$field} . ":\n";
          $match = 0;

          # Keep track of the types of mismatches
          $Mismatches{$field}++;
          push (@Mismatches,$field);
        }
      }
      else
      {
        print(STDOUT "ERROR: Missing field in " . getObjName($Obj2ref) . ":$field!\n");
      }
    }

#    if ($main::DEBUG && $kcount > 1)
#    { 
#      $gWarning .= "WARNING:KNOWNDIFFCNT:" . getPcapFilename() . ":$gFlowkey:$kcount\n";
#    }

    if ($match == 1)
    {
      delete(${$Obj1ref}{$gFlowkey});
      delete(${$Obj2ref}{$gFlowkey});
      $matches++;
    }
    else
    {
      $mismatches++;
      if ($main::DEBUG)
      {
        $debug_output .= "PROBLEM:./gen_wireshark_filter.pl $gFlowkey -i " . getPcapFilename() . "\n";
        $debug_output .= getObjName($Obj1ref) . ":" . $flower->getFlow() . ":\n";
        $debug_output .= getObjName($Obj2ref) . ":" . $Obj2->getFlow() . ":\n";
        print(STDOUT "Mismatch in FlowKey: $gFlowkey\n");
        print(STDOUT $debug_output);
      }

      # create aggrigate key
      my $AggrigateKey = join(',',sort(@Mismatches));

      $AggrigateMismatches{$AggrigateKey}++;

    }
  }

  my $Object1 = getObjName($Obj1ref);
  my $Object2 = getObjName($Obj2ref);

  $output .= write_label('Protocol', $protocol);
  $output .= write_percent("$protocol Match",    $matches,    scalar(@${objKeys1}));
  if ($mismatches)
  {
    $output .= write_percent("$protocol MisMatch", $mismatches, scalar(@${objKeys1}));
  }
  if ($missing)
  {
    $output .= write_percent("$protocol Missing",  $missing,    scalar(@${objKeys1}));
  }

  foreach (sort(keys(%gExceptions)))
  {
    if ($gExceptions{$_})
    {
      $output .= write_percent("$protocol EX $_", $gExceptions{$_}, scalar(@${objKeys1}));
    }
  }

  if ($mismatches)
  {
    $output .= write_label('Mismatch Breakdown', $protocol);
    foreach my $key (sort(keys(%Mismatches)))
    {
      $output .= write_mismatch($protocol, $key, $Mismatches{$key});
    }

    $output .= write_label('Aggrigate Mismatch Breakdown', $protocol);
    foreach my $key (sort(keys(%AggrigateMismatches)))
    {
      $output .= write_mismatch($protocol, $key, $AggrigateMismatches{$key});
    }
  }

  return($output);
}


sub getWarnings()
{
  return($gWarning);
}


sub getExampleExceptions()
{
  my $output = '';

  foreach (sort(keys(%gExampleExceptions)))
  {
    if ($gExampleExceptions{$_})
    {
      $output .= $gExampleExceptions{$_};
    }
  }

  return($output);
}


sub getOrigin($)
{
  my($field) = @_;
  my $origin = '';

  if ($field =~ /SRC_/)
  {
    $origin = 'SRC';
  }
  if ($field =~ /DST_/)
  {
    $origin = 'DST';
  }

  return($origin);
}


sub updateException($)
{
  my($exception) = @_;
  $gExceptions{$exception}++;
  $gExceptionCnt{$exception}++;

  if ($gExceptionCnt{$exception} < 4)
  {
    $gExampleExceptions{$exception} .= "EXCEPTION:$exception:$gFlowkey:" . getPcapFilename() . "\n";
  }
}


sub kdiff_FRAGMENT($$$)
{
  my($field, $flower, $flo) = @_;
  my $origin = getOrigin($field);

  if ($field =~ /^..._BYTES$/ || $field =~ /^..._PACKETS$/)
  {
    if (
         ($flower->{"${origin}_PACKETS"} >  $flo->{"${origin}_PACKETS"}     &&
          $flower->{"${origin}_BYTES"}   >= $flo->{"${origin}_BYTES"})  ||
         ($flower->{"${origin}_PACKETS"} >= $flo->{"${origin}_PACKETS"}     &&
          $flower->{"${origin}_BYTES"}   >  $flo->{"${origin}_BYTES"})
       )
    {
      #  KNOWN Difference: Flo and Flower handle IP Fragmentation differently
      updateException('IP_FRAGMENTATION');
      return(1);
    }
  }

  return(0);
}


sub kdiff_TCP_SEQ($$$)
{
  my($field, $flower, $flo) = @_;

  if ($flower->{$field} ne '' && $flower->{$field} == 0 && $flo->{$field} > 0)
  {
    #  KNOWN Difference: Flo sets SRC_FIRST_TCP_SEQ to first non-zero value
    #  Ex: 2008-11-10.14.46.54.sc08isp.pcap
    updateException('ZERO_TCP_SEQ');
    return(1);
  }

  return(0);
}


sub kdiff_TCP_OPTS($$$)
{
  my($field, $flower, $flo) = @_;
  my $origin = getOrigin($field);

  if ($flo->{"${origin}_TCP_OPTS"} ne "")
  {
    if (
         $flo->{"${origin}_TCP_OPTS"}   >= $flower->{"${origin}_TCP_OPTS"}   &&
         $flower->{"${origin}_PACKETS"} eq $flo->{"${origin}_PACKETS"}
       )
    {
      #  KNOWN Difference: Flo does not handle *_TCP_FLAGS correctly
      #                    Flo sets the tcp flags to bogus data sometimes
      #                    if the header is less than 20 (malformed/illegal)
      updateException('MALFORMED_TCP_HEADER');
      return(1);
    }
  }
  return(0);
}


sub kdiff_SRC_LAST_TCP_SEQ($$$)
{
  my($field, $flower, $flo) = @_;

  if (
      $flo->{$field}        ne $flo->{'SRC_FIRST_TCP_SEQ'}  &&
      $flo->{'SRC_PACKETS'} == 1
     )
  {
    #  KNOWN Difference: Flo does not populate SRC_LAST_TCP_SEQ or Flower does
    updateException('ONE_PACKET_ZERO_SEQ');
    return(1);
  }

  return(0);
}


sub kdiff_TCP_EARLY_LATE_FLAG($$$)
{
  my($field, $flower, $flo) = @_;

  if (($flo->{'SRC_PACKETS'} + $flo->{'DST_PACKETS'}) > 37)
  {
    my $temp =  $flo->{$field};
    $temp    =~ s/</_LT_/g;
    $temp    =~ s/>/</g;
    $temp    =~ s/_LT_/>/g;

    if ($temp eq $flower->{$field})
    {
      #  KNOWN Difference: Flo has reversed "<" and ">" from data guide
      updateException('REVERSED_TCP_LATE_FLAG');
      return(1);
    }
  }

  if (
      ($flo->{'SRC_PACKETS'} + $flo->{'DST_PACKETS'}) >  32 &&
      ($flo->{'SRC_PACKETS'} + $flo->{'DST_PACKETS'}) <= 37
     )
  {
    my $prec   = 3 * 27;
    my $field1 = substr($flower->{$field}, 0, $prec);
    my $field2 = substr($flo->{$field},    0, $prec);
    if ($field1 eq $field2)
    {
      #  KNOWN Difference: Flower starts removing flags at 28, Flow at 32
      updateException('TCP_FLAG_SHIFT');
      return(1);
    }
  }

  return(0);
}


sub kdiff_PAYLOAD($$$)
{
  my($field, $flower, $flo) = @_;
  my $origin = getOrigin($field);

  if ($flo->{"${origin}_BYTES"} >= 46 && $flo->{$field} eq '0')
  {
    if ($flo->{'_PROTO'} !=  1 && $flo->{'_PROTO'} !=  6 && $flo->{'_PROTO'} != 17)
    {
      #  KNOWN Difference: Flo does not calculate SRC_PAYLOAD correctly for OTHER Protocols
      updateException('ZERO_OTHER_PAYLOAD');
      return(1);
    }
    if ($flo->{'_PROTO'} ==  1)
    {
      #  KNOWN Difference: Flo does not calculate DST_PAYLOAD correctly for ICMP
      updateException('ZERO_ICMP_PAYLOAD');
      return(1);
    }
  }
  if ($flo->{"${origin}_PAYLOAD"} > $flo->{"${origin}_BYTES"})
  {
    #  KNOWN Difference: Flo does not calculate *_PAYLOAD correctly
    #                    with malformed Layer 3 headers
    updateException('PAYLOAD_EXCEEDS_BYTES');
    return(1);
  }
  if ($flo->{'_PROTO'} == 17)
  {
    if (
         $flower->{'SRC_PAYLOAD'}           != $flo->{'SRC_PAYLOAD'}   &&
         $flower->{'SRC_BYTES'}             == $flo->{'SRC_BYTES'}     &&
        ($flower->{'SRC_PAYLOAD'}
          + ($flower->{'SRC_PACKETS'} * 42) == $flower->{'SRC_BYTES'})
       )
    {
      #  KNOWN Difference: Flo calculates UDP payload using the length
      #                    in the UDP header rather than using the length
      #                    in the IP header and subtracting 8
      updateException('UDP_SRC_PAYLOAD');
      return(1);
    }
  }
  return(0);
}


sub kdiff_ZERO_PADDED_FIRST_OCTET($$$)
{
  my($field, $flower, $flo) = @_;
  #  KNOWN Difference: Flo pads the first octet of IP address, Flower does not
  #  Ex: 2008-11-10.00.44.16.sc08isp.pcap
  if (
      ('0' x (12 - length($flower->{$field})) . $flower->{$field}) eq
      ('0' x (12 - length($flo->{$field}))    . $flo->{$field})
     )
  {
    updateException('ZERO_PADDED_FIRST_OCTET');
    return(1);
  }
  return(0);
}

sub kdiff_TIME_FLOAT($$$)
{
  my($field, $flower, $flo) = @_;
  #  KNOWN Difference: Flo calculates duration using floating
  #                 point math. Only accurate to 6 places
  #                 e.g.   1.123423 =>   1.123423
  #                 e.g.  11.123423 =>  11.123420
  #                 e.g. 111.123423 => 111.123400
  my $prec = 7;
  if ($field eq 'TIMET')
  {
    $prec = 15;
  }
  my ($field1) = (substr($flower->{$field}, 0, $prec) =~ s/\.//);
  my ($field2) = (substr($flo->{$field},    0, $prec) =~ s/\.//);
  if ($field1 eq $field2)
  {
    updateException('TIME_FLOAT');
    return(1);
  }
  else
  {
    if ($field1 < $field2)
    {
      if (($field1 + 1) == $field2)
      {
        updateException('TIME_FLOAT');
        return(1);
      }
    }
    else
    {
      if (($field2 + 1) == $field1)
      {
        updateException('TIME_FLOAT');
        return(1);
      }
    }
  }
  return(0);
}


sub getObjName($)
{
  my($Objref) = @_;
  my $Object;
  foreach my $flowkey (sort(keys(%{$Objref})))
  {
    $Object  = ${$Objref}{$flowkey}{'_CLASS'};
    $Object .= ' ' x (length('Flower') - length($Object));
    last;
  }
  return($Object);
}


sub getObjSize($)
{
  my($Objref) = @_;
  my $Object;
  return(scalar(keys(%{$Objref})));
}


sub write_label {
  ($gLabel, $gValue) = @_;
  my $comment_line = "#------------------------------------------------------------\n";
  $~ = "LABELLINE";
  write;
  return($comment_line . "# $gLabel $gValue\n" . $comment_line);
}


sub write_percent {
  my($label, $numerator, $denominator) = @_;
  $gLabel   =  $label;
  if ($denominator)
  {
    $gPercent = ($numerator / $denominator) * 100.00;
  }
  else
  {
    $gPercent = 0;
  }
  $gCount = $numerator;
  $gCount = commify($gCount);
  $~ = "PERCENTLINE";
  write;
  return("__$label:$gCount\n");
}


sub write_total {
  ($gLabel, $gCount) = @_;
  if ($gCount =~ m/\./)
  {
    $~ = "F_TOTALLINE";
  }
  else
  {
    $~ = "TOTALLINE";
  }
  $gCount = commify($gCount);
  write;
  return("$gLabel:$gCount\n");
}


sub commify ($)
{
  my($input)= @_;
  $input = reverse($input);
  $input =~ s<(\d\d\d)(?=\d)(?!\d*\.)><$1,>g;
  return(reverse($input));
}


sub write_mismatch {
  ($gProtocol, $gLabel, $gCount) = @_;
  my @labels = split(',',$gLabel);
  my $first = shift(@labels);
  my $comma = '';
  if (scalar(@labels))
  {
    $comma = ',';
  }
  write_total("$first$comma", $gCount);
  foreach (@labels)
  {
    print(STDOUT "  $_,\n");
  }
  return("$gProtocol $gLabel:$gCount\n");
}


format LABELLINE =
-----------------------------------------------------------------------
@<<<<<<<<<<<<<<<<<<<<<<<<<<<< : @>>>>>>>
$gLabel,        $gValue
.


format F_TOTALLINE =
@<<<<<<<<<<<<<<<<<<<<<<<<<<<< : @>>>>>>>>>>>>>>>>>>>>>
$gLabel,        $gCount
.


format TOTALLINE =
@<<<<<<<<<<<<<<<<<<<<<<<<<<<< : @>>>>>>>>>>>>>>>>>>
$gLabel,        $gCount
.


format PERCENTLINE =
@<<<<<<<<<<<<<<<<<<<<<<<<<<<< : @>>>>>>>>>>>>>>>>>> : @##.##%
$gLabel,        $gCount,   $gPercent
.


1;
