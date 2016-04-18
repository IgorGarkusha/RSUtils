#!/bin/bash
echo ""
echo "START BUILD..."
echo ""

MACOS=`uname -a | awk '{print $1;}' | grep Darwin`
FREEBSD=`uname -a | awk '{print $1;}' | grep FreeBSD`

if [ "-"${FREEBSD} == "-FreeBSD" ]; then 
	# Build for FreeBSD
	export PLATFORM=bsd
	mkdir ./objects
	# make -f ./makefiles/makefile
elif [ "-"${MACOS} == "-Darwin" ]; then 
	# Build for MAC OS X
	export PLATFORM=mac
	mkdir ./obj
	# make -f ./makefiles/makefile
else
	# Build for GNU/Linux
	export PLATFORM=lnx
	mkdir ./obj
	# make -f ./makefiles/makefile
fi

mkdir ./bin
make -f ./makefiles/makefile

cd ./makefiles/
sh ./javabuild.sh

cd ..

echo ""
echo "./bin:"
ls -lh ./bin/
echo ""
echo ""
echo "BUILD COMPLETE."
echo ""
