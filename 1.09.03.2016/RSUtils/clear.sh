#!/bin/bash
CUR_DIR=`pwd`
make -f ./makefile.unx clean

cd $CUR_DIR
rm -f ./binary/target/*

echo ""
echo "./binary/target:"
ls -lh ./binary/target
echo ""

rm -f ./source/downloaders/lib/class/org/rsutils/downloader/*.class
rm -f ./source/downloaders/lib/class/org/rsutils/*.class
rm -f ./source/downloaders/modis/modis_downloader/class/modis_downloader/*.class
rm -f ./source/downloaders/sentinel/s1a_grd_downloader/class/s1a_grd_downloader/*
rm -f ./source/downloaders/sentinel/s2a_tile_downloader/class/s2a_tile_downloader/*
