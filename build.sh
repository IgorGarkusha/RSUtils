#!/bin/bash

OPJ_VER=2.1.1

mkdir ./bin
mkdir ./bin/lib

MACOS=`uname -a | awk '{print $1;}' | grep Darwin`
FREEBSD=`uname -a | awk '{print $1;}' | grep FreeBSD`

PLATFORM=""

if [ "-"${FREEBSD} == "-FreeBSD" ]; then 
	# Build for FreeBSD
	export PLATFORM=bsd
	mkdir ./objects
	PLATFORMBUILD="freebsd"
	TIFFLIB=""
elif [ "-"${MACOS} == "-Darwin" ]; then 
	# Build for MAC OS X
	export PLATFORM=mac
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${RSUTILS_HOME}/lib
	export PATH=$PATH:$LD_LIBRARY_PATH:/Applications/CMake.app/Contents/bin
	mkdir ./obj
	PLATFORMBUILD="macosx"

	if [ ! -f /Library/Frameworks/UnixImageIO.framework/Versions/E/unix/lib/libtiff.dylib ]; then
		echo "BUILD ERROR. FILE NOT FOUND: /Library/Frameworks/UnixImageIO.framework/Versions/E/unix/lib/libtiff.dylib"
		exit 1
	fi

	TIFFLIB=-DTIFF_LIBRARY:string=/Library/Frameworks/UnixImageIO.framework/Versions/E/unix/lib/libtiff.dylib
else
	# Build for GNU/Linux
	export PLATFORM=lnx
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${RSUTILS_HOME}/lib
	export PATH=$PATH:$LD_LIBRARY_PATH
	mkdir ./obj
	PLATFORMBUILD="linux"
	TIFFLIB=""
fi

echo ""
echo "START BUILD OpenJPEG 2000 library..."
echo ""

sleep 3

tar -zxvf ./external/openjpeg/openjpeg-${OPJ_VER}.tar.gz -C ./external/openjpeg/
cd ./external/openjpeg/openjpeg-${OPJ_VER}

rm -Rf ./build
mkdir ./build
cd ./build

cmake -DCMAKE_INSTALL_PREFIX:string=${RSUTILS_HOME} -DCMAKE_C_FLAGS:string=-O3 ${TIFFLIB} ..

make && make install

cp ../../opj_decompress_jp2 ${RSUTILS_HOME}/
chmod +x ${RSUTILS_HOME}/opj_decompress_jp2

if [ ! -f ${RSUTILS_HOME}/bin/opj_decompress ]; then
	echo "BUILD ERROR. FILE NOT FOUND: "${RSUTILS_HOME}"/bin/opj_decompress"
	exit 1
fi

cp ${RSUTILS_HOME}/bin/opj_decompress ${RSUTILS_HOME}/

cd ../../../../

echo ""
echo "START BUILD RSUtils..."
echo ""

sleep 3

make -f ./makefiles/makefile

cd ./makefiles/
sh ./javabuild.sh

cd ..

mkdir ./bin/html
cp ./src/html/* ./bin/html/

mkdir ./bin/palette
cp ./src/calc_index/ndvi_pal.txt ./bin/palette/

echo ""
echo "./bin:"
ls -lh ./bin/
echo ""
echo ""
echo "BUILD COMPLETE."
echo ""
