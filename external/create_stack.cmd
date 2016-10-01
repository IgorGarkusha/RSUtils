@echo off
set PYTHONPATH=%PYTHONPATH%;%QGIS_PYTHON_HOME%\Lib\site-packages\osgeo
if -%1 == - goto error
if -%2 == - goto error
if -%3 == - goto error
%QGIS_HOME%\bin\gdal_merge.py -separate -o %*
goto exit
:error
echo Example: create_stack.cmd OutputMultiBandImage.tif Band1.tif Band2.tif ... BandN.tif
:exit
