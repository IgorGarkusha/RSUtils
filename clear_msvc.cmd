@echo off

set PATH=%PATH%;%QGIS_HOME%\bin;%QGIS_HOME%\apps\msys\bin

mv "%QGIS_HOME%"\apps\msys\bin\sh.exe "%QGIS_HOME%"\apps\msys\bin\-sh.exe

set OPJ_VER=2.1.2

set PLATFORM=msvc
nmake.exe -f makefiles\makefile clean

del /Q src\downloaders\lib\class\org\rsutils\downloader\*
del /Q src\downloaders\lib\class\org\rsutils\*
@rem del /Q src\downloaders\modis\modis_downloader\class\modis_downloader\*
del /Q src\downloaders\sentinel\s1a_grd_downloader\class\s1a_grd_downloader\*
del /Q src\downloaders\sentinel\s1a_slc_downloader\class\s1a_slc_downloader\*
del /Q src\downloaders\sentinel\s2a_tile_downloader\class\s2a_tile_downloader\*
del /Q src\satellite_coverage\org\rsutils\mapviewer\*.class

rm -Rf bin
rm -Rf obj
rm -Rf external\openjpeg\openjpeg-%OPJ_VER%

mv "%QGIS_HOME%"\apps\msys\bin\-sh.exe "%QGIS_HOME%"\apps\msys\bin\sh.exe

echo.
dir bin
echo.
echo.
echo CLEAR COMPLETE.
echo.
