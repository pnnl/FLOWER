#!/usr/bin/perl

use strict;
use Data::Dumper;
use Getopt::Long;
use List::Util qw(min max);
use XML::LibXML;
use XML::XPath;
use XML::DOM;
use XML::Simple;

my $CONFDIR  = "$ENV{'FLOWER_HOME'}/conf";
my $DEBUG    = 0;
my $HELP     = 0;
my $FILE     = '';    # Something like "$CONFDIR/flower_defs.xml"


sub error($)
{
  my($error) = @_;

  print("\n  ERROR: $error\n\n");

  return;
}


sub setPlugin
{
  my($plugin) = @_;
  my $file    = $CONFDIR . '/' . $plugin . '_defs.xml';

  if ("" eq $plugin)
  {
    error("Missing plugin name");
    usage();
    exit(1);
  }

  if (! -d $CONFDIR)
  {
    error("Missing configuration directory, $CONFDIR");
    exit(1);
  }

  $FILE = $file;

  return;
}


sub usage()
{
  print("\n");
  print("  Usage: $0 [dh] plugin\n\n");
  print("    -d = Turn on debug output\n");
  print("    -h = Help\n");
  print("\n");
}


sub init()
{
  Getopt::Long::Configure("bundling");
  Getopt::Long::Configure("permute");
  GetOptions("d+" => \$main::DEBUG, "h+" => \$HELP, '<>' => \&setPlugin);

  if ($HELP)
  {
    usage();
    exit(0);
  }

  # FIX: TODO: 
  $FILE = $CONFDIR . '/' . 'flower' . '_defs.xml';

  if (! -f $FILE)
  {
    error("Missing plugin file, $FILE");
    exit(1);
  }

  return;
}


sub lookupBytes($)
{
  my($type)  = @_;
  my %lookup = (
                 'time_t'         => 4,
                 'u_int64_t'      => 8,
                 'u_int32_t'      => 4,
                 'u_int16_t'      => 2,
                 'u_int08_t'      => 1,
                 'u_int8_t'       => 1,
                 'char unsigned'  => 1,
                 'unsigned char'  => 1,
               );

  if (! defined($lookup{$type}))
  {
    die("No data type or no definition for data type: $type");
  }

  return($lookup{$type});
}


sub lookupRefs($)
{
  my($type)  = @_;
  my %lookup = (
                 'vector<char unsigned>' => 1,
               );
  my $result = '';

  if (defined($lookup{$type}))
  {
    $result = '& ';
  }

  return($result);
}


sub getActiveFields($)
{
  my($tree) = @_;
  my %fields;
  foreach my $field (keys(%{$tree->{'Field'}}))
  {
    next if ($tree->{'Field'}->{$field}->{'active'} ne 'true');
    $fields{$field} = $tree->{'Field'}->{$field};
  }

  return(%fields);
}


sub getFieldOrder($%)
{
  my($attribute_order, %fields) = @_;
  my %order;
  foreach my $field (keys(%fields))
  {
    my $number      = $fields{$field}->{$attribute_order};
    if (defined($order{$number}))
    {
      die("Field $field is trying to use $attribute_order value $number that is already used by $order{$number}");
    }
    $order{$number} = $field;
  }

  return(%order);
}


sub getDataType($$)
{
  my($node, $pad) = @_;
  my $dataType = $node->{'Container'}->{'Data'}->{'Type'};
  my $quantity = $node->{'Container'}->{'Data'}->{'Quantity'};

  return("Data<$dataType, " . ' ' x ($pad - length($dataType)) . "$quantity>");
}


sub getPhysicalStruct(%%)
{
  my($memory, $fields) = @_;
  my $struct     = "typedef struct PhysicalStruct_t\n";
  my $maxName    = length('padding');
  my $maxQuan    = 0;
  my $maxSize    = 0;
  my $maxType    = 0;
  my $totalBytes = 0;

  foreach my $field (sort(keys(%$memory)))
  {
    my $dataType = $fields->{$memory->{$field}}->{'Container'}->{'Data'}->{'Type'};
    my $quantity = $fields->{$memory->{$field}}->{'Container'}->{'Data'}->{'Quantity'};
    my $size     = lookupBytes($dataType);
    $maxName     = max($maxName, length($memory->{$field}));
    $maxQuan     = max($maxQuan, length($quantity));
    $maxSize     = max($maxSize, length($size));
    $maxType     = max($maxType, length($dataType));
  }

  $struct       .= "{\n";
  foreach my $field (sort(keys(%$memory)))
  {
    my $dataType = $fields->{$memory->{$field}}->{'Container'}->{'Data'}->{'Type'};
    my $quantity = $fields->{$memory->{$field}}->{'Container'}->{'Data'}->{'Quantity'};
    my $size     = lookupBytes($dataType);
    my $name     = $memory->{$field};
    my $TSPACE   = ' ' x ($maxType - length($dataType));
    my $FSPACE   = ' ' x ($maxName - length($name));
    $struct     .= "  Data<$dataType, " . $TSPACE . "$quantity>  $name; " . $FSPACE . "// $quantity * $size = " . ($quantity * $size) . " bytes;\n";
    $totalBytes += $quantity * $size;
  }
  if (($totalBytes % 8) != 0)
  {
    my $dataType = 'u_int8_t';
    my $quantity = ($totalBytes % 8);
    my $size     = 1;
    my $name     = 'padding';
    my $TSPACE   = ' ' x ($maxType - length($dataType));
    my $FSPACE   = ' ' x ($maxName - length($name));
    $struct     .= "  Data<$dataType, " . $TSPACE . "$quantity>  $name; " . $FSPACE . "// $quantity * $size = " . ($quantity * $size) . " bytes;\n";
    $totalBytes += $quantity * $size;
  }
  $struct .= "} PhysicalStruct;   //  Total Bytes = $totalBytes\n";

  return($struct);
}


sub getPolicyNames(%%)
{
  my($order, $fields) = @_;
  my $methods;
  my %methods;

  foreach my $field (sort(keys(%$order)))
  {
    my $i    = 0;
    my $done = 0;
    while (! $done)
    {
      my $method  = '';
      # DEVELOPER NOTE: What about multiple arguments?
      # DEVELOPER NOTE: What about return types?
      if (ref($fields->{$order->{$field}}->{'Policies'}->[$i]->{'Policy'}->{'Method'}) eq 'HASH')
      {
        $method = $fields->{$order->{$field}}->{'Policies'}->[$i]->{'Policy'}->{'Method'}->{'content'};
      }
      else
      {
        $method = $fields->{$order->{$field}}->{'Policies'}->[$i]->{'Policy'}->{'Method'};
      }
      last if ($method eq '');
      $methods{$method} = 1;
      $i++;
    }
  }

  return(sort(keys(%methods)));
}


sub getPolicyFunctions($%%)
{
  my($policy, $order, $fields) = @_;
  my $methods;
  my %methods;
  my %signatures;

  foreach my $field (sort(keys(%$order)))
  {
    my $i    = 0;
    my $done = 0;
    while (! $done)
    {
      my $type      = '';
      my $args      = '';
      my $method    = '';
      # DEVELOPER NOTE: What about return types?
      if (ref($fields->{$order->{$field}}->{'Policies'}->[$i]->{'Policy'}->{'Method'}) eq 'HASH')
      {
        my $node   = $fields->{$order->{$field}}->{'Policies'}->[$i]->{'Policy'}->{'Method'};
        $type      = $node->{'type'};
        $args      = $node->{'additional_args'};
        $method    = $node->{'content'};
        $signatures{$method} = $node->{'signature'};
      }
      else
      {
        $method    = $fields->{$order->{$field}}->{'Policies'}->[$i]->{'Policy'}->{'Method'};
      }
      last if ($method eq '');
      my $visitor = $fields->{$order->{$field}}->{'Policies'}->[$i]->{'Policy'}->{'VisitorMethod'};
      my $name    = $order->{$field};
      my $method_args = "this->physical_struct.$name";
      if ($type eq 'binary')
      {
        $method_args .= ", p_flow->physical_struct.$name";
      }
      if ($args ne '')
      {
        $method_args .= ", $args";
      }
      if ($policy eq $method && $visitor !~ /None$/)
      {
        $methods{$method} .= "  $visitor< " . getDataType($fields->{$order->{$field}}, 0) . " >::$method($method_args);\n";
      }
      $i++;
    }
  }

  foreach (sort(keys(%methods)))
  {
    my $signature = "void $_" . "_visitor(void) throw()";
    if (defined($signatures{$_}))
    {
      $signature = $signatures{$_};
    }
    $methods .= "$signature\n";
    $methods .= "{\n";
    $methods .= "$methods{$_}";
    $methods .= "}\n\n";
  }

  return($methods);
}


sub getAssignFunction($%)
{
  my($order, $fields) = @_;
  my $assign_function = '';
  my $assign_sig      = '';
  my $assign_impl     = '';
  my $parameters;
  my $body;

  # TODO: FIX: This should not be hardcoded
  open(FH, "$ENV{'FLOWER_HOME'}/conf/flower_sig.conf") || die("Can't open PacketParser.hpp: $!");
  while (<FH>)
  {
    $assign_sig  .= $_;
    s/\s*=.*,/,/g;
    s/\s*=.*//g;
    $assign_impl .= $_;
  }
  close(FH);

  foreach my $field (sort(keys(%$order)))
  {
    my $baseDataType = $fields->{$order->{$field}}->{'Container'}->{'Data'}->{'Type'};
    my $type         = getDataType($fields->{$order->{$field}}, 0);
    my $ref          = lookupRefs($baseDataType);
    my $decl         = "$baseDataType const $ref";
    my $variables    = $fields->{$order->{$field}}->{'Container'}->{'Data'}->{'PopulateFromVariable'};
    #$parameters     .= ", $decl" . join(", $decl", split(/,\s*/, $fields->{$order->{$field}}->{'Container'}->{'Data'}->{'PopulateFromVariable'}));
    $body           .= "  this->physical_struct.$order->{$field}.assign($variables);\n";
  }

  chomp($assign_sig);
  $assign_function .= "$assign_sig;\n\n\n";
  $assign_function .= "$assign_impl";
  $assign_function .= "{\n";
  $assign_function .= $body . "\n  return;\n}\n";
  return($assign_function);
}


sub main()
{
  my $tree         = XMLin($FILE, KeyAttr => {Field => 'name'});
  my %fields       = getActiveFields($tree);
  my %print_order  = getFieldOrder('print_order',  %fields);
  my %memory_order = getFieldOrder('memory_order', %fields);
  
  #  Print some comments

  print("// Fields       = " . join(', ', keys(%fields)) . "\n\n");

  foreach (sort(keys(%print_order)))
  {
    print("// Print  Order = $_ -> " . $print_order{$_} . "\n");
  }
  print("\n");

  foreach (sort(keys(%memory_order)))
  {
    print("// Memory Order = $_ -> " . $memory_order{$_} . "\n");
  }
  print("\n");

  #  Print the Physical Struct

  print("// Data Structure = \n" . getPhysicalStruct(\%memory_order, \%fields) . "\n\n");
  
  #  Print the Policies

  foreach my $policy (getPolicyNames(\%memory_order, \%fields))
  {
    if ($policy eq 'toString')
    {
      print(getPolicyFunctions($policy, \%print_order, \%fields));
    }
    else
    {
      print(getPolicyFunctions($policy, \%memory_order, \%fields));
    }
    print("\n");
  }
  
  #  Print the Assignment Function

  print(getAssignFunction(\%memory_order, \%fields) . "\n");

  #print Dumper($tree);
}


init();
main();
  
exit(1);
