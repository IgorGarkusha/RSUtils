@echo off

@rem !!! SET SYSTEM VARIABLES RSUTILS_HOME and QGIS_HOME !!!
@rem Example Path for RSUTILS_HOME=C:\RSUtils\binary\target
@rem Example Path for QGIS_HOME=C:\QGIS

echo.
echo BUILD S2_SET_PROJECTION and CHECK_REFLECTANCE...
echo.
nmake.exe -f makefile.win32

call javabuild.cmd

echo.
dir binary\target\
echo.
echo.
echo BUILD COMPLETE.
echo.
