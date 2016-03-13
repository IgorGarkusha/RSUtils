#!/bin/bash
echo ""
echo "BUILD S2_SET_PROJECTION and CHECK_REFLECTANCE..."
echo ""

MACOS=`uname -a | awk '{print $1;}' | grep Darwin`
FREEBSD=`uname -a | awk '{print $1;}' | grep FreeBSD`

if [ "-"${FREEBSD} == "-FreeBSD" ]; then 
	# Build for FreeBSD
	make -f ./makefile.bsd
elif [ "-"${MACOS} == "-Darwin" ]; then 
	# Build for MAC OS X
	make -f ./makefile.mac
else
	# Build for GNU/Linux
	make -f ./makefile.unx
fi

echo ""
echo "BUILD RSUTILS_DOWNLOADER LIBRARY..."
echo ""

rm -f ./source/downloaders/lib/class/org/rsutils/downloader/*.class
rm -f ./source/downloaders/lib/class/org/rsutils/*.class

cd ./source/downloaders/lib/src/org/rsutils/downloader/

javac -Xlint -d ../../../../class/ -cp ../../../../src:. *
cd ..
javac -Xlint -d ../../../class/ -cp ../../../src:. *.java
cp ./gplv3-127x51.png ../../../class/org/rsutils/
cd ../../../
jar cf ../../../binary/target/librsutils_downloader.jar -C ./class .

echo ""
echo "BUILD MODIS_DOWNLOADER..."
echo ""

rm -f ../modis/modis_downloader/class/modis_downloader/*.class
cd ../modis/modis_downloader/src/modis_downloader/

javac -Xlint -d ../../class/ -cp ../../src:../../../../../../binary/target/librsutils_downloader.jar:. *.java
cd ../../
jar cf ../../../../binary/target/modis_downloader.jar -C ./class .

cd ../../

echo ""
echo "BUILD S1A_GRD_DOWNLOADER..."
echo ""

rm -Rf ./sentinel/s1a_grd_downloader/class/*
cd ./sentinel/s1a_grd_downloader/src/s1a_grd_downloader/

javac -Xlint -d ../../class/ -cp ../../src:../../../../../../binary/target/librsutils_downloader.jar:. *.java
cd ../../
jar cf ../../../../binary/target/s1a_grd_downloader.jar -C ./class .

cd ../

echo ""
echo "BUILD S2A_TILE_DOWNLOADER..."
echo ""

rm -Rf ./s2a_tile_downloader/class/*
cd ./s2a_tile_downloader/src/s2a_tile_downloader/

javac -Xlint -d ../../class/ -cp ../../src:../../../../../../binary/target/librsutils_downloader.jar:. *.java
cd ../../
jar cf ../../../../binary/target/s2a_tile_downloader.jar -C ./class .

cd ../../../../

echo ""
echo "CREATE START SCRIPTS..."
echo ""

# export RSUTILS_HOME=`pwd`/binary/target

echo "#!"$SHELL > ./binary/target/modis_downloader
echo "java -cp ${RSUTILS_HOME}/librsutils_downloader.jar:${RSUTILS_HOME}/modis_downloader.jar:. modis_downloader.modis_download \$1" >> ./binary/target/modis_downloader
echo "" >> ./binary/target/modis_downloader
chmod +x ./binary/target/modis_downloader

echo "#!"$SHELL > ./binary/target/s1a_grd_downloader
echo "java -cp ${RSUTILS_HOME}/librsutils_downloader.jar:${RSUTILS_HOME}/s1a_grd_downloader.jar:. s1a_grd_downloader.s1a_grd_download \$1" >> ./binary/target/s1a_grd_downloader
echo "" >> ./binary/target/s1a_grd_downloader
chmod +x ./binary/target/s1a_grd_downloader

echo "#!"$SHELL > ./binary/target/s2a_tile_downloader
echo "java -cp ${RSUTILS_HOME}/librsutils_downloader.jar:${RSUTILS_HOME}/s2a_tile_downloader.jar:. s2a_tile_downloader.s2_tile_download \$1" >> ./binary/target/s2a_tile_downloader
echo "" >> ./binary/target/s2a_tile_downloader
chmod +x ./binary/target/s2a_tile_downloader


echo ""
echo "./binary/target:"
ls -lh ./binary/target/
echo ""
echo ""
echo "BUILD COMPLETE."
echo ""
