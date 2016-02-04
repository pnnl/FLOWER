package FileManip;

use vars qw(@ISA @EXPORT $gDebug);
require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(makeDir makePath copy slink move checkModTime);
# use strict;

use Debug;
$gDebug = 0;

# sub fdev   () {  0 }
# sub fino   () {  1 }
# sub fmode  () {  2 }
# sub fnlink () {  3 }
# sub fuid   () {  4 }
# sub fgid   () {  5 }
# sub fsize  () {  7 }
# sub fmtime () {  9 }

sub notifyCustodian($)
{
  my($message) = @_;
  print("$message\n");
}


sub makeDir {
  my $subname = 'makeDir';
  my $sub = new Debug($subname, $gDebug);
  #  Description:
  #    Makes the given directory, similar to mkdir -p
  #  Parameters:
  #    $dir  - (string), The directory to make
  #  Return Value(s):
  #    0 - Failure
  #    1 - Success
  #

  use POSIX;

  my($dir, $user, $group)      = @_;
  my $fullPath  = '';                 #  Absolute path
  my @path      = split('/', $dir);
  my $path      = shift(@path);

  debug($sub, &trace, "Entering", __LINE__);

  if ($path !~ /^\s*$/) {
    $fullPath = getcwd();             #  Relative path made absolute
    unshift(@path, $path);
  }

  #
  #  For each element in the path just keep creating the directories
  #  until the full path is created
  #
  foreach $path (@path) {
    $fullPath .= "/$path";
    debug($sub, &scalar, "Path = $path, Fullpath = $fullPath", __LINE__);
    if (! -e $fullPath) {

      #  The directory does not exist so make it using the current umask
      #  value of the user running this code

      if (! mkdir($fullPath, 0777)) {
        notifyCustodian("$subname - ERROR: Can't make directory, $fullPath: $!");
        debug($sub, &trace, "ERROR - Leaving", __LINE__);
        return(0);
      }
      chown($user, $group, $fullPath) if ( $user );
    }
  }  
  debug($sub, &trace, "Leaving", __LINE__);
  return(1);
}


sub makePath {
  my $subname = 'makePath';
  my $sub = new Debug($subname, $gDebug);
  #  Description:
  #    Makes the path for the filename 
  #  Parameters:
  #    $filename  - (string), The path to make
  #  Return Value(s):
  #    0 - Failure
  #    1 - Success
  #
  my ($file) = @_;
  my @parts = split ('/', $file);
  return 1 if ($#parts == 0);
  $file =~ s/\/$parts[$#parts]//;
  return (makeDir($file));

}


sub checkModTime {
  my $subname = 'checkModTime';
  my $sub = new Debug($subname, $gDebug);
  #
  #  Description:
  #    Compares the creation/modification time of the files(s) with
  #    the current date/time minus $minutes.  If the file(s) is newer
  #    then it is added to the list of files to be returned.
  #  Parameters:
  #    $minutes   - (integer), Number of minutes to compare
  #                            creation/modification times against.
  #    @files     - (array),   List of files to be checked.
  #  Return Value(s):
  #    @newFiles  - (array),   List of files from @files that have been
  #                            created/modified in the last $minutes minutes.
  #

  my($minutes, @files) = @_;
  my $compareTime;
  my $file;
  my $modTime;
  my @newFiles;

  debug($sub, &trace, "Entering", __LINE__);

  $compareTime = time() - ($minutes * 60);

  foreach $file (@files) {
    if (-e $file) {
      $modTime = (stat($file))[9];
      if ($modTime eq "" && $!) {
        notifyCustodian("$subname - ERROR: Can't stat file, $file: $!");
        }
      else { 
        push(@newFiles, $file) if ($modTime >= $compareTime); 
        }
      }
    else {
      notifyCustodian("$subname - ERROR: File not found, $file: $!");
      }
    }
  debug($sub, &trace, "Leaving", __LINE__);
  return(@newFiles);
}


sub copy {
  my $subname = 'copy';
  my $sub = new Debug($subname, $gDebug);
  #
  #  Description:
  #    Copy $fileOne to $fileTwo.
  #  Parameters:                                               
  #    $fileOne - (string), the file being copied and then removed.
  #    $fileTwo - (string), the file to be copied to.
  #  Return Value(s):
  #    1 - Success
  #    0 - Failure
  #

  my($fileOne, $fileTwo) = @_;
  my($defBlockSize) = 16 * 2**10;
  my $blockSize     = 0;
  my $buffer        = 0;
  my $offset        = 0;
  my $readCount     = 0;
  my $writeCount    = 0;

  debug($sub, &trace, "Entering", __LINE__);
  debug($sub, &lots,  "FileOne = $fileOne, FileTwo = $fileTwo", __LINE__);

  if (! -e $fileOne) {
    notifyCustodian("$subname - ERROR: Can't copy $fileOne to $fileTwo(__LINE__): $!");
    return(0);
  }

  open(SH, "<$fileOne") || (notifyCustodian("$subname - ERROR: Can't open file, $fileOne: $!"), return(0));
  open(DH, ">$fileTwo") || (notifyCustodian("$subname - ERROR: Can't open file, $fileTwo: $!"), return(0));

  $blockSize = (stat(SH))[11];
  $blockSize = $defBlockSize unless ($blockSize > $defBlockSize);

  #
  #  The following uses the low level system calls to efficiently
  #  read from the source file and write to the destination file.
  #
  while ($readCount = sysread(SH, $buffer, $blockSize)) {
    for ($offset = 0; $offset < length($buffer); $offset += $writeCount) {
       $writeCount = syswrite(DH, $buffer, length($buffer), $offset);
       if (! defined($writeCount)) {
         notifyCustodian("$subname - ERROR: syswrite on file, $fileTwo failed: $!");
         return(0);
       }
    } 
  } 
  if (! defined($readCount)) {
    notifyCustodian("$subname - ERROR: sysread on file, $fileOne failed: $!");
    return(0);
  }
  close(SH) || (notifyCustodian("$subname - Can't close file, $fileOne: $!"), return(0));
  close(DH) || (notifyCustodian("$subname - Can't close file, $fileTwo: $!"), return(0));
  debug($sub, &trace, "Leaving", __LINE__);
  return(1);
}


sub slink {
  my $subname = 'slink';
  my $sub = new Debug($subname, $gDebug);
  #
  #  Description:
  #    Symbolically Link $fileOne to $fileTwo.
  #    Called slink to avoid name clash with symlink and link
  #  Parameters:
  #    $fileOne - (string), the file being linked and then removed.
  #    $fileTwo - (string), the file to be linked to.
  #  Return Value(s):
  #    1 - Success
  #    0 - Failure
  #

  my($fileOne, $fileTwo) = @_;

  debug($sub, &trace, "Entering", __LINE__);
  debug($sub, &lots,  "FileOne = $fileOne, FileTwo = $fileTwo", __LINE__);

  if (! -e $fileOne) {
    notifyCustodian("$subname - ERROR: Can't link $fileOne to $fileTwo: $!");
    return(0);
  }

  if (eval { symlink("",""); 1 }) {
    #
    #  This system supports symbolic links
    #
    if (symlink($fileOne, $fileTwo)) {
      return(1);
    }
    else {
      notifyCustodian("$subname - ERROR: Can't link $fileOne to $fileTwo: $!");
      return(0);
    }
  }
  else {
    notifyCustodian("$subname - ERROR: This system does not support symbolic links");
    return(1);
  }
}


sub move {
  my $subname = 'move';
  my $sub = new Debug($subname, $gDebug);
  #
  #  Description:
  #    Copy $fileOne to $fileTwo and remove $fileOne.
  #    Assuming it is OK to overwrite $fileTwo.
  #  Parameters:                                               
  #    $fileOne - (string), the file being copied and then removed.
  #    $fileTwo - (string), the file to be copied to.
  #  Return Value(s):
  #    1 - Success
  #    0 - Failure
  #

  my($fileOne, $fileTwo) = @_;

  debug($sub, &trace,  "Leaving", __LINE__);
  debug($sub, &scalar, "FileOne = $fileOne, FileTwo = $fileTwo", __LINE__);

  if ( -l $fileOne ) {
    if (rename($fileOne, $fileTwo)) {
      debug($sub, &trace, "Rename Success - Leaving - return 1", __LINE__);
      return(1);
    }
  }

  if (copy($fileOne, $fileTwo)) {
    #
    #  The copy was successful so now we can delete the original file.
    #
    if (unlink($fileOne)) {
      debug($sub, &trace, "Copy and unlink Success - Leaving - return 1", __LINE__);
      return(1);
    }
    else {
      notifyCustodian("$subname - ERROR: Copy succeeded but can't remove file, $fileOne: $!");
      debug($sub, &trace, "Failure - Leaving - return 0", __LINE__);
      return(0);
    }
  }
  else {
    notifyCustodian("$subname - ERROR: Can't copy $fileOne to $fileTwo(__LINE__): $!");
    debug($sub, &trace, "Failure - Leaving - return 0", __LINE__);
    return(0);
  }
  debug($sub, &trace, "Leaving - Shouldn't get here!", __LINE__);
}


1;
