#!/bin/bash

echo ""
echo "BUILD RSUTILS_DOWNLOADER LIBRARY..."
echo ""

rm -f ../src/downloaders/lib/class/org/rsutils/downloader/*.class
rm -f ../src/downloaders/lib/class/org/rsutils/*.class

cd ../src/downloaders/lib/src/org/rsutils/downloader/

javac -Xlint -d ../../../../class/ -cp ../../../../src:. *
cd ..
javac -Xlint -d ../../../class/ -cp ../../../src:. *.java
cp ./gplv3-127x51.png ../../../class/org/rsutils/
cd ../../../
jar cf ../../../bin/librsutils_downloader.jar -C ./class .

# echo ""
# echo "BUILD MODIS_DOWNLOADER..."
# echo ""

# rm -f ../modis/modis_downloader/class/modis_downloader/*.class
# cd ../modis/modis_downloader/src/modis_downloader/

# javac -Xlint -d ../../class/ -cp ../../src:../../../../../../bin/librsutils_downloader.jar:. *.java
# cd ../../
# jar cf ../../../../bin/modis_downloader.jar -C ./class .

# cd ../../

cd ..

echo ""
echo "BUILD S1A_GRD_DOWNLOADER..."
echo ""

rm -Rf ./sentinel/s1a_grd_downloader/class/*
cd ./sentinel/s1a_grd_downloader/src/s1a_grd_downloader/

javac -Xlint -d ../../class/ -cp ../../src:../../../../../../bin/librsutils_downloader.jar:. *.java
cd ../../
jar cf ../../../../bin/s1a_grd_downloader.jar -C ./class .

cd ../

echo ""
echo "BUILD S1A_SLC_DOWNLOADER..."
echo ""

rm -Rf ./s1a_slc_downloader/class/*
cd ./s1a_slc_downloader/src/s1a_slc_downloader/

javac -Xlint -d ../../class/ -cp ../../src:../../../../../../bin/librsutils_downloader.jar:. *.java
cd ../../
jar cf ../../../../bin/s1a_slc_downloader.jar -C ./class .

cd ../

echo ""
echo "BUILD S2A_TILE_DOWNLOADER..."
echo ""

rm -Rf ./s2a_tile_downloader/class/*
cd ./s2a_tile_downloader/src/s2a_tile_downloader/

javac -Xlint -d ../../class/ -cp ../../src:../../../../../../bin/librsutils_downloader.jar:. *.java
cd ../../
jar cf ../../../../bin/s2a_tile_downloader.jar -C ./class .

cd ../../../../

echo ""
echo "BUILD SATELLITE_COVERAGE..."
echo ""

cd ./src/satellite_coverage/org/rsutils/mapviewer
javac AboutDialog.java
javac -cp ../../../lib/JMapViewer.jar:../../../:. SatelliteCoverage.java
cd ../../../
jar cfm ../../bin/SatelliteCoverage.jar manifest.mf org data
mkdir ../../bin/lib
cp -f ./lib/JMapViewer.jar ../../bin/lib/
cd ../../

echo ""
echo "CREATE START SCRIPTS..."
echo ""

# export RSUTILS_HOME=`pwd`/bin

# echo "#!"$SHELL > ./bin/modis_downloader
# echo 'java -cp "${RSUTILS_HOME}"/librsutils_downloader.jar:"${RSUTILS_HOME}"/modis_downloader.jar:. modis_downloader.modis_download $1' >> ./bin/modis_downloader
# echo "" >> ./bin/modis_downloader
# chmod +x ./bin/modis_downloader

echo "#!"$SHELL > ./bin/s1a_grd_downloader
echo 'java -cp "${RSUTILS_HOME}"/librsutils_downloader.jar:"${RSUTILS_HOME}"/s1a_grd_downloader.jar:. s1a_grd_downloader.s1a_grd_download $1' >> ./bin/s1a_grd_downloader
echo "" >> ./bin/s1a_grd_downloader
chmod +x ./bin/s1a_grd_downloader

echo "#!"$SHELL > ./bin/s1a_slc_downloader
echo 'java -cp "${RSUTILS_HOME}"/librsutils_downloader.jar:"${RSUTILS_HOME}"/s1a_slc_downloader.jar:. s1a_slc_downloader.s1a_slc_download $1' >> ./bin/s1a_slc_downloader
echo "" >> ./bin/s1a_slc_downloader
chmod +x ./bin/s1a_slc_downloader

echo "#!"$SHELL > ./bin/s2a_tile_downloader
echo 'java -cp "${RSUTILS_HOME}"/librsutils_downloader.jar:"${RSUTILS_HOME}"/s2a_tile_downloader.jar:. s2a_tile_downloader.s2_tile_download $1' >> ./bin/s2a_tile_downloader
echo "" >> ./bin/s2a_tile_downloader
chmod +x ./bin/s2a_tile_downloader

echo "#!"$SHELL > ./bin/satellite_coverage
echo 'java -jar "${RSUTILS_HOME}"/SatelliteCoverage.jar' >> ./bin/satellite_coverage
echo "" >> ./bin/satellite_coverage
chmod +x ./bin/satellite_coverage
