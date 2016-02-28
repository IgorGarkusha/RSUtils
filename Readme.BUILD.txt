============================================================================================
Build requirements
============================================================================================

1. For GNU/Linux: installed GNU GCC/G++ compilers;
   for Mac OS X: installed Xcode;
   for Windows 32/64-bit: installed Windows SDK or Visual Studio with Visual C++ compiler.
2. Installed the library GDAL (only for GNU/Linux and Mac OS X).
3. Installed the library Libxml2 (only for GNU/Linux and Mac OS X).
4. Installed OpenJDK or Oracle JDK 1.8.
5. Installed Quantum GIS 2.12 (necessarily only for Windows! For other -- optional).

============================================================================================
Building in GNU/Linux or Mac OS X
============================================================================================

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

============================================================================================
Building in Windows
============================================================================================

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
