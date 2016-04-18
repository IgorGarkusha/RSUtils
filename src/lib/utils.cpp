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

#include "utils.h"
#include <gdal.h>
#include <ogrsf_frmts.h>
#include <ogr_core.h>
#include <ogr_api.h>
#include <cpl_conv.h>
#include <math.h>

CUtils::CUtils(){}
CUtils::~CUtils(){}

size_t CUtils::sizeofType(GDALDataType data_type)
{
	switch(data_type)
	{
		case GDT_Float32: 	return sizeof(float);
		case GDT_Int32:		return sizeof(int);
		case GDT_UInt32:	return sizeof(unsigned int);
		case GDT_Int16:		return sizeof(short int);
		case GDT_UInt16:	return sizeof(unsigned short int);
		case GDT_Byte:		return sizeof(unsigned char);
		case GDT_CInt16:	return sizeof(int);
		default: 			return 0;
	}
}

void* CUtils::mallocData(GDALRasterBandH hBand, void * pdata, int count)
{
	switch(GDALGetRasterDataType(hBand))
	{
		case GDT_Float32:
			pdata = CPLMalloc(sizeof(float)*count);
			break;
		case GDT_Int32:
			pdata = CPLMalloc(sizeof(int)*count);
			break;
		case GDT_UInt32:
			pdata = CPLMalloc(sizeof(unsigned int)*count);
			break;
		case GDT_Int16:
			pdata = CPLMalloc(sizeof(short int)*count);
			break;
		case GDT_UInt16:
			pdata = CPLMalloc(sizeof(unsigned short int)*count);
			break;
		case GDT_Byte:
			pdata = CPLMalloc(sizeof(unsigned char)*count);
			break;
		case GDT_CInt16:
			pdata = CPLMalloc(sizeof(int)*count);
			break;
			
		//UNKNOWN
		default: pdata = NULL;
	}
	return pdata;
}

float CUtils::getDataAsFloat(GDALRasterBandH hBand, const void * pdata, int index)
{
	float * pFloat = NULL;
	int * pInt = NULL;
	unsigned int * pUInt = NULL;
	short int * pShort = NULL;
	unsigned short int * pUShort = NULL;
	unsigned char * pByte = NULL;
	float v = 0;
	TRANSFER_CINT16 tr = {0};
	
	switch((int)GDALGetRasterDataType(hBand))
	{
		case GDT_Float32:
			pFloat = (float*)pdata;
			v = pFloat[index];
			return v;

		case GDT_Int32:
			pInt = (int*)pdata;
			v = (float)pInt[index];
			return v;

		case GDT_UInt32:
			pUInt = (unsigned int*)pdata;
			v = (float)pUInt[index];
			return v;

		case GDT_Int16:
			pShort = (short int*)pdata;
			v = (float)pShort[index];
			return v;

		case GDT_UInt16:
			pUShort = (unsigned short int*)pdata;
			v = (float)pUShort[index];
			return v;

		case GDT_Byte:
			pByte = (unsigned char*)pdata;
			v = (float)pByte[index];
			return v;

		case GDT_CInt16:
			pInt = (int*)pdata;			
			tr.data = pInt[index];
			return (float)tr.sdata[0];

		//UNKNOWN
		default: return 0;
	}

	return 0;
}

void CUtils::setData(float v, GDALRasterBandH hBand, const void * pdata, int index)
{
	float * pFloat = NULL;
	int * pInt = NULL;
	unsigned int * pUInt = NULL;
	short int * pShort = NULL;
	unsigned short int * pUShort = NULL;
	unsigned char * pByte = NULL;
	TRANSFER_CINT16 tr = {0};
	
	switch((int)GDALGetRasterDataType(hBand))
	{
		case GDT_Float32:
			pFloat = (float*)pdata;
			pFloat[index] = v;
			break;

		case GDT_Int32:
			pInt = (int*)pdata;
			pInt[index] = (int)v;
			break;

		case GDT_UInt32:
			pUInt = (unsigned int*)pdata;
			pUInt[index] = (unsigned int)v;
			break;

		case GDT_Int16:
			pShort = (short int*)pdata;
			pShort[index] = (short int)v;
			break;

		case GDT_UInt16:
			pUShort = (unsigned short int*)pdata;
			pUShort[index] = (unsigned short int)v;
			break;

		case GDT_Byte:
			pByte = (unsigned char*)pdata;
			pByte[index] = (unsigned char)v;
			break;

		case GDT_CInt16:
			pInt = (int*)pdata;			
			tr.sdata[0] = (short)v;
			pInt[index] = tr.data;
			break;
		//UNKNOWN
		//default:;
	}
}

void CUtils::getComplexDataAsDouble(GDALRasterBandH hBand, const void * pdata, int index, double * pReal, double * pImaginary)
{
	int * pInt = NULL;
	TRANSFER_CINT16 tr = {0};
	
	*pReal = 0 ;
	*pImaginary = 0;
	
	if(GDALGetRasterDataType(hBand) == GDT_CInt16)
	{
		pInt = (int*)pdata;			
		tr.data = pInt[index];
		*pReal = (double)tr.sdata[0];
		*pImaginary = (double)tr.sdata[1];
	}
}

void CUtils::getRasterLine(GDALRasterBandH hBand, int current_row, int cols, void* pLineValues)
{
	GDALRasterIO(hBand, GF_Read, 0, current_row, cols, 1, pLineValues, cols, 1, GDALGetRasterDataType(hBand), 0, 0);
}

void CUtils::setRasterLine(GDALRasterBandH hBand, int current_row, int cols, void* pLineValues)
{
	GDALRasterIO(hBand, GF_Write, 0, current_row, cols, 1, pLineValues, cols, 1, GDALGetRasterDataType(hBand), 0, 0);
}

void CUtils::getRasterBlock(GDALRasterBandH hBand, int rows, int cols, void* pBlockValues)
{
	GDALRasterIO(hBand, GF_Read, 0, 0, cols, rows, pBlockValues, cols, rows, GDALGetRasterDataType(hBand), 0, 0);
}

void CUtils::setRasterBlock(GDALRasterBandH hBand, int rows, int cols, void* pBlockValues)
{
	GDALRasterIO(hBand, GF_Write, 0, 0, cols, rows, pBlockValues, cols, rows, GDALGetRasterDataType(hBand), 0, 0);
}

int CUtils::progress_ln(FILE* stream, int index, int count, int oldpersent)
{
	double tmp = index*100.0f/(double)count;
	int pr = (int)(tmp);
	if((oldpersent)!= pr) fprintf(stream, "\rProgress: %d%%", pr);
	return pr;
}

int CUtils::progress_ln_ex(FILE* stream, int index, int count, int progress)
{
	if(progress == END_PROGRESS) { fprintf(stream, "\rProgress: 100%%\n"); return 0; }
	else
	if(progress == START_PROGRESS) return (count-1)/10;
	else
	if(index == progress) { fprintf(stream, "\rProgress: %d%%", (int)((double)progress*100.0/(double)(count-1))); return progress+( (count-1)/10 ); }
	else
	return progress;
}

void CUtils::createNewFloatGeoTIFF(const char* fileName, int bands, int rows, int cols, double adfGeoTransform[6], char szProjection[512], float fillData, float noDataValue)
{
	char **papszOptions = NULL;
    GDALDriverH hDriver;
    GDALRasterBandH hDataset;
	GDALRasterBandH hBand;
	if( (hDriver = GDALGetDriverByName("GTiff")) != NULL)
	{
        if( (hDataset = GDALCreate( hDriver, fileName, cols, rows, bands, GDT_Float32, papszOptions )) !=NULL )
		{
			GDALSetGeoTransform(hDataset, adfGeoTransform );
			GDALSetProjection(hDataset, szProjection );
			for(int band = 1; band<=bands; band++)
			{
				if( (hBand = GDALGetRasterBand(hDataset, band)) != NULL )
				{
					float *pline = (float *)CPLMalloc(sizeof(float)*cols);
					for(int i=0; i<cols; i++) pline[i] = fillData;

					for(int i=0; i<rows; i++) GDALRasterIO(hBand, GF_Write, 0, i, cols, 1, pline, cols, 1, GDT_Float32, 0, 0 );
					CPLFree(pline);

					GDALSetRasterNoDataValue(hBand, noDataValue);
				}
			}
			GDALClose(hDataset);
		}
	}
}

void CUtils::createNewByteGeoTIFF(const char* fileName, int bands, int rows, int cols, double adfGeoTransform[6], char szProjection[512], byte fillData, byte noDataValue)
{
	char **papszOptions = NULL;
    GDALDriverH hDriver;
    GDALRasterBandH hDataset;
	GDALRasterBandH hBand;
	if( (hDriver = GDALGetDriverByName("GTiff")) != NULL)
	{
        if( (hDataset = GDALCreate( hDriver, fileName, cols, rows, bands, GDT_Byte, papszOptions )) !=NULL )
		{
			GDALSetGeoTransform(hDataset, adfGeoTransform );
			GDALSetProjection(hDataset, szProjection );
			for(int band = 1; band<=bands; band++)
			{
				if( (hBand = GDALGetRasterBand(hDataset, band)) != NULL )
				{
					byte *pline = (byte *)CPLMalloc(sizeof(byte)*cols);
					for(int i=0; i<cols; i++) pline[i] = fillData;

					for(int i=0; i<rows; i++) GDALRasterIO(hBand, GF_Write, 0, i, cols, 1, pline, cols, 1, GDT_Byte, 0, 0 );
					CPLFree(pline);

					GDALSetRasterNoDataValue(hBand, noDataValue);
				}
			}
			GDALClose(hDataset);
		}
	}
}

void CUtils::calculateFloatGeoTIFFStatistics(GDALDatasetH hDataset, int userBandNumber, bool flNoDataValueAsBackground, float NoDataValue)
{
	fputs("\nCalculate statistics...\n", stderr);

	GDALRasterBandH hBand =  GDALGetRasterBand(hDataset, 1);
	int cols  = GDALGetRasterBandXSize(hBand);
	int rows  = GDALGetRasterBandYSize(hBand);
	int bands = GDALGetRasterCount(hDataset);
	
	float * pbuf = NULL;
	pbuf = (float *)CPLMalloc(sizeof(float)*cols);
	
	double min = 0, max = 0, mean = 0, stddev = 0;
    double summ = 0;
    int count = 0;
	
	for(int band=1; band<=bands; band++)
	{
		if(userBandNumber != -1) fprintf(stderr, "Band %d...\n", userBandNumber);
		else fprintf(stderr, "Band %d...\n", band);
		
		hBand =  GDALGetRasterBand(hDataset, band);		
		if(flNoDataValueAsBackground) NoDataValue = getFloatNoDataValueAsBackground(hBand);
		
		min = max = mean = stddev = summ = 0;
		count = 0;
		bool flFirst = true;
		
		int pr = CUtils::progress_ln_ex(stderr, 0, 0, START_PROGRESS);
		for(int i=0; i<rows; i++)
		{
			GDALRasterIO(hBand, GF_Read, 0, i, cols, 1, pbuf, cols, 1, GDT_Float32, 0, 0 );
			for(int j=0; j<cols; j++) if(pbuf[j]!=NoDataValue) 
			{
				if(flFirst)
				{
					mean = pbuf[j];
					min = max = mean;
					flFirst = false;
				}
				else
				{
					mean += pbuf[j];
					if( min > pbuf[j] ) min = pbuf[j];
					if( max < pbuf[j] ) max = pbuf[j];
				}
				count++;
			}
			pr = CUtils::progress_ln_ex(stderr, i, rows, pr);
		}
		CUtils::progress_ln_ex(stderr, 0, 0, END_PROGRESS);
		
		if(count>0)	mean /= (double)count;
		else mean = 0;
		
		pr = CUtils::progress_ln_ex(stderr, 0, 0, START_PROGRESS);
		for(int i=0; i<rows; i++)
		{
			GDALRasterIO(hBand, GF_Read, 0, i, cols, 1, pbuf, cols, 1, GDT_Float32, 0, 0 );
			for(int j=0; j<cols; j++) if(pbuf[j]!=NoDataValue) summ += (pbuf[j]-mean)*(pbuf[j]-mean);
						
			pr = CUtils::progress_ln_ex(stderr, i, rows, pr);
		}
		CUtils::progress_ln_ex(stderr, 0, 0, END_PROGRESS);
				
		summ = 0; stddev = 0;
		if((count-1)>0)
		{
			summ /= (double)(count-1);
			if(summ!=0) stddev = sqrt(summ);
		}
		
		GDALSetRasterStatistics(hBand, min, max, mean, stddev);
		GDALSetRasterNoDataValue(hBand, NoDataValue);
	}
	
	CPLFree(pbuf);
}

void CUtils::calculateByteGeoTIFFStatistics(GDALDatasetH hDataset, int userBandNumber, byte flNoDataValueAsBackground, byte NoDataValue)
{
	fputs("\nCalculate statistics...\n", stderr);

	GDALRasterBandH hBand =  GDALGetRasterBand(hDataset, 1);
	int cols  = GDALGetRasterBandXSize(hBand);
	int rows  = GDALGetRasterBandYSize(hBand);
	int bands = GDALGetRasterCount(hDataset);
	
	byte * pbuf = NULL;
	pbuf = (byte *)CPLMalloc(sizeof(byte)*cols);
	
	byte min = 0, max = 0, mean = 0;
	double stddev = 0;
    double summ = 0;
    int count = 0;
	
	for(int band=1; band<=bands; band++)
	{
		if(userBandNumber != -1) fprintf(stderr, "Band %d...\n", userBandNumber);
		else fprintf(stderr, "Band %d...\n", band);
		
		hBand =  GDALGetRasterBand(hDataset, band);		
		if(flNoDataValueAsBackground) NoDataValue = getFloatNoDataValueAsBackground(hBand);
		
		min = max = mean = stddev = summ = 0;
		count = 0;
		bool flFirst = true;
		
		int pr = CUtils::progress_ln_ex(stderr, 0, 0, START_PROGRESS);
		for(int i=0; i<rows; i++)
		{
			GDALRasterIO(hBand, GF_Read, 0, i, cols, 1, pbuf, cols, 1, GDT_Byte, 0, 0 );
			for(int j=0; j<cols; j++) if(pbuf[j]!=NoDataValue) 
			{
				if(flFirst)
				{
					mean = pbuf[j];
					min = max = mean;
					flFirst = false;
				}
				else
				{
					mean += pbuf[j];
					if( min > pbuf[j] ) min = pbuf[j];
					if( max < pbuf[j] ) max = pbuf[j];
				}
				count++;
			}
			pr = CUtils::progress_ln_ex(stderr, i, rows, pr);
		}
		CUtils::progress_ln_ex(stderr, 0, 0, END_PROGRESS);
		
		double dmean = 0;
		if(count > 0) dmean = mean / (double)count;
		
		pr = CUtils::progress_ln_ex(stderr, 0, 0, START_PROGRESS);
		for(int i=0; i<rows; i++)
		{
			GDALRasterIO(hBand, GF_Read, 0, i, cols, 1, pbuf, cols, 1, GDT_Byte, 0, 0 );
			for(int j=0; j<cols; j++) if(pbuf[j]!=NoDataValue) summ += ((double)pbuf[j]-dmean)*((double)pbuf[j]-dmean);
						
			pr = CUtils::progress_ln_ex(stderr, i, rows, pr);
		}
		CUtils::progress_ln_ex(stderr, 0, 0, END_PROGRESS);
		
		summ = 0; stddev = 0;
		if((count-1)>0)
		{
			summ /= (double)(count-1);
			if(summ!=0) stddev = sqrt(summ);
		}
			
		GDALSetRasterStatistics(hBand, min, max, mean, stddev);
		GDALSetRasterNoDataValue(hBand, NoDataValue);
	}
	
	CPLFree(pbuf);
}
/*
float CUtils::getFloatNoDataValueAsBackground(GDALRasterBandH hBand)
{
	float NoDataValue = 0;
	if(hBand != NULL) GDALRasterIO(hBand, GF_Read, 1, 1, 1, 1, &NoDataValue, 1, 1, GDT_Float32, 0, 0 );
	return NoDataValue;
}
*/
float CUtils::getFloatNoDataValueAsBackground(GDALRasterBandH hBand)
{
	float NoDataValue = 0;
	if(hBand != NULL) 
	{
		void* p = NULL;
		p = mallocData(hBand, p, 1);
		getRasterLine(hBand, 1, 1, p);
		NoDataValue = getDataAsFloat(hBand, (const void*)p, 1);
		CPLFree(p);
	}
	return NoDataValue;
}

bool CUtils::insideInPolygons(OGRDataSourceH poDS, double x, double y)
{
	bool res = false;
	OGRGeometryH pt = OGR_G_CreateGeometry(wkbPoint);
	OGR_G_AddPoint_2D(pt, x, y);

	for(int iLayer = 0; iLayer < OGR_DS_GetLayerCount(poDS); iLayer++)	
	{
		OGRLayerH poLayer = OGR_DS_GetLayer(poDS, iLayer);
		if(poLayer!=NULL)
		{
			OGREnvelope layerBounds;
			OGR_L_GetExtent(poLayer, &layerBounds, 1);
			
			if(	(layerBounds.MinX <= x) && (layerBounds.MinY <= y) && 
				(layerBounds.MaxX >= x) && (layerBounds.MaxY >= y) )
			{
				OGR_L_ResetReading(poLayer);
				if(OGR_FD_GetGeomType( OGR_L_GetLayerDefn(poLayer) ) == wkbPolygon)
				{
					OGRFeatureH poFeat;
					while((poFeat = OGR_L_GetNextFeature(poLayer))!= NULL)
					{
						OGRGeometryH hGeom = OGR_F_GetGeometryRef(poFeat);
						if(OGR_G_Within(pt, hGeom))
						{
							res = true;
							break;
						}
					}
					if(res) { OGR_L_ResetReading(poLayer); break; }
				}
			}
		}
	}	
	OGR_G_DestroyGeometry(pt);
	return res;
}

bool CUtils::isFloat32DataType(GDALDatasetH hDataset)
{
	return ( GDALGetRasterDataType( GDALGetRasterBand(hDataset, 1) ) == GDT_Float32 );
}
