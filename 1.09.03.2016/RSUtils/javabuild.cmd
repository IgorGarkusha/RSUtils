@echo off

echo.
echo BUILD RSUTILS_DOWNLOADER LIBRARY...
echo.

cd source\downloaders\lib\src\org\rsutils\downloader\

javac -Xlint -d ..\..\..\..\class\ -cp ..\..\..\..\src;. *
cd ..
javac -Xlint -d ..\..\..\class\ -cp ..\..\..\src;. *.java
copy gplv3-127x51.png ..\..\..\class\org\rsutils\
cd ..\..\..\
jar cf ..\..\..\binary\target\librsutils_downloader.jar -C .\class .

echo.
echo BUILD MODIS_DOWNLOADER...
echo.

cd ..\modis\modis_downloader\src\modis_downloader\

javac -Xlint -d ..\..\class\ -cp ..\..\src;..\..\..\..\..\..\binary\target\librsutils_downloader.jar;. *.java
cd ..\..\
jar cf ..\..\..\..\binary\target\modis_downloader.jar -C .\class .

cd ..\..\

echo.
echo BUILD S1A_GRD_DOWNLOADER...
echo.

cd sentinel\s1a_grd_downloader\src\s1a_grd_downloader\

javac -Xlint -d ..\..\class\ -cp ..\..\src;..\..\..\..\..\..\binary\target\librsutils_downloader.jar;. *.java
cd ..\..\
jar cf ..\..\..\..\binary\target\s1a_grd_downloader.jar -C .\class .

cd ..\

echo.
echo BUILD S2A_TILE_DOWNLOADER...
echo.

cd s2a_tile_downloader\src\s2a_tile_downloader\

javac -Xlint -d ..\..\class\ -cp ..\..\src;..\..\..\..\..\..\binary\target\librsutils_downloader.jar;. *.java
cd ..\..\
jar cf ..\..\..\..\binary\target\s2a_tile_downloader.jar -C .\class .

cd ..\..\..\..\

echo.
echo CREATE START SCRIPTS...
echo.

echo @echo off >  binary\target\modis_downloader.cmd
echo java -cp %RSUTILS_HOME%\librsutils_downloader.jar;%RSUTILS_HOME%\modis_downloader.jar;. modis_downloader.modis_download %%* >> binary\target\modis_downloader.cmd
echo. >> binary\target\modis_downloader.cmd

echo @echo off > binary\target\s1a_grd_downloader.cmd
echo java -cp %RSUTILS_HOME%\librsutils_downloader.jar;%RSUTILS_HOME%\s1a_grd_downloader.jar;. s1a_grd_downloader.s1a_grd_download %%* >> binary\target\s1a_grd_downloader.cmd
echo. >> binary\target\s1a_grd_downloader.cmd

echo @echo off > binary\target\s2a_tile_downloader.cmd
echo java -cp %RSUTILS_HOME%\librsutils_downloader.jar;%RSUTILS_HOME%\s2a_tile_downloader.jar;. s2a_tile_downloader.s2_tile_download %%* >> binary\target\s2a_tile_downloader.cmd
echo. >> binary\target\s2a_tile_downloader.cmd

