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

#include <stdio.h>
#include <stdlib.h>
#include <gdal_priv.h>
#include <cpl_conv.h>
#include <ogr_spatialref.h>
#include "s2_xml.h"

#define PROG_VERSION "1"
#define DATE_VERSION "09.03.2016"

#define MAX_PATH 1024

typedef unsigned short int UINT16;

void printMessage();
int progress(int band, int index, int count, int oldpersent);
float getReflectance(UINT16 dn);

int main(int argc, char* argv[])
{
	fprintf(stderr, "SENTINEL-2. SET PROJECTION and REFLECTANCE Creator (for product L1C S2A MSI)\nVersion %s.%s. Free software. GNU General Public License, version 3\n", PROG_VERSION, DATE_VERSION);
	fprintf(stderr, "Copyright (C) 2016 Igor Garkusha.\nUkraine, Dnipropetrovsk\n\n");
	
	if(argc!=7) 
	{
		fputs("Input parameters not found!\n", stderr);
		fputs("Example:\n", stderr);
		fputs("s2_set_projection S2_TILE_Band.tif S2_PRODUCT_XML.xml S2_TILE_XML.xml Resolution S2_OutputFloat_TILE_Band.tif NoDataValueForOutput\n", stderr);
		fputs("s2_set_projection Band2.tif S2_PRODUCT_XML.xml S2_TILE_XML.xml 10 Band2_refl.tif 0\n", stderr);
		fputs("\nResolution Bands S2A MSI: 10 -- 2, 3, 4, 8\n", stderr);
		fputs("                          20 -- 5, 6, 7, 8b, 11, 12\n", stderr);
		fputs("                          60 -- 1, 9, 10\n\n", stderr);
		return 1;
	}
		
	char inputTiffFileName[MAX_PATH]="";
	char outputTiffFileName[MAX_PATH]="";
	
	int resolution = atoi(argv[argc-3]);
	float NoDataValue = atof(argv[argc-1]);
	
	strcpy(inputTiffFileName, argv[1]);
	strcpy(outputTiffFileName, argv[argc-2]);
	
	InitXML();
	StartParseXML(argv[2]);
	StartParseXML(argv[3]);
	DestroyXML();
	
	printS2BandInfo();
	
	GDALAllRegister();	


	// ================================================================
	// PREPARE PROJECTION INFO

	double adfGeoTransform[6]={0, 1, 0, 0, 0, -1};
	
	int indexResolution = 0;
	if(resolution == 20) indexResolution = 1;
	else
	if(resolution == 60) indexResolution = 2;
			
	adfGeoTransform[0]=bandsInfo[indexResolution].ULX;  // XLeftBottom
	adfGeoTransform[1]=bandsInfo[indexResolution].XDIM; // PixelWidth
	adfGeoTransform[2]=0;
	adfGeoTransform[3]=bandsInfo[indexResolution].ULY;  // YLeftBottom
	adfGeoTransform[4]=0;
	adfGeoTransform[5]=bandsInfo[indexResolution].YDIM; // PixelHeight
		
	OGRSpatialReferenceH oSRS = OSRNewSpatialReference("");	
	OSRSetWellKnownGeogCS( oSRS, "EPSG:4326" );
	
	epsg_code -= 32000;
	if((epsg_code-600)>100) 
		OSRSetUTM( oSRS, epsg_code-700, 0); // Southern
	else
		OSRSetUTM( oSRS, epsg_code-600, 1); // Northern
		
	char *szProjection = NULL;
	OSRExportToWkt( oSRS, &szProjection );
	// ================================================================
	
		
	GDALDatasetH  	hDatasetIn = NULL;
	GDALDatasetH  	hDatasetOut = NULL;
    GDALRasterBandH hBandIn = NULL;
    GDALRasterBandH hBandOut = NULL;
    
    int bands = 0;
    int cols  = 0;
    int rows  = 0;
	
	hDatasetIn = GDALOpen(inputTiffFileName, GA_ReadOnly);
	
	if(hDatasetIn)
	{
		bands = GDALGetRasterCount(hDatasetIn);
		hBandIn = GDALGetRasterBand(hDatasetIn, 1);
		if(hBandIn)
		{
			cols = GDALGetRasterBandXSize(hBandIn);
			rows = GDALGetRasterBandYSize(hBandIn);
			
			char **papszOptions = NULL;
			GDALDriverH hDriver;
			if( (hDriver = GDALGetDriverByName("GTiff")) != NULL)
			{
				if( (hDatasetOut = GDALCreate( hDriver, outputTiffFileName, cols, rows, bands, GDT_Float32, papszOptions )) !=NULL )
				{
					GDALSetGeoTransform(hDatasetOut, adfGeoTransform );
					GDALSetProjection(hDatasetOut, szProjection );
					
					UINT16 *pLineIn = (UINT16 *)CPLMalloc(sizeof(UINT16)*cols);
					float *pLineOut = (float *)CPLMalloc(sizeof(float)*cols);
					
					for(int band = 1; band<=bands; band++)
					{
						if( (hBandIn = GDALGetRasterBand(hDatasetIn, band)) != NULL )
						{
							if( (hBandOut = GDALGetRasterBand(hDatasetOut, band)) != NULL )
							{
								int oldpr = -1;
								for(int i=0; i<rows; i++)
								{
									GDALRasterIO(hBandIn,  GF_Read, 0, i, cols, 1, pLineIn, cols, 1, GDALGetRasterDataType(hBandIn), 0, 0);
									
									for(int j=0; j<cols; j++) pLineOut[j] = getReflectance(pLineIn[j]);
									
									GDALRasterIO(hBandOut, GF_Write, 0, i, cols, 1, pLineOut, cols, 1, GDT_Float32, 0, 0 );
									
									oldpr=progress(band, i, rows-1, oldpr);
								}
								
								GDALSetRasterNoDataValue(hBandOut, NoDataValue);
								
								///////////////////////////////////////////////
								// Recalc Statistics
								
								fputs("\nCreate Statistics...\n", stderr);
								
								double min = 0;
								double max = 0;
								double avg = 0;
								double stddev = 0;
								bool flFirst = true;
								double count = 0;
											
								for(int i=0; i<rows; i++)
								{
									GDALRasterIO(hBandOut,  GF_Read, 0, i, cols, 1, pLineOut, cols, 1, GDT_Float32, 0, 0);
									
									for(int j=0; j<cols; j++)
									{
										if(pLineOut[j] != NoDataValue) 
										{ 
											avg += pLineOut[j]; count++; 

											if(flFirst) { min = max = pLineOut[j]; flFirst = false; }
											else
											{
												if(min > pLineOut[j]) min = pLineOut[j];
												if(max < pLineOut[j]) max = pLineOut[j];
											}
										}
									}
								}

								avg /= count;

								for(int i=0; i<rows; i++)
								{
									GDALRasterIO(hBandOut,  GF_Read, 0, i, cols, 1, pLineOut, cols, 1, GDT_Float32, 0, 0);
									for(int j=0; j<cols; j++) if(pLineOut[j] != NoDataValue) stddev += ((pLineOut[j]-avg)*(pLineOut[j]-avg));
								}

								stddev /= (count-1.0);
								stddev = sqrt(stddev);
								
								GDALSetRasterStatistics(hBandOut, min, max, avg, stddev);
								///////////////////////////////////////////////
							}
						}
					}
					
					CPLFree(pLineIn);
					CPLFree(pLineOut);
					
					GDALClose(hDatasetOut);
				}
			}
			
		}
		
		GDALClose(hDatasetIn);
		fputs("Processing COMPLETE!\n", stderr);
	}
	
	return 0;
}

int progress(int band, int index, int count, int oldpersent)
{
	float tmp = index*100.0f/(float)count;
	int pr = (int)tmp;
	if(oldpersent!= pr)	fprintf(stderr, "\rBand %d. Progress: %d%%", band, pr);
	if(pr==100) fprintf(stderr, "\n");
	return pr;
}

float getReflectance(UINT16 dn)
{
	return (float)((double)dn/QUANTIFICATION_VALUE);
}
