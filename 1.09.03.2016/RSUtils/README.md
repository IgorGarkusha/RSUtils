# RSUtils
Remote Sensing Utilities.
Version 1.09.03.2016.
Last Update 19.03.2016.

**Welcome to RSUtils (Remote Sensing Utilities - Extentions GDAL/OGR)!**

RSUtils is free and open source (GPL) software.
RSUtils - this is a small empowerment tools GDAL command. RSUtils based on the capabilities of the GDAL/OGR library. When developing the used functionality of the libxml2 library.

Copyright (C) 2016, Igor Garkusha. Ukraine, Dnipropetrovsk.

## GDAL/OGR General

In general GDAL/OGR is licensed under an MIT/X style license with the 
following terms:

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

## XML toolkit from the GNOME project

Copyright (C) 1998-2012 Daniel Veillard. All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is fur-
nished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FIT-
NESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.



##RSUtils: build requirements

1. For GNU/Linux: installed GNU GCC/G++ compilers;
   for Mac OS X: installed Xcode;
   for Windows 32/64-bit: installed Windows SDK or Visual Studio with Visual C++ compiler.
2. Installed the library GDAL (only for GNU/Linux and Mac OS X).
3. Installed the library Libxml2 (only for GNU/Linux and Mac OS X).
4. Installed OpenJDK or Oracle JDK 1.8.
5. Installed Quantum GIS 2.12 (necessarily only for Windows! For other -- optional).


##RSUtils: building in GNU/Linux or Mac OS X

1. Add to PATH the location jdk/bin directory.
2. Add to PATH the location RSUtils/binary/target directory. Create system variable 
RSUTILS_HOME and set her the value in path to RSUtils/binary/target directory.
For example in GNU/Linux: 
	export RSUTILS_HOME=/home/username/bin/RSUtils/binary/target
	export PATH=$PATH:$RSUTILS_HOME
For example in Mac OS X:
	export RSUTILS_HOME=/Users/username/Documents/bin/RSUtils/binary/target
	export PATH=$PATH:$RSUTILS_HOME
3. Execute from console file build.sh. To clean the build directory to use the file clear.sh.


##RSUtils: building in Windows


1. Add to PATH the location jdk\bin directory.
2. Add to PATH the location RSUtils\binary\target directory. Create system variable 
RSUTILS_HOME and set her the value in path to RSUtils\binary\target directory.
For example: 
	RSUTILS_HOME=C:\RSUtils\binary\target
3. Create system variable QGIS_HOME and set her the value in path to QGIS 2.12 directory.
For example: 
	QGIS_HOME=C:\QGIS
4. Start Windows SDK Command Prompt. Go to the directory RSUtils and execute file 
build32.cmd (for Windows 32-bit) or build64.cmd (for Windows 64-bit).
To clean the build directory to use the file clear.cmd.
