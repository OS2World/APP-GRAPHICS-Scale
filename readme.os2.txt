SCALE v1.0

Scales TGA files

This is a little program I developed while working with graphics files which were somewhat
bigger in size then normal.

The program will take an TGA picture and scale it by the given factor and write the resulting
TGA file. Sounds simple. It is. But that can be done in any graphics program you say. You are
right, but have you tried that with files of lets say more than 50 MB. Then most graphics programs
need more then twice the size of the picture in memory and which normal system has that.

This program just needs a small amount of memory because it simply reads the TGA file and writes
out the resulting points. No keeping of large pieces in memory. It also can be used in a batch file
to manipulate multiple files.

To generate the example file:	scale lion.tga lion2.tga 2

This will scale the source image by the factor 2

Requirements: The executable program requires at least a 386 cpu with OS/2.

Files Included

    SCALE.C		C source code
    SCALE.EXE		OS/2 executable
    README.OS2	this file
    LION.TGA		source image

Compiling source

The source code has been compiled and tested with

   Borland C++ v2.1 for OS/2

Disclaimer:

The author assumes no responsibility for the use of the program, source
code, documentation, sample data, and etc. 

Have fun.

U. A. Mueller
email: sicota.country@usa.net

comments welcome