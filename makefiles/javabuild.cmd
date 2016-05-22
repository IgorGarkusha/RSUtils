@echo off

echo.
echo BUILD RSUTILS_DOWNLOADER LIBRARY...
echo.

cd ..\src\downloaders\lib\src\org\rsutils\downloader\

javac -Xlint -d ..\..\..\..\class\ -cp ..\..\..\..\src;. *
cd ..
javac -Xlint -d ..\..\..\class\ -cp ..\..\..\src;. *.java
copy gplv3-127x51.png ..\..\..\class\org\rsutils\
cd ..\..\..\
jar cf ..\..\..\bin\librsutils_downloader.jar -C .\class .

echo.
echo BUILD MODIS_DOWNLOADER...
echo.

cd ..\modis\modis_downloader\src\modis_downloader\

javac -Xlint -d ..\..\class\ -cp ..\..\src;..\..\..\..\..\..\bin\librsutils_downloader.jar;. *.java
cd ..\..\
jar cf ..\..\..\..\bin\modis_downloader.jar -C .\class .

cd ..\..\

echo.
echo BUILD S1A_GRD_DOWNLOADER...
echo.

cd sentinel\s1a_grd_downloader\src\s1a_grd_downloader\

javac -Xlint -d ..\..\class\ -cp ..\..\src;..\..\..\..\..\..\bin\librsutils_downloader.jar;. *.java
cd ..\..\
jar cf ..\..\..\..\bin\s1a_grd_downloader.jar -C .\class .

cd ..\

echo.
echo BUILD S1A_SLC_DOWNLOADER...
echo.

cd s1a_slc_downloader\src\s1a_slc_downloader\

javac -Xlint -d ..\..\class\ -cp ..\..\src;..\..\..\..\..\..\bin\librsutils_downloader.jar;. *.java
cd ..\..\
jar cf ..\..\..\..\bin\s1a_slc_downloader.jar -C .\class .

cd ..\

echo.
echo BUILD S2A_TILE_DOWNLOADER...
echo.

cd s2a_tile_downloader\src\s2a_tile_downloader\

javac -Xlint -d ..\..\class\ -cp ..\..\src;..\..\..\..\..\..\bin\librsutils_downloader.jar;. *.java
cd ..\..\
jar cf ..\..\..\..\bin\s2a_tile_downloader.jar -C .\class .

cd ..\..\..\..\

echo.
echo BUILD SATELLITE_COVERAGE...
echo.

cd src\satellite_coverage\org\rsutils\mapviewer
javac AboutDialog.java
javac -cp ..\..\..\lib\JMapViewer.jar;..\..\..\;. SatelliteCoverage.java
cd ..\..\..\
jar cfm ..\..\bin\SatelliteCoverage.jar manifest.mf org data
mkdir ..\..\bin\lib
copy lib\JMapViewer.jar ..\..\bin\lib\
cd ..\..\

echo.
echo CREATE START SCRIPTS...
echo.

echo @echo off >  bin\modis_downloader.cmd
echo java -cp %RSUTILS_HOME%\librsutils_downloader.jar;%RSUTILS_HOME%\modis_downloader.jar;. modis_downloader.modis_download %%* >> bin\modis_downloader.cmd
echo. >> bin\modis_downloader.cmd

echo @echo off > bin\s1a_grd_downloader.cmd
echo java -cp %RSUTILS_HOME%\librsutils_downloader.jar;%RSUTILS_HOME%\s1a_grd_downloader.jar;. s1a_grd_downloader.s1a_grd_download %%* >> bin\s1a_grd_downloader.cmd
echo. >> bin\s1a_grd_downloader.cmd

echo @echo off > bin\s1a_slc_downloader.cmd
echo java -cp %RSUTILS_HOME%\librsutils_downloader.jar;%RSUTILS_HOME%\s1a_slc_downloader.jar;. s1a_slc_downloader.s1a_slc_download %%* >> bin\s1a_slc_downloader.cmd
echo. >> bin\s1a_slc_downloader.cmd

echo @echo off > bin\s2a_tile_downloader.cmd
echo java -cp %RSUTILS_HOME%\librsutils_downloader.jar;%RSUTILS_HOME%\s2a_tile_downloader.jar;. s2a_tile_downloader.s2_tile_download %%* >> bin\s2a_tile_downloader.cmd
echo. >> bin\s2a_tile_downloader.cmd

echo @echo off > bin\satellite_coverage.cmd
echo java -jar %RSUTILS_HOME%\SatelliteCoverage.jar >> bin\satellite_coverage.cmd
echo. >> bin\satellite_coverage.cmd
