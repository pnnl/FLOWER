#!/usr/bin/perl -w

use strict;
use warnings;

# one output line for Flo
package InitSetup;

# Other global statements
use vars qw(@ISA @EXPORT %STATUS);

@ISA    = qw(Exporter);
@EXPORT = qw(getTmpDir);


sub getTmpDir ()
{
  return('/dev/shm/' . $ENV{'USER'} . '/flower');
}


1;
