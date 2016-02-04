Pointer Jason and I worked on the Windows port and cleaned up the Diagnostic code in the process.

You will need some software from \\PNL\Projects\ENSIP\Software

 - Install Boost in the default location (e.g. C:\Program Files\boost so you end up with C:\Program Files\boost\boost_1_35_0)
 - Install WinPcap in the default location (e.g. C:\Program Files\WinPcap)
 - Install the WpdPack in C:\Libraries\WpdPack

In your Visual Studio "Projects" directory make a directory named "ENSIP" and "Diagnostic".
Change directory to "ENSIP" and use TortoiseSVN "repo-browser" to check out ENSIP (https://cvs.pnl.gov/svn/ensip/ensip/trunk)
Change directory to "Diagnostic" and use TortoiseSVN "repo-browser" to check out ENSIP (https://cvs.pnl.gov/svn/swawt/diagnostic/trunk)
NOTE: If you don't have access, let me know and I can ask Tim Carlson to add you.

Open up the ENSIP Visual Studio solution ("ENSIP\support\other\ide\vs\ENSIP\ENSIP.sln") and click on the ENSIP solution and
"Rebuild". If everything works, the "ENSIP\support\other\ide\vs\ENSIP\flower.bat" should produce data in "ENSIP\data\flower\output\200803".

Darren