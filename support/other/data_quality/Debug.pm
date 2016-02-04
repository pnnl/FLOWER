package Debug;

#
#  DISCLAIMER
#
#   This material was prepared as an account of work sponsored by an agency
#   of the United States Government.  Neither the United States Government
#   nor the United States Department of Energy, nor Battelle, nor any of
#   their employees, MAKES ANY WARRANTY, EXPRESS OR IMPLIED,OR ASSUMES ANY
#   LEGAL LIABILITY OR RESPONSIBILITY FOR THE ACCURACY, COMPLETENESS, OR
#   USEFULNESS OF ANY INFORMATION, APPARATUS, PRODUCT, SOFTWARE, OR PROCESS
#   DISCLOSED, OR REPRESENTS THAT ITS USE WOULD NOT INFRINGE PRIVATELY
#   OWNED RIGHTS.
#
#  ACKNOWLEDGMENT
#
#   This software and its documentation were produced with Government
#   support under Contract Number DE-AC06-76RLO-1830 awarded by the United
#   States Department of Energy.  The Government retains a paid-up
#   non-exclusive, irrevocable worldwide license to reproduce, prepare
#   derivative works, perform publicly and display publicly by or for
#   the Government, including the right to distribute to other Government
#   contractors.
#
#
#  $Id: Debug.perl,v 1.2 2003/06/05 06:02:10 macduff sts-delivery-3.23-1.sol5_10 $
#  Abstract:
#    This package is useful to embed debugging information into
#    programs.
#  Usage: (Example)
#    use Debug.pm;
#    sub name () {
#    my $subname = 'name';
#    my $sub = new Debug($subname, $gDebug);
#    debug($sub, &trace,  "Entering", __LINE__);
#    debug($sub, &info,   "Usually information to help describe the current condition", __LINE__);
#    debug($sub, &scalar, "Usually one or more SCALAR values", __LINE__);
#    debug($sub, &array,  "Usually one or more ARRAY  values", __LINE__);
#    debug($sub, &lots,   "Usually conditions that produce FREQUENT or LOTS of output like every line of a file being read", __LINE__);
#    debug($sub, &trace,  "Exiting",  __LINE__);
#  Input file(s):
#    NONE
#  Output file(s):
#    NONE
#  Return Value(s);
#    NONE
#  Manifest:
#    NONE
#  Operating system(s):
#    Solaris 2.X
#  Author:  Darren Curtis
# 

use vars qw(%gDebugLevel %gDebug);
require Exporter;
@ISA = qw(Exporter);
@EXPORT    = qw(debug trace info scalar array lots);
@EXPORT_OK = qw(debug trace info scalar array lots);
use strict;
no strict 'refs';

#
#  Set up constants for debugging.
#
sub trace  () {  1 }
sub info   () {  2 }
sub scalar () {  4 }
sub array  () {  8 }
sub lots   () { 16 }


%gDebugLevel = (  '1' => 'Trace ',
                  '2' => 'Info  ',
                  '4' => 'Scalar',
                  '8' => 'Array ',
                 '16' => 'Lots  ');


sub debug ($$$$) {
  my $sub = 'debug';
  #  Description:
  #    Print out debugging messages
  #  Parameters:
  #    $routine - (string),  The calling routine
  #    $num     - (integer), The debugging level for the message in $string
  #                          (i.e. &trace, &info, &scalar, &array, &lots)
  #    $string  - (string),  The message to be printed
  #  Return Value(s):
  #    NONE
  #
 
  my($routine, $num, $string, $line) = @_;
  my $warn = $^W;

  $main::gRes{S}{'WHEREAMI'} = "$$routine{'name'}, Line = $line\n\tDebug Message = $string";
  $main::sleep && sleep($main::sleep);

  $^W      =  0;

  #  Uncomment the following lines to debug
  #print "Debug - Routine = $$routine{'name'}, Level = $$routine{'level'}\n";
  #print "Debug - Num     = $num,              String = $string\n";

  if (!defined($$routine{'name'})) {
    my($pkg, $file, $line) = caller();
    print(STDOUT "DEBUG ERROR - No routine ($routine) defined in:\n              File ($file), Package ($pkg), Line ($line).\n");
    $^W = $warn;
    return(0);
  }
  if (($$routine{'level'} & $num) == $num) {
    print(STDOUT "$gDebugLevel{$num} $$routine{'name'} - $string\n") if (length($string));
    $^W = $warn;
    return(1);
  }
}


sub new ($$$) {
  #  Description:
  #    Register debugging level for a routine
  #  Parameters:
  #    $pkg   - (string),  The calling package
  #    $sub   - (string),  The name of the subroutine
  #    $level - (integer), The debugging level (0-31)
  #  Return Value(s):
  #    Blessed reference to be used by Debug::debug
  #
 
  my($pkg, $sub, $level) = @_;
  my $debug = {'name' => $sub, 'level' => $level};
  bless $debug, $pkg;
  return($debug);
}


1;
