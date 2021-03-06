## RSUtils. Version 2.06.12.2016

1. Add NMDI-index to calc_index and calc_index_stack.
2. Fix s2a_tile_downloader, tcap.
3. Include support OpenJPEG 2000 Library ver.2.1.2.

## RSUtils. Version 2.24.09.2016

1. Fixed many bugs.
2. Add new program: l8_dn_batch_convert.
3. Big update program: s2a_tile_downloader (include AWS-support!).
4. Update modules: l8_dn_conversion.cpp, check_reflectance.cpp.
5. Update downloaders, libraries.
6. Include support OpenJPEG 2000 Library ver.2.1.1.
7. Temporarily removed from the package modis_downloader.

## RSUtils. Version 1.17.07.2016

1. Add new program: calc_index_stack.
2. New features added to the programs: calc_index, l8_dn_conversion.
3. Update modules: utils.h, utils.cpp, calc_index.cpp, l8_dn_conversion.cpp.

## RSUtils. Version 1.10.07.2016

1. Add new programs: tcap, stat_map_creator.
2. Fix bug in module: utils.cpp.
3. Update modules: utils.h, utils.cpp, calc_index.cpp (add indexes: WDRVI, VARI, LAI(based on EVI)), tif2con.cpp (calculate statistics in various modes).

## RSUtils. Version 1.22.05.2016

#### Update 06/01/2016

1. Has been updated coverage scheme Sentinel-2A.

#### Update 05/31/2016

1. Fix bugs in module SatelliteCoverage.java.
2. Has been updated coverage schemes WRS-2 DESCENDING and MODIS Sinusoidal tiles.

#### Update 05/24/2016

1. Has been updated module Utils.java: added the ability to properly run the satellite_coverage in Windows from external programs.

## RSUtils. Version 1.22.05.2016

1. Add new programs: satellite_coverage, s1a_slc_downloader.
2. Fix bugs in modules: ModisDownloadFrame.java, calc_index.cpp.
3. Update downloaders (add satellite coverage scheme): modis_downloader, s2a_tile_downloader.

## RSUtils. Version 1.17.04.2016

#### Update 04/21/2016

1. Has been updates build scripts for GNU/Linux. Enable support build for Ubuntu. Tested under Ubuntu 14.04, Fedora 23.
2. Update modis_downloader, s1a_grd_downloader, s2a_tile_downloader: fix window size on some platforms.

#### Update 04/20/2016

Updated directory structure for build and added tutorials.

## RSUtils. Version 1.17.04.2016

1. Has been updates build scripts for various platform (support build for: GNU/Linux GNU GCC, Mac OS X GNU GCC, FreeBSD Clang-compiler, MS Windows MS Visual C++-compiler, MS Windows MinGW-32 GNU GCC).
2. Improved build system.
3. Added two additional modes in the program check_reflectance.
4. Added new programs: l8_dn_conversion, calc_index, tif2con.

## RSUtils. Version 1.09.03.2016

#### Update 03/19/2016

Fix bugs in module FileDownload.java

#### Update 03/14/2016

Has been updates build scripts for various platform
