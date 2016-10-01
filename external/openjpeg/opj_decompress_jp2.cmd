@echo off
set PATH=%PATH%;%RSUTILS_HOME%\lib
if -%1 == - goto error
if -%2 == - goto error
"%RSUTILS_HOME%"\opj_decompress.exe -i %1 -o %2
goto exit
:error
"%RSUTILS_HOME%"\opj_decompress
echo Example: %RSUTILS_HOME%\opj_decompress_jp2 input.jp2 output.tif
:exit
