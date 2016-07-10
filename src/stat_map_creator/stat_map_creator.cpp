/*
 * Project: Remote Sensing Utilities (Extentions GDAL/OGR)
 * Author:  Igor Garkusha <rsutils.gis@gmail.com>
 *          Ukraine, Dnipro
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

#define NODATAVALUE 	-9999

#define PROG_VERSION "1"
#define DATE_VERSION "10.07.2016"

void printHelp();

typedef enum{MIN, MAX, AVG, SUM, STD, VAR} STATMODE;

int main(int argc, char* argv[])
{
	fprintf(stderr, "STATISTIC MAP CREATOR\nVersion %s.%s. Free software. GNU General Public License, version 3\n", PROG_VERSION, DATE_VERSION);
	fprintf(stderr, "Copyright (C) 2016 Igor Garkusha.\nUkraine, Dnipro\n\n");
	
	if(argc!=5)
	{
		fputs("Input parameters not found!\n", stderr);
		printHelp();

		fputs("\n", stderr);
		
		return 1;
	}
	
    GDALDatasetH  pSrcDataset = NULL;
    GDALDatasetH  pDstDataset = NULL;
    
    GDALRasterBandH pSrcBand = NULL;
    GDALRasterBandH pDstBand = NULL;
    
    GDALDriverH pDriver = NULL;

    GDALAllRegister();
	
	bool flagNoData = (atoi(argv[argc-1]) == 1)?true:false;
	int statIndexFlag = -1;
	
	if( strcmp(argv[argc-2], "min") == 0) 	statIndexFlag = MIN;
	else
	if( strcmp(argv[argc-2], "max") == 0) 	statIndexFlag = MAX;
	else
	if( strcmp(argv[argc-2], "avg") == 0) 	statIndexFlag = AVG;
	else
	if( strcmp(argv[argc-2], "sum") == 0) 	statIndexFlag = SUM;
	else
	if( strcmp(argv[argc-2], "std") == 0) 	statIndexFlag = STD;
	else
	if( strcmp(argv[argc-2], "var") == 0) 	statIndexFlag = VAR;
	else statIndexFlag = AVG;
	
		    
    pSrcDataset = GDALOpen( argv[1], GA_ReadOnly );
    
    int bands = 0;
    
    if(pSrcDataset!=NULL)
    {
		if(CUtils::isFloat32DataType(pSrcDataset))
		{
			bands = GDALGetRasterCount(pSrcDataset);
			
			pSrcBand = GDALGetRasterBand(pSrcDataset, 1);
			if(pSrcBand != NULL)
			{
				int cols  = GDALGetRasterBandXSize(pSrcBand);
				int rows  = GDALGetRasterBandYSize(pSrcBand);				
				
				float NoDataValue = 0;

				pDriver = GDALGetDriverByName("GTiff");
				char **papszOptions = NULL;
				pDstDataset = GDALCreate(pDriver, argv[2], cols, rows, 1, GDT_Float32, papszOptions);
				double adfGeoTransform[6]={0};
				GDALGetGeoTransform(pSrcDataset, adfGeoTransform );
				const char *szProjection = GDALGetProjectionRef(pSrcDataset);
				GDALSetGeoTransform(pDstDataset, adfGeoTransform );
				GDALSetProjection(pDstDataset, szProjection );
			
				pDstBand = GDALGetRasterBand(pDstDataset, 1);
			
				float *pSrcLine = NULL;
				float *pDstLine = NULL;
		
				pSrcLine = (float*)CPLMalloc(sizeof(GDALGetRasterDataType(pSrcBand))*cols);
				pDstLine = (float*)CPLMalloc(sizeof(GDALGetRasterDataType(pDstBand))*cols);
				
				int pr = CUtils::progress_ln_ex(stderr, 0, 0, START_PROGRESS);
				
				if(flagNoData == false)
				{
					for(int i=0; i<rows; i++)
					{
						for(int j=0; j<cols; j++) pDstLine[j] = 0;
						for(int j=0; j<cols; j++)
						{
							float v = 0;
							float min = 0;
							float max = 0;
							float avg = 0;
							float sum = 0;
														
							for(int k=1; k<=bands; k++)
							{
								pSrcBand = GDALGetRasterBand(pSrcDataset, k);
								
								CUtils::getRasterValue(pSrcBand, i, j, (void*)&v);
															
								switch(statIndexFlag)
								{
									case MIN:
										if(k == 1) min = v;	else min = (min>v)?v:min;
										break;
									case MAX:
										if(k == 1) max = v;	else max = (max<v)?v:max;
										break;
									case AVG:
									case VAR:
									case STD:
									case SUM:
										sum += v;
										break;
								}
							}
							
							if( (statIndexFlag == STD)||(statIndexFlag == VAR) )
							{
								avg = sum/(float)bands;
								sum = 0;
								for(int k=1; k<=bands; k++)
								{
									pSrcBand = GDALGetRasterBand(pSrcDataset, k);
									CUtils::getRasterValue(pSrcBand, i, j, (void*)&v);
									sum += ((v-avg)*(v-avg));
								}
							}
							
							switch(statIndexFlag)
							{
								case MIN: pDstLine[j] = min; break;
								case MAX: pDstLine[j] = max; break;
								case AVG: pDstLine[j] = sum/(float)bands; break;
								case SUM: pDstLine[j] = sum; break;
								case VAR: pDstLine[j] = sum/(float)(bands-1); break;
								case STD: pDstLine[j] = sqrtf(sum/(float)(bands-1)); break;
							}
						}
						
						GDALRasterIO(pDstBand, GF_Write, 0, i, cols, 1, pDstLine, cols, 1, GDT_Float32, 0, 0 );
						
						pr = CUtils::progress_ln_ex(stderr, i, rows, pr);
					}
				}
				else // WITH NODATA VALUE - pixel(1,1)
				{
					for(int i=0; i<rows; i++)
					{
						for(int j=0; j<cols; j++) pDstLine[j] = 0;
						for(int j=0; j<cols; j++)
						{
							float v = 0;
							float min = 0;
							float max = 0;
							float avg = 0;
							float sum = 0;

							bool flNoData = false;
							
							for(int k=1; k<=bands; k++)
							{
								pSrcBand = GDALGetRasterBand(pSrcDataset, k);
								NoDataValue = CUtils::getFloatNoDataValueAsBackground(pSrcBand);
								
								CUtils::getRasterValue(pSrcBand, i, j, (void*)&v);
								
								if(v == NoDataValue) { flNoData = true; break; }
								
								switch(statIndexFlag)
								{
									case MIN:
										if(k == 1) min = v;	else min = (min>v)?v:min;
										break;
									case MAX:
										if(k == 1) max = v;	else max = (max<v)?v:max;
										break;
									case AVG:
									case VAR:
									case STD:
									case SUM:
										sum += v;
										break;
								}
							}
							
							if(flNoData)
							{
								pDstLine[j] = 0;
								continue;
							}
							
							if( (statIndexFlag == STD)||(statIndexFlag == VAR) )
							{
								avg = sum/(float)bands;
								sum = 0;
								for(int k=1; k<=bands; k++)
								{
									pSrcBand = GDALGetRasterBand(pSrcDataset, k);
									CUtils::getRasterValue(pSrcBand, i, j, (void*)&v);
									sum += ((v-avg)*(v-avg));
								}
							}
							
							switch(statIndexFlag)
							{
								case MIN: pDstLine[j] = min; break;
								case MAX: pDstLine[j] = max; break;
								case AVG: pDstLine[j] = sum/(float)bands; break;
								case SUM: pDstLine[j] = sum; break;
								case VAR: pDstLine[j] = sum/(float)(bands-1); break;
								case STD: pDstLine[j] = sqrtf(sum/(float)(bands-1)); break;
							}
						}
						
						GDALRasterIO(pDstBand, GF_Write, 0, i, cols, 1, pDstLine, cols, 1, GDT_Float32, 0, 0 );
						
						pr = CUtils::progress_ln_ex(stderr, i, rows, pr);
					}
				}
				
				CUtils::progress_ln_ex(stderr, 0, 0, END_PROGRESS);
				
				CPLFree(pSrcLine); pSrcLine = NULL;
				CPLFree(pDstLine); pDstLine = NULL;
				
				if(flagNoData) CUtils::calculateFloatGeoTIFFStatistics(pDstDataset, -1, true);
				else CUtils::calculateFloatGeoTIFFStatistics(pDstDataset, -1, false);
				
				fputs("\nEnd Processing.\n\n", stderr);
			}
						
		}
		
		if(pSrcDataset!=NULL) { GDALClose(pSrcDataset); }
		if(pDstDataset!=NULL) { GDALClose(pDstDataset); }
	}

    return 0;
}

void printHelp()
{
	fputs("Example:\n", stderr);
	fputs("stat_map_creator Input_Multiband.tif Output_SingleBand.tif StatIndexFlag NoDataModeFlag\n", stderr);
	fputs("stat_map_creator multiband.tif avg.tif avg 1\n", stderr);
	fputs("\n", stderr);
	
	fputs("StatIndexFlag:\n", stderr);
	
	fputs("\tmin   -- minimum\n", stderr);
	fputs("\tmax   -- maximum\n", stderr);
	fputs("\tavg   -- average\n", stderr);
	fputs("\tsum   -- summa\n", stderr);
	fputs("\tstd   -- standard deviation\n", stderr);	
	fputs("\tvar   -- variance\n\n", stderr);
	
	fputs("NoDataModeFlag: Value of pixel with coordinates (1,1)\n\n", stderr);
}
