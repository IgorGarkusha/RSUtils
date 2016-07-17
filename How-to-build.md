## BUILD RSUtils

### RSUtils: build requirements

1. For GNU/Linux: installed GNU GCC/G++ compilers;
   for Mac OS X: installed Xcode;
   for Windows 32/64-bit: installed Windows SDK or Visual Studio with Visual C++ compiler
   or for Windows installed MinGW-32 GNU GCC compiler (for example with IDE Code::Blocks);
   for FreeBSD: installed Clang etc.

2. Installed the library GDAL (only for GNU/Linux, Mac OS X, FreeBSD (with support GEOS!)).

3. Installed the library Libxml2 (only for GNU/Linux, Mac OS X, FreeBSD).

4. Installed OpenJDK 1.8 or Oracle JDK 1.8.

5. Installed Quantum GIS 2.12 or high (necessarily only for MS Windows! For other -- optional).


### RSUtils: building in GNU/Linux, Mac OS X, FreeBSD

1.Add to PATH the location jdk/bin directory.

2.Add to PATH the location RSUtils/bin directory. Create system variable 
RSUTILS_HOME and set her the value in path to RSUtils/bin directory.

For example in GNU/Linux: 

	export RSUTILS_HOME=/home/username/bin/RSUtils/bin
	
	export PATH=$PATH:$RSUTILS_HOME
	
For example in Mac OS X:

	export RSUTILS_HOME=/Users/username/Documents/bin/RSUtils/bin
	
	export PATH=$PATH:$RSUTILS_HOME
	
For FreeBSD -- setup locale with UTF-8 support; support GEOS in GDAL library must be enabled!!!

3.Execute from console file build.sh (bash ./build.sh). To clean the build directory to use the file clear.sh (bash ./clear.sh).


### RSUtils: building in Microsoft Windows

1.Add to PATH the location jdk\bin directory.

2.Add to PATH the location RSUtils\bin directory. Create system variable 
RSUTILS_HOME and set her the value in path to RSUtils\bin directory.

For example: 

	RSUTILS_HOME=C:\RSUtils\bin
	
3.Create system variable QGIS_HOME and set her the value in path to QGIS 2.12 (or high version) directory.

For example: 

	QGIS_HOME=C:\QGIS
	
4.Start Windows SDK Command Prompt. Go to the directory RSUtils and execute file 
build_msvc.cmd (for MS Visual C++ compiler) or build_mingw32.cmd (for MinGW-32 GNU GCC).
To clean the build directory to use the file clear_msvc.cmd or clear_mingw32.cmd (depending on the build).
