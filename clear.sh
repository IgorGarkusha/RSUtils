#!/bin/bash
CUR_DIR=`pwd`

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
rm -f ./src/downloaders/modis/modis_downloader/class/modis_downloader/*
rm -f ./src/downloaders/sentinel/s1a_grd_downloader/class/s1a_grd_downloader/*
rm -f ./src/downloaders/sentinel/s2a_tile_downloader/class/s2a_tile_downloader/*

cd $CUR_DIR
rm -f ./bin/*

echo ""
echo "./bin:"
ls -lh ./bin
echo ""
echo ""
echo "CLEAR COMPLETE."
echo ""
