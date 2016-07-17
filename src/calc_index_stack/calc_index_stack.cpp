/*
 * Project: Remote Sensing Utilities (Extentions GDAL/OGR)
 * Author:  Igor Garkusha <rsutils.gis@gmail.com>
 *          Ukraine, Dnipro (Dnipropetrovsk)
 * 
 * Copyright (C) 2016, Igor Garkusha <rsutils.gis@gmail.com>
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

#include <gdal_priv.h>
#include <cpl_conv.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../lib/utils.h"

#define NODATAVALUE -9999

#define PROG_VERSION "1"
#define DATE_VERSION "17.07.2016"

int count_indexes = -1;
int * indexes = NULL;
int idSensor = -1;
float * noDataValues = NULL;

typedef enum{OLI27, OLI25, OLI45, TM, TM34, ETM, ETM34, MSI, MSI10}SENSOR;

typedef enum{NDVI, GNDVI, BNDVI, NBR, NDSII, NDWI, NDSI, NDSI2, RVI, SW,
			 RB, SN, DVI, IPVI, EVI, EVI2, SAVI, SI_BLUE, SI_GREEN, 
			 BI_RED_NIR, BI_GREEN_RED, CI_GREEN, CI_RED, WDRVI, GBNDVI,
			 VARI, LAI
			}V_INDEX;

void printHelp();
bool loadParametersFile(const char* fileName);
int getIdSensorFromName(const char * sensorName);
int getIndexFromName(const char * strIndexName);
const char * getIndexName(int index);
void createAndSaveIndexes(GDALDatasetH  pDstDataset, GDALDatasetH  pSrcDataset, int rows, int cols, int bands, int idSensor, bool flagNoData, const float* srcNoDataValue);
float ** calculateIndexes(float ** pSrcLine, int srcBands, int srcCols, float ** pDstLine, int idSensor, bool flagNoData, const float* srcNoDataValue);
float getVIndex(float ** pSrcLine, int srcBands, int column, int index, int idSensor, bool flagNoData, const float* srcNoDataValue);

int main(int argc, char* argv[])
{
	fprintf(stderr, "CALCULATION OF VEGETATION INDICES STACK\nVersion %s.%s. Free software. GNU General Public License, version 3\n", PROG_VERSION, DATE_VERSION);
	fprintf(stderr, "Copyright (C) 2016 Igor Garkusha.\nUkraine, Dnipro (Dnipropetrovsk)\n\n");
	
	if(argc!=5)
	{
		fputs("Input parameters not found!\n", stderr);
		printHelp();

		fputs("\n", stderr);
		
		return 1;
	}
	
	if( loadParametersFile(argv[3]) )
	{
		GDALDatasetH  pSrcDataset = NULL;
		GDALDatasetH  pDstDataset = NULL;  
		GDALRasterBandH pSrcBand = NULL;
		GDALDriverH pDriver = NULL;

		GDALAllRegister();
	
		bool flagNoData = (atoi(argv[argc-1]) == 1)?true:false;
		
		pSrcDataset = GDALOpen( argv[1], GA_ReadOnly );
    
		int bands = 0;
		
		if(pSrcDataset!=NULL)
		{
			if(CUtils::isFloat32DataType(pSrcDataset))
			{
				bands = GDALGetRasterCount(pSrcDataset);
				if( ((bands == 6)||(bands == 4)||(bands == 2)) )
				{
					pSrcBand = GDALGetRasterBand(pSrcDataset, 1);
					if(pSrcBand != NULL)
					{
						int cols  = GDALGetRasterBandXSize(pSrcBand);
						int rows  = GDALGetRasterBandYSize(pSrcBand);

						if(flagNoData)
						{
							noDataValues = new float[bands];
							for(int i=0; i<bands; i++) noDataValues[i] = 0;
						}

						pDriver = GDALGetDriverByName("GTiff");
						char **papszOptions = NULL;
						pDstDataset = GDALCreate(pDriver, argv[2], cols, rows, count_indexes, GDT_Float32, papszOptions);
						double adfGeoTransform[6]={0};
						GDALGetGeoTransform(pSrcDataset, adfGeoTransform );
						const char *szProjection = GDALGetProjectionRef(pSrcDataset);
						GDALSetGeoTransform(pDstDataset, adfGeoTransform );
						GDALSetProjection(pDstDataset, szProjection );
						
						if(flagNoData)
						{
							for(int i=0; i<bands; i++)
							{
								pSrcBand = GDALGetRasterBand(pSrcDataset, i+1);
								noDataValues[i] = CUtils::getFloatNoDataValueAsBackground(pSrcBand);
							}
						}
						
						createAndSaveIndexes(pDstDataset, pSrcDataset, rows, cols, bands, idSensor, flagNoData, (const float*) noDataValues);
					
						if(flagNoData) CUtils::calculateFloatGeoTIFFStatistics(pDstDataset, -1, true);
						else CUtils::calculateFloatGeoTIFFStatistics(pDstDataset, -1, false);
						
						if(flagNoData) { delete [] noDataValues; noDataValues = NULL; }
						
						fputs("\nEnd Processing.\n\n", stderr);
					}
				}
				else
				{
					fputs("\nERROR: Source Band Number is Invalid!!!\n", stderr);
					fprintf(stderr, "Source Bands Number: %d!\n\n", bands);
					printHelp();
				}
			}
			
			if(pSrcDataset!=NULL) { GDALClose(pSrcDataset); }
			if(pDstDataset!=NULL) { GDALClose(pDstDataset); }
		}
		
		delete [] indexes;
		indexes = NULL;
	}
	
    return 0;
}

bool loadParametersFile(const char* fileName)
{
	FILE* fin = NULL;
	char sensor[10]="";
	char strIndexName[15]="";
	if((fin = fopen(fileName, "rt"))!=NULL)
	{
		fscanf(fin, "%s", sensor);
		idSensor = getIdSensorFromName(sensor);
		fscanf(fin, "%d", &count_indexes);
		indexes = new int[count_indexes];
		for(int i=0; i<count_indexes; i++)
		{
			strcpy(strIndexName, "");
			fscanf(fin, "%s", strIndexName);
			indexes[i] = getIndexFromName(strIndexName);
		}
		fclose(fin);
		return true;
	}
	
	return false;
}

int getIdSensorFromName(const char * sensorName)
{
	if( strcmp(sensorName, "oli27") == 0 ) 	return OLI27;
	if( strcmp(sensorName, "oli25") == 0 ) 	return OLI25;
	if( strcmp(sensorName, "oli45") == 0 ) 	return OLI45;
	if( strcmp(sensorName, "tm") == 0 ) 	return TM;
	if( strcmp(sensorName, "tm34") == 0 ) 	return TM34;
	if( strcmp(sensorName, "etm") == 0 ) 	return ETM;
	if( strcmp(sensorName, "etm34") == 0 ) 	return ETM34;
	if( strcmp(sensorName, "msi") == 0 ) 	return MSI;
	if( strcmp(sensorName, "msi10") == 0 ) 	return MSI10;
	return -1;
}

int getIndexFromName(const char * strIndexName)
{
	if( strcmp(strIndexName, "ndvi") == 0 )		return NDVI;
	if( strcmp(strIndexName, "gndvi") == 0 )	return GNDVI;
	if( strcmp(strIndexName, "bndvi") == 0 )	return BNDVI;
	if( strcmp(strIndexName, "nbr") == 0 )		return NBR;
	if( strcmp(strIndexName, "ndsii") == 0 )	return NDSII;
	if( strcmp(strIndexName, "ndwi") == 0 )		return NDWI;
	if( strcmp(strIndexName, "ndsi") == 0 )		return NDSI;
	if( strcmp(strIndexName, "ndsi2") == 0 )	return NDSI2;
	if( strcmp(strIndexName, "rvi") == 0 )		return RVI;
	if( strcmp(strIndexName, "sw") == 0 )		return SW;
	if( strcmp(strIndexName, "rb") == 0 )		return RB;
	if( strcmp(strIndexName, "sn") == 0 )		return SN;
	if( strcmp(strIndexName, "dvi") == 0 )		return DVI;
	if( strcmp(strIndexName, "ipvi") == 0 )		return IPVI;
	if( strcmp(strIndexName, "evi") == 0 )		return EVI;
	if( strcmp(strIndexName, "evi2") == 0 )		return EVI2;
	if( strcmp(strIndexName, "savi") == 0 )		return SAVI;
	if( strcmp(strIndexName, "si_blue") == 0 )	return SI_BLUE;
	if( strcmp(strIndexName, "si_green") == 0 )		return SI_GREEN;
	if( strcmp(strIndexName, "bi_red_nir") == 0 )	return BI_RED_NIR;
	if( strcmp(strIndexName, "bi_green_red") == 0 )	return BI_GREEN_RED;
	if( strcmp(strIndexName, "ci_green") == 0 )		return CI_GREEN;
	if( strcmp(strIndexName, "ci_red") == 0 )	 	return CI_RED;
	if( strcmp(strIndexName, "wdrvi") == 0 )	return WDRVI;
	if( strcmp(strIndexName, "gbndvi") == 0 )	return GBNDVI;
	if( strcmp(strIndexName, "vari") == 0 )		return VARI;
	if( strcmp(strIndexName, "lai") == 0 )		return LAI;
	
	return -1;
}

const char * getIndexName(int index)
{
	switch(index)
	{
		case NDVI: return "NDVI";
		case GNDVI: return "GNDVI";
		case BNDVI: return "BNDVI";
		case NBR: return "NBR";
		case NDSII: return "NDSII";
		case NDWI: return "NDWI";
		case NDSI: return "NDSI";
		case NDSI2: return "NDSI2";
		case RVI: return "RVI";
		case SW: return "SWI";
		case RB: return "RB";
		case SN: return "SN";
		case DVI: return "DVI";
		case IPVI: return "IPVI";
		case EVI: return "EVI";
		case EVI2: return "EVI2";
		case SAVI: return "SAVI";
		case SI_BLUE: return "SI_BLUE";
		case SI_GREEN: return "SI_GREEN";
		case BI_RED_NIR: return "BI_RED_NIR";
		case BI_GREEN_RED: return "BI_GREEN_RED";
		case CI_GREEN: return "CI_GREEN";
		case CI_RED: return "CI_RED";
		case WDRVI: return "WDRVI";
		case GBNDVI: return "GBNDVI";
		case VARI: return "VARI";
		case LAI: return "LAI";
	}
	
	return "UNKNOWN";
}

void createAndSaveIndexes(GDALDatasetH  pDstDataset, GDALDatasetH  pSrcDataset, int rows, int cols, int bands, int idSensor, bool flagNoData, const float* srcNoDataValue)
{
	float **pSrcLine = NULL;
	float **pDstLine = NULL;

	pSrcLine = new float*[bands];
	for(int i=0; i<bands; i++) pSrcLine[i] = new float[cols];
	
	pDstLine = new float*[count_indexes];
	for(int i=0; i<count_indexes; i++) pDstLine[i] = new float[cols];

	GDALRasterBandH pSrcBand = NULL;
	GDALRasterBandH pDstBand = NULL;
	
	int pr = CUtils::progress_ln_ex(stderr, 0, 0, START_PROGRESS);
	for(int i=0; i<rows; i++)
	{
		for(int k=0; k<bands; k++)
		{
			pSrcBand = GDALGetRasterBand(pSrcDataset, k+1);
			GDALRasterIO(pSrcBand, GF_Read, 0, i, cols, 1, pSrcLine[k], cols, 1, GDALGetRasterDataType(pSrcBand), 0, 0 );
		}
		
		pDstLine = calculateIndexes(pSrcLine, bands, cols, pDstLine, idSensor, flagNoData, srcNoDataValue);
		
		for(int k=0; k<count_indexes; k++)
		{
			pDstBand = GDALGetRasterBand(pDstDataset, k+1);
			GDALRasterIO(pDstBand, GF_Write, 0, i, cols, 1, pDstLine[k], cols, 1, GDALGetRasterDataType(pDstBand), 0, 0 );
		}
		
		pr = CUtils::progress_ln_ex(stderr, i, rows, pr);
	}
	CUtils::progress_ln_ex(stderr, 0, 0, END_PROGRESS);
	
	for(int i=0; i<count_indexes; i++) delete [] pDstLine[i];
	delete [] pDstLine;
	pDstLine = NULL;
	
	for(int i=0; i<bands; i++) delete [] pSrcLine[i];
	delete [] pSrcLine;
	pSrcLine = NULL;
}

float ** calculateIndexes(float ** pSrcLine, int srcBands, int srcCols, float ** pDstLine, int idSensor, bool flagNoData, const float* srcNoDataValue)
{
	for(int j=0; j< count_indexes; j++)
	{
		for(int i=0; i<srcCols; i++) 
		{
			pDstLine[j][i] = getVIndex(pSrcLine, srcBands, i, indexes[j], idSensor, flagNoData, srcNoDataValue);
		}
	}
	return pDstLine;
}

float getVIndex(float ** pSrcLine, int srcBands, int column, int index, int idSensor, bool flagNoData, const float* srcNoDataValue)
{
	int BLUE=0, GREEN=1, RED=2, NIR=3, SWIR1=4, SWIR2=5;
	float zn = 0;
	
	if((idSensor == MSI10)||(idSensor == OLI25)) { SWIR1 = SWIR2 = -1; }
	else
	if((idSensor == OLI45)||(idSensor == TM34)||(idSensor == ETM34)) { BLUE = GREEN = SWIR1 = SWIR2 = -1; RED = 0; NIR = 1; }
	
	// Check bands values for each pixel
	for(int i=0; i<srcBands; i++) 
	{
		if(flagNoData)
		{
			if(pSrcLine[i][column] == srcNoDataValue[i]) return NODATAVALUE;
		}
		
		pSrcLine[i][column] = (pSrcLine[i][column]<0)?0:pSrcLine[i][column];
		pSrcLine[i][column] = (pSrcLine[i][column]>1)?1:pSrcLine[i][column];
	}
	
	switch(index)
	{
		case NDVI: 
			if((pSrcLine[NIR][column]+pSrcLine[RED][column])!=0) { return (pSrcLine[NIR][column]-pSrcLine[RED][column])/(pSrcLine[NIR][column]+pSrcLine[RED][column]); } else return NODATAVALUE;
		case GNDVI:
			if((idSensor == OLI45)||(idSensor == TM34)||(idSensor == ETM34)) return NODATAVALUE;
			if((pSrcLine[NIR][column]+pSrcLine[GREEN][column])!=0) { return (pSrcLine[NIR][column]-pSrcLine[GREEN][column])/(pSrcLine[NIR][column]+pSrcLine[GREEN][column]); } else return NODATAVALUE;
		case BNDVI:
			if((idSensor == OLI45)||(idSensor == TM34)||(idSensor == ETM34)) return NODATAVALUE;
			if((pSrcLine[NIR][column]+pSrcLine[BLUE][column])!=0) { return (pSrcLine[NIR][column]-pSrcLine[BLUE][column])/(pSrcLine[NIR][column]+pSrcLine[BLUE][column]); } else return NODATAVALUE;
		case NBR:
			if((idSensor == OLI45)||(idSensor == TM34)||(idSensor == ETM34)) return NODATAVALUE;
			if(idSensor == OLI25) return NODATAVALUE;
			if(idSensor == MSI10) return NODATAVALUE;
			if((pSrcLine[NIR][column]+pSrcLine[SWIR2][column])!=0) { return (pSrcLine[NIR][column]-pSrcLine[SWIR2][column])/(pSrcLine[NIR][column]+pSrcLine[SWIR2][column]); } else return NODATAVALUE;
		case NDSII:
			if((idSensor == OLI45)||(idSensor == TM34)||(idSensor == ETM34)) return NODATAVALUE;
			if(idSensor == OLI25) return NODATAVALUE;
			if(idSensor == MSI10) return NODATAVALUE;
			if((pSrcLine[GREEN][column]+pSrcLine[SWIR1][column])!=0) { return (pSrcLine[GREEN][column]-pSrcLine[SWIR1][column])/(pSrcLine[GREEN][column]+pSrcLine[SWIR1][column]); } else return NODATAVALUE;
		case NDWI:
			if((idSensor == OLI45)||(idSensor == TM34)||(idSensor == ETM34)) return NODATAVALUE;
			if(idSensor == OLI25) return NODATAVALUE;
			if(idSensor == MSI10) return NODATAVALUE;
			if((pSrcLine[NIR][column]+pSrcLine[SWIR1][column])!=0) { return (pSrcLine[NIR][column]-pSrcLine[SWIR1][column])/(pSrcLine[NIR][column]+pSrcLine[SWIR1][column]); } else return NODATAVALUE;
		case NDSI:
			if((idSensor == OLI45)||(idSensor == TM34)||(idSensor == ETM34)) return NODATAVALUE;
			if(idSensor == OLI25) return NODATAVALUE;
			if(idSensor == MSI10) return NODATAVALUE;
			if((pSrcLine[SWIR1][column]+pSrcLine[SWIR2][column])!=0) { return (pSrcLine[SWIR1][column]-pSrcLine[SWIR2][column])/(pSrcLine[SWIR1][column]+pSrcLine[SWIR2][column]); } else return NODATAVALUE;
		case NDSI2:
			if((pSrcLine[RED][column]+pSrcLine[NIR][column])!=0) { return (pSrcLine[RED][column]-pSrcLine[NIR][column])/(pSrcLine[RED][column]+pSrcLine[NIR][column]); } else return NODATAVALUE;
		case RVI:
			if(pSrcLine[RED][column]!=0) { return pSrcLine[NIR][column]/pSrcLine[RED][column]; } else return NODATAVALUE;
		case SW:
			if((idSensor == OLI45)||(idSensor == TM34)||(idSensor == ETM34)) return NODATAVALUE;
			if(idSensor == OLI25) return NODATAVALUE;
			if(idSensor == MSI10) return NODATAVALUE;
			if(pSrcLine[SWIR2][column]!=0) { return pSrcLine[SWIR1][column]/pSrcLine[SWIR2][column]; } else return NODATAVALUE;
		case RB:
			if((idSensor == OLI45)||(idSensor == TM34)||(idSensor == ETM34)) return NODATAVALUE;
			if(pSrcLine[BLUE][column]!=0) { return pSrcLine[RED][column]/pSrcLine[BLUE][column]; } else return NODATAVALUE;
		case SN:
			if((idSensor == OLI45)||(idSensor == TM34)||(idSensor == ETM34)) return NODATAVALUE;
			if(idSensor == OLI25) return NODATAVALUE;
			if(idSensor == MSI10) return NODATAVALUE;
			if(pSrcLine[NIR][column]!=0) { return pSrcLine[SWIR1][column]/pSrcLine[NIR][column]; } else return NODATAVALUE;
		case DVI:
			return (pSrcLine[NIR][column]-pSrcLine[RED][column]);
		case IPVI:
			if((pSrcLine[NIR][column]+pSrcLine[RED][column])!=0) { return pSrcLine[NIR][column]/(pSrcLine[NIR][column]+pSrcLine[RED][column]); } else return NODATAVALUE;
		case EVI:
			if((idSensor == OLI45)||(idSensor == TM34)||(idSensor == ETM34)) return NODATAVALUE;
			zn = 1.0f+pSrcLine[NIR][column] + 6.0f*pSrcLine[RED][column] - 7.5f*pSrcLine[BLUE][column];
			if(0 == zn) return NODATAVALUE;
			else return 2.5f*((pSrcLine[NIR][column]-pSrcLine[RED][column])/zn);
		case EVI2:
			return 2.4f*((pSrcLine[NIR][column]-pSrcLine[RED][column])/(pSrcLine[NIR][column]+pSrcLine[RED][column] + 1.0f));
		case SAVI:
			return ((pSrcLine[NIR][column]-pSrcLine[RED][column])/(pSrcLine[NIR][column]+pSrcLine[RED][column] + 0.5f))*(1.0f + 0.5f);
		case SI_BLUE:
			if((idSensor == OLI45)||(idSensor == TM34)||(idSensor == ETM34)) return NODATAVALUE;
			if((pSrcLine[BLUE][column]*pSrcLine[RED][column])>=0) return sqrtf(pSrcLine[BLUE][column]*pSrcLine[RED][column]);
			else return NODATAVALUE;
		case SI_GREEN:
			if((idSensor == OLI45)||(idSensor == TM34)||(idSensor == ETM34)) return NODATAVALUE;
			if((pSrcLine[GREEN][column]*pSrcLine[RED][column])>=0) return sqrtf(pSrcLine[GREEN][column]*pSrcLine[RED][column]);
			else return NODATAVALUE;
		case BI_RED_NIR:
			return sqrtf((pSrcLine[RED][column]*pSrcLine[RED][column])+(pSrcLine[NIR][column]*pSrcLine[NIR][column]));
		case BI_GREEN_RED:
			if((idSensor == OLI45)||(idSensor == TM34)||(idSensor == ETM34)) return NODATAVALUE;
			return sqrtf((pSrcLine[GREEN][column]*pSrcLine[GREEN][column])+(pSrcLine[RED][column]*pSrcLine[RED][column]));
		case CI_GREEN:
			if((idSensor == OLI45)||(idSensor == TM34)||(idSensor == ETM34)) return NODATAVALUE;
			if(pSrcLine[GREEN][column]!=0) return (pSrcLine[NIR][column]/pSrcLine[GREEN][column])-1.0; else return NODATAVALUE;
		case CI_RED:
			if(pSrcLine[RED][column]!=0) return (pSrcLine[NIR][column]/pSrcLine[RED][column])-1.0; else return NODATAVALUE;			
		case WDRVI:
			if((0.2f*pSrcLine[NIR][column]+pSrcLine[RED][column])!=0) return (0.2f*pSrcLine[NIR][column]-pSrcLine[RED][column])/(0.2f*pSrcLine[NIR][column]+pSrcLine[RED][column]); else return NODATAVALUE;
		case GBNDVI:
			if((idSensor == OLI45)||(idSensor == TM34)||(idSensor == ETM34)) return NODATAVALUE;
			if((pSrcLine[NIR][column] + pSrcLine[GREEN][column] + pSrcLine[BLUE][column])!=0) return (pSrcLine[NIR][column] - (pSrcLine[GREEN][column] + pSrcLine[BLUE][column]))/(pSrcLine[NIR][column] + (pSrcLine[GREEN][column] + pSrcLine[BLUE][column])); else return NODATAVALUE;
		case VARI:
			if((idSensor == OLI45)||(idSensor == TM34)||(idSensor == ETM34)) return NODATAVALUE;
			if((pSrcLine[GREEN][column] + pSrcLine[RED][column] + pSrcLine[BLUE][column])!=0) return (pSrcLine[GREEN][column] - pSrcLine[RED][column])/(pSrcLine[GREEN][column] + pSrcLine[RED][column] + pSrcLine[BLUE][column]); else return NODATAVALUE;
		case LAI:
			if((idSensor == OLI45)||(idSensor == TM34)||(idSensor == ETM34)) return NODATAVALUE;
			zn = (1.0f+pSrcLine[NIR][column] + 6.0f*pSrcLine[RED][column] - 7.5f*pSrcLine[BLUE][column]);
			if(0 == zn) return NODATAVALUE;
			else return 3.618f*(2.5f*((pSrcLine[NIR][column] - pSrcLine[RED][column])/zn))-0.118f;
	}
	
	return NODATAVALUE;
}

void printHelp()
{
	fputs("Example:\n", stderr);
			
	fputs("calc_index_stack MultibandReflectance_input.tif StackIndexes_output.tif IndexesList.txt NoDataModeFlagForSource\n", stderr);
	fputs("\n", stderr);

	fputs("IndexesList.txt file format:\n", stderr);
	fputs("\t<SOURCE_REMOTE_SENSING_SCANNER>\n", stderr);
	fputs("\t<N__COUNT_INDEXES_IN_STACK>\n", stderr);
	fputs("\t<NAME_OF_INDEX_1>\n", stderr);
	fputs("\t<NAME_OF_INDEX_2>\n", stderr);
	fputs("\t. . .\n", stderr);
	fputs("\t. . .\n", stderr);
	fputs("\t<NAME_OF_INDEX_N>\n", stderr);
	fputs("Example:\n", stderr);
	fputs("\toli27\n", stderr);
	fputs("\t7\n", stderr);
	fputs("\tndvi\n", stderr);
	fputs("\tgndvi\n", stderr);
	fputs("\tndwi\n", stderr);
	fputs("\tevi\n", stderr);
	fputs("\twdrvi\n", stderr);
	fputs("\tvari\n", stderr);
	fputs("\tlai\n", stderr);
	fputs("\n", stderr);
	fputs("SOURCE_REMOTE_SENSING_SCANNER:\n", stderr);
	fputs("\toli27 -- Landsat-8   OLI  (bands: 2,3,4,5,6,7)\n", stderr);
	fputs("\toli25 -- Landsat-8   OLI  (bands: 2,3,4,5)\n", stderr);
	fputs("\toli45 -- Landsat-8   OLI  (bands: 4,5)\n", stderr);
	fputs("\ttm    -- Landsat-4,5 TM   (bands: 1,2,3,4,5,7)\n", stderr);
	fputs("\ttm34  -- Landsat-4,5 TM   (bands: 3,4)\n", stderr);
	fputs("\tetm   -- Landsat-7   ETM+ (bands: 1,2,3,4,5,7)\n", stderr);
	fputs("\tetm34 -- Landsat-7   ETM+ (bands: 3,4)\n", stderr);
	fputs("\tmsi   -- Sentinel-2A MSI  (bands: 2,3,4,8,11,12)\n", stderr);
	fputs("\tmsi10 -- Sentinel-2A MSI  (bands: 2,3,4,8)\n", stderr);
	fputs("\n", stderr);
	fputs("NAME_OF_INDEX (there is more information in help CALC_INDEX):\n", stderr);
	fputs("\tndvi         -- Normalized Difference Vegetation Index (NDVI) -- (NIR-RED)/(NIR+RED)\n", stderr);
	fputs("\tgndvi        -- Green NDVI -- (NIR-GREEN)/(NIR+GREEN). Unsupport OLI45, TM34, ETM34 mode!\n", stderr);
	fputs("\tbndvi        -- Blue NDVI -- (NIR-BLUE)/(NIR+BLUE). Unsupport OLI45, TM34, ETM34 mode!\n", stderr);
	fputs("\tnbr          -- Normalized Burn Ratio -- (NIR-SWIR2)/(NIR+SWIR2). Unsupport OLI45, TM34, ETM34, OLI25, MSI10 mode!\n", stderr);
	fputs("\tndsii        -- Normalized Difference Snow/Ice Index -- (GREEN-SWIR1)/(GREEN+SWIR1). Unsupport OLI45, TM34, ETM34, OLI25, MSI10 mode!\n", stderr);
	fputs("\tndwi         -- Normalized Difference Water Index -- (NIR-SWIR1)/(NIR+SWIR1). Unsupport OLI45, TM34, ETM34, OLI25, MSI10 mode!\n", stderr);
	fputs("\tndsi         -- Normalized Difference Salinity Index -- (SWIR1-SWIR2)/(SWIR1+SWIR2). Unsupport OLI45, TM34, ETM34, OLI25, MSI10 mode!\n", stderr);
	fputs("\tndsi2        -- Normalized Difference Salinity Index 2 -- (RED-NIR)/(RED+NIR)\n", stderr);
	fputs("\trvi          -- Simple Ratio 800/670 Ratio Vegetation Index -- (NIR)/(RED)\n", stderr);
	fputs("\tsw           -- Simple Ratio SWIRI/SWIRII Clay Minerals -- (SWIR1)/(SWIR2). Unsupport OLI45, TM34, ETM34, OLI25, MSI10 mode!\n", stderr);
	fputs("\trb           -- Simple Ratio Red/Blue Iron Oxide -- (Red)/(Blue). Unsupport OLI45, TM34, ETM34 mode!\n", stderr);
	fputs("\tsn           -- Simple Ratio SWIRI/NIR Ferrous Minerals -- (SWIR1)/(NIR). Unsupport OLI45, TM34, ETM34, OLI25, MSI10 mode!\n", stderr);
	fputs("\tdvi          -- Difference Vegetation Index -- (NIR - RED)\n", stderr);
	fputs("\tipvi         -- Infrared Percentage Vegetation Index -- (NIR)/(NIR+RED)\n", stderr);
	fputs("\tevi          -- Enhanced Vegetation Index -- 2.5*[(NIR - RED)/(1 + NIR + 6*RED - 7.5*BLUE)]. Unsupport OLI45, TM34, ETM34 mode!\n", stderr);
	fputs("\tevi2         -- Enhanced Vegetation Index 2 -- 2.4*[(NIR - RED)/(NIR + RED + 1)]\n", stderr);
	fputs("\tsavi         -- Soil-Adjusted Vegetation Index -- [(NIR - RED)/(NIR + RED + 0.5)]*[1 + 0.5]\n", stderr);
	fputs("\tsi_blue      -- Salinity Index -- SQRT(BLUE * RED). Unsupport OLI45, TM34, ETM34 mode!\n", stderr);
	fputs("\tsi_green     -- Salinity Index -- SQRT(GREEN * RED). Unsupport OLI45, TM34, ETM34 mode!\n", stderr);
	fputs("\tbi_red_nir   -- Brightness Index -- SQRT(RED^2 + NIR^2)\n", stderr);
	fputs("\tbi_green_red -- Brightness Index -- SQRT(GREEN^2 + RED^2). Unsupport OLI45, TM34, ETM34 mode!\n", stderr);
	fputs("\tci_green     -- Green Chlorophyll Index -- (NIR/GREEN)-1. Unsupport OLI45, TM34, ETM34 mode!\n", stderr);
	fputs("\tci_red       -- Red-edge Chlorophyll Index -- (NIR/RED)-1\n", stderr);
	fputs("\twdrvi        -- Wide dynamic range vegetation index -- (0.2*NIR-RED)/(0.2*NIR+RED)\n", stderr);
	fputs("\tgbndvi       -- Green-Blue NDVI -- [NIR - (GREEN + BLUE)]/[NIR + (GREEN + BLUE)]. Unsupport OLI45, TM34, ETM34 mode!\n", stderr);
	fputs("\tvari         -- Visible Atmospherically Resistant Index -- [(GREEN - RED)]/(GREEN + RED + BLUE)]. Unsupport OLI45, TM34, ETM34 mode!\n", stderr);
	fputs("\tlai          -- Green Leaf Area Index -- (3.618*EVI-0.118). Unsupport OLI45, TM34, ETM34 mode!\n", stderr);	
	
	fputs("\n", stderr);

	fputs("NoDataModeFlagForSource: Value of source pixel with coordinates (1,1)\n", stderr);
	fputs("NoData for Output Stack Image: -9999\n\n", stderr);
}
