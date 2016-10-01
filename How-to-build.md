## BUILD RSUtils

### RSUtils: build requirements

1. For GNU/Linux: installed GNU GCC/G++ compilers;
   for Mac OS X: installed Xcode;
   for Windows 32/64-bit: installed Windows SDK or Visual Studio with Visual C++ compiler
   or for Windows installed MinGW-32 GNU GCC compiler (for example with IDE Code::Blocks);
   for FreeBSD: installed Clang etc.

2. Installed the CMake (for Mac OS X and MS Windows from: https://cmake.org/download/).

3. Installed the library GDAL (only for GNU/Linux, Mac OS X, FreeBSD (with support GEOS!)).

4. Installed the library Libxml2 (only for GNU/Linux, Mac OS X, FreeBSD).

5. Installed OpenJDK 1.8 or Oracle JDK 1.8.

6. Installed x86 Quantum GIS 2.12 or high (necessarily only for MS Windows! For other -- optional).

7. Installed x86 GRASS 6.4.4 and copy folder "GRASS 6.4.4\msys" into folder "QGIS\apps\" (necessarily only for MS Windows and QGIS 2.16.x!).

Minimum requires packages for GNU/Linux, Mac OS X, FreeBSD: 
   bash 4, gdal 1.11, gdal-libs 1.11, gdal-python 1.11, 
   libgeotiff 1.4, libtiff 4.0.3, libpng 1.6, lcms2 2.7, 
   zlib 1.2, libxml2 2.9, OpenJDK 1.8.x, 
   gvfs (GNU/Linux, FreeBSD).

ATTENTION! If you're using ESRI ArcGIS and QGIS in the same Windows-version, you may experience incorrect operation python-gdal-modules and QGIS!

### RSUtils: building in GNU/Linux, Mac OS X, FreeBSD

1.Add to PATH the location jdk/bin directory.

2.Add to PATH the location RSUtils/bin directory and library. Create system variable 
RSUTILS_HOME and set her the value in path to RSUtils/bin directory.

For example in GNU/Linux (add to $HOME/.bashrc): 

	export RSUTILS_HOME=/home/username/bin/RSUtils/bin
	export PATH=$PATH:$RSUTILS_HOME
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$RSUTILS_HOME/lib
	
For example in Mac OS X (add to $HOME/.bash_profile): 

	export RSUTILS_HOME=/Users/username/Documents/bin/RSUtils/bin
	export GDALPROGS=/Library/Frameworks/GDAL.framework/Versions/Current/Programs
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$RSUTILS_HOME/lib
	export PATH=$PATH:$GDALPROGS:$RSUTILS_HOME
	
For FreeBSD -- setup locale with UTF-8 support; support GEOS in GDAL library must be enabled!!!

3.Execute from console file build.sh (bash ./build.sh). To clean the build directory to use the file clear.sh (bash ./clear.sh).


### RSUtils: building in Microsoft Windows

1.Add to PATH the location jdk\bin directory.

2.Create system variables QGIS_HOME, QGIS_PYTHON_HOME and set the values in path to QGIS 2.12 (or high version) directory.

For example: 

	QGIS_HOME=C:\QGIS

	QGIS_PYTHON_HOME=C:\QGIS\apps\Python27

3.Add to PATH the location RSUtils\bin directory and library. Create system variable 
RSUTILS_HOME and set her the value in path to RSUtils\bin directory.

For example: 

	RSUTILS_HOME=C:\RSUtils\bin

Add to PATH-variable (example):

	%RSUTILS_HOME%\bin;%RSUTILS_HOME%\lib;%QGIS_HOME%\bin;%QGIS_HOME%\apps\msys\bin;%QGIS_PYTHON_HOME%;%PATH%
	
4.Start Windows SDK Command Prompt. Go to the directory RSUtils and execute file 
build_msvc.cmd (for MS Visual C++ compiler) or build_mingw32.cmd (for MinGW-32 GNU GCC).
To clean the build directory to use the file clear_msvc.cmd or clear_mingw32.cmd (depending on the build).
