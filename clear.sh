#!/bin/bash
CUR_DIR=`pwd`

OPJ_VER=2.1.1

MACOS=`uname -a | awk '{print $1;}' | grep Darwin`
FREEBSD=`uname -a | awk '{print $1;}' | grep FreeBSD`

if [ "-"${FREEBSD} == "-FreeBSD" ]; then 
    # Build for FreeBSD
    export PLATFORM=bsd
elif [ "-"${MACOS} == "-Darwin" ]; then 
    # Build for MAC OS X
    export PLATFORM=mac
else
    # Build for GNU/Linux
    export PLATFORM=lnx
fi

make -f ./makefiles/makefile clean

rm -f ./src/downloaders/lib/class/org/rsutils/downloader/*
rm -f ./src/downloaders/lib/class/org/rsutils/*
# rm -f ./src/downloaders/modis/modis_downloader/class/modis_downloader/*
rm -f ./src/downloaders/sentinel/s1a_grd_downloader/class/s1a_grd_downloader/*
rm -f ./src/downloaders/sentinel/s1a_slc_downloader/class/s1a_slc_downloader/*
rm -f ./src/downloaders/sentinel/s2a_tile_downloader/class/s2a_tile_downloader/*
rm -f ./src/satellite_coverage/org/rsutils/mapviewer/*.class

cd $CUR_DIR
rm -Rf ./bin
rm -Rf ./obj
rm -Rf ./objects

rm -Rf ./external/openjpeg/openjpeg-${OPJ_VER}

echo ""
echo "CLEAR COMPLETE."
echo ""
