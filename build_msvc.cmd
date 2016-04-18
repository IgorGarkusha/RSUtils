@echo off

@rem !!! SET SYSTEM VARIABLES RSUTILS_HOME and QGIS_HOME !!!
@rem Example Path for RSUTILS_HOME=C:\RSUtils\bin
@rem Example Path for QGIS_HOME=C:\QGIS

echo.
echo START BUILD...
echo.
mkdir bin
mkdir obj
set PLATFORM=msvc
nmake.exe -f makefiles\makefile

cd makefiles
call javabuild.cmd

echo.
dir bin\
echo.
echo.
echo BUILD COMPLETE.
echo.
