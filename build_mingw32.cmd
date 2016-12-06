@echo off

@rem !!! SET SYSTEM VARIABLES RSUTILS_HOME, QGIS_HOME, QGIS_PYTHON_HOME !!!
@rem Example Path for RSUTILS_HOME=C:\RSUtils\bin
@rem Example Path for QGIS_HOME=C:\QGIS
@rem Example Path for QGIS_PYTHON_HOME=C:\QGIS\apps\Python27

echo.
echo START BUILD OpenJPEG 2000 library...
echo.

sleep 3

set PATH=%PATH%;%QGIS_HOME%\bin;%QGIS_HOME%\apps\msys\bin
mv "%QGIS_HOME%"\apps\msys\bin\sh.exe "%QGIS_HOME%"\apps\msys\bin\-sh.exe

set OPJ_VER=2.1.2
set TIFFLIB="%QGIS_HOME%\bin\libtiff.dll"

mkdir bin
mkdir bin\lib

cd external\openjpeg\
rm -Rf openjpeg-%OPJ_VER%
tar -zxvf openjpeg-%OPJ_VER%.tar.gz
cd openjpeg-%OPJ_VER%

mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX:string=%RSUTILS_HOME% -DCMAKE_C_FLAGS:string=-O3 %TIFFLIB% -G "MinGW Makefiles" ..
mingw32-make
mingw32-make install

copy ..\..\opj_decompress_jp2.cmd "%RSUTILS_HOME%"\opj_decompress_jp2.cmd

if not exist "%RSUTILS_HOME%"\bin\opj_decompress.exe (
    echo BUILD ERROR. FILE NOT FOUND: %RSUTILS_HOME%\bin\opj_decompress.exe not found!
    goto exit
)

copy "%RSUTILS_HOME%"\bin\opj_decompress.exe "%RSUTILS_HOME%"\opj_decompress.exe

cd ../../../../

echo.
echo START BUILD RSUtils...
echo.

sleep 3

mkdir bin
mkdir obj
set PLATFORM=mingw32
mingw32-make.exe -f makefiles\makefile

cd makefiles
call javabuild.cmd

mkdir bin\html
copy src\html\* bin\html\*

mkdir bin\palette
copy src\calc_index\ndvi_pal.txt bin\palette\*

copy external\create_stack.cmd bin\create_stack.cmd

mv "%QGIS_HOME%"\apps\msys\bin\-sh.exe "%QGIS_HOME%"\apps\msys\bin\sh.exe

echo.
dir bin\
echo.
echo.
echo BUILD COMPLETE.
echo.

:exit
