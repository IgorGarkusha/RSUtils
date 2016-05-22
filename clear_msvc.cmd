@echo off
set PLATFORM=msvc
nmake.exe -f makefiles\makefile clean

del /Q bin\lib\*
del /Q bin\*

del /Q src\downloaders\lib\class\org\rsutils\downloader\*
del /Q src\downloaders\lib\class\org\rsutils\*
del /Q src\downloaders\modis\modis_downloader\class\modis_downloader\*
del /Q src\downloaders\sentinel\s1a_grd_downloader\class\s1a_grd_downloader\*
del /Q src\downloaders\sentinel\s1a_slc_downloader\class\s1a_slc_downloader\*
del /Q src\downloaders\sentinel\s2a_tile_downloader\class\s2a_tile_downloader\*
del /Q src\satellite_coverage\org\rsutils\mapviewer\*.class

echo.
dir bin
echo.
echo.
echo CLEAR COMPLETE.
echo.
