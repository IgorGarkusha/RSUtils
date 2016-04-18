/*
 * Project: Remote Sensing Utilities (Extentions GDAL/OGR)
 * Author:  Igor Garkusha <igor_garik@ua.fm>
 *          Ukraine, Dnipropetrovsk
 * 
 * Copyright (C) 2016, Igor Garkusha <igor_garik@ua.fm>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 * 
*/

#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <gdal.h>
#include <ogrsf_frmts.h>
#include <ogr_core.h>
#include <ogr_api.h>

#ifndef MAX_PATH
#define MAX_PATH	512
#endif

#define START_PROGRESS	-1
#define END_PROGRESS	-2

typedef unsigned char byte;

typedef union _tagTransferCINT16 
{
	int data;
	short sdata[2];
}TRANSFER_CINT16;

class CUtils
{
	private:
		CUtils();
	public:
		~CUtils();
		static size_t sizeofType(GDALDataType data_type);
		static void* mallocData(GDALRasterBandH hBand, void * pdata, int count);
		static float getDataAsFloat(GDALRasterBandH hBand, const void * pdata, int index);
		static void setData(float v, GDALRasterBandH hBand, const void * pdata, int index);
		static void getComplexDataAsDouble(GDALRasterBandH hBand, const void * pdata, int index, double * pReal, double * pImaginary);
		static void getRasterLine(GDALRasterBandH hBand, int current_row, int cols, void* pLineValues);
		static void setRasterLine(GDALRasterBandH hBand, int current_row, int cols, void* pLineValues);
		static void getRasterBlock(GDALRasterBandH hBand, int rows, int cols, void* pBlockValues);
		static void setRasterBlock(GDALRasterBandH hBand, int rows, int cols, void* pBlockValues);
		static int progress_ln(FILE* stream, int index, int count, int oldpersent);
		static int progress_ln_ex(FILE* stream, int index, int count, int progress);
		static void createNewFloatGeoTIFF(const char* fileName, int bands, int rows, int cols, double adfGeoTransform[6], char szProjection[512], float fillData, float noDataValue);
		static void createNewByteGeoTIFF(const char* fileName, int bands, int rows, int cols, double adfGeoTransform[6], char szProjection[512], byte fillData, byte noDataValue);
		static void calculateFloatGeoTIFFStatistics(GDALDatasetH hDataset, int userBandNumber, bool flNoDataValueAsBackground=true, float NoDataValue=-9999);
		static void calculateByteGeoTIFFStatistics(GDALDatasetH hDataset, int userBandNumber, byte flNoDataValueAsBackground, byte NoDataValue);
		static float getFloatNoDataValueAsBackground(GDALRasterBandH hBand);
		static bool insideInPolygons(OGRDataSourceH poDS, double x, double y);
		static bool isFloat32DataType(GDALDatasetH hDataset);
};
#endif
