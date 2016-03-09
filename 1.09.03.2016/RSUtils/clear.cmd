@echo off
nmake.exe -f makefile.win32 clean

del /Q binary\target\*
del /Q source\downloaders\lib\class\org\rsutils\downloader\*.class
del /Q source\downloaders\lib\class\org\rsutils\*.class
del /Q source\downloaders\modis\modis_downloader\class\modis_downloader\*.class
del /Q source\downloaders\sentinel\s1a_grd_downloader\class\*
del /Q source\downloaders\sentinel\s2a_tile_downloader\class\*

echo.
dir binary\target
echo.
