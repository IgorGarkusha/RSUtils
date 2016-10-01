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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gdal_priv.h>
#include <cpl_conv.h>

#define PROG_VERSION "1"
#define DATE_VERSION "28.08.2016"

#define MAX_PATH 1024

int progress(int band, int index, int count, int oldpersent);
float getCorrectReflectance(float rTOA, int method, float threshold, double NoDataValue);

int main(int argc, char* argv[])
{
	fprintf(stderr, "CHECK and CORRECT REFLECTANCE\nVersion %s.%s. Free software. GNU General Public License, version 3\n", PROG_VERSION, DATE_VERSION);
	fprintf(stderr, "Copyright (C) 2016 Igor Garkusha.\nUkraine, Dnipro (Dnipropetrovsk)\n\n");
	
	if(argc!=4) 
	{
		fputs("Input parameters not found!\n", stderr);
		fputs("Example:\n", stderr);
		fputs("check_reflectance input.tif Method output.tif\n", stderr);
		fputs("Method: 1 -- simple clipping if outside interval 0-1 [if(Value<0) OutValue=0; if(Value>1) OutValue=1;]\n", stderr);
		fputs("        2 -- clipping and transformation (For ThresholdValueOnLevel1Percent>1):\n",stderr);
		fputs("               if( Value < 0 ) OutValue := 0\n", stderr);
		fputs("               if( 0 <= Value < ThresholdValueOnLevel1Percent ) OutValue := Value/ThresholdValueOnLevel1Percent\n", stderr);
		fputs("               if( Value >= ThresholdValueOnLevel1Percent ) OutValue := 1\n", stderr);
		fputs("        3 -- simple clipping if outside interval 0-1 with background pixel (coords: 1,1) [if(Value<0) OutValue=NoDataValue; if(Value>1) OutValue=NoDataValue;]\n", stderr);
		fputs("        4 -- clipping and transformation (For ThresholdValueOnLevel1Percent>1) with background pixel (coords: 1,1):\n",stderr);
		fputs("               if( Value < 0 ) OutValue := NoDataValue\n", stderr);
		fputs("               if( 0 <= Value < ThresholdValueOnLevel1Percent ) OutValue := Value/ThresholdValueOnLevel1Percent\n", stderr);
		fputs("               if( Value >= ThresholdValueOnLevel1Percent ) OutValue := NoDataValue\n\n", stderr);
		return 1;
	}
		
	char inputTiffFileName[MAX_PATH]="";
	char outputTiffFileName[MAX_PATH]="";
	
	int method = atoi(argv[2]);
		
	strcpy(inputTiffFileName, argv[1]);
	strcpy(outputTiffFileName, argv[argc-1]);
	
	GDALAllRegister();	
		
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
					double adfGeoTransform[6]={0, 0, 0, 0, 0, 0};
					GDALGetGeoTransform(hDatasetIn, adfGeoTransform);
					GDALSetGeoTransform(hDatasetOut, adfGeoTransform );
										
					if( GDALGetProjectionRef( hDatasetIn ) != NULL ) 
					{
						////////////////////////////////////////////////////////////////
						// !!! THIS IS BUG !!!
						// char szProjection[4096]="";
						// strcpy(szProjection, GDALGetProjectionRef( hDatasetIn ));
						// GDALSetProjection(hDatasetOut, szProjection );
						////////////////////////////////////////////////////////////////
						
						GDALSetProjection(hDatasetOut, GDALGetProjectionRef( hDatasetIn ) );
					}
					
					float *pLineIn = (float *)CPLMalloc(sizeof(float)*cols);
					float *pLineOut = (float *)CPLMalloc(sizeof(float)*cols);
					
					for(int band = 1; band<=bands; band++)
					{
						if( (hBandIn = GDALGetRasterBand(hDatasetIn, band)) != NULL )
						{
							if( (hBandOut = GDALGetRasterBand(hDatasetOut, band)) != NULL )
							{
								int bSuccess = 0;
								double NoDataValue = 0;
								if( method != 1 ) 
								{
									NoDataValue = GDALGetRasterNoDataValue(hBandIn, &bSuccess);
									if(!bSuccess) NoDataValue = 0;
								}
								
								float vThreshold = 0;
								
								double min = 0;
								double max = 0;
								double avg = 0;
								double stddev = 0;
								
								int oldpr = -1;
																
								GDALGetRasterStatistics(hBandIn, FALSE, TRUE, &min, &max, &avg, &stddev);
								
								if(method == 2)
								{
									double PersentThreshold = 1.0;

									fprintf(stderr, "Compute Histogram...\n");
									int hist[256] = {0};
									int iv = 0;
									
									oldpr = -1;
									for(int i=0; i<rows; i++)
									{
										GDALRasterIO(hBandIn,  GF_Read, 0, i, cols, 1, pLineIn, cols, 1, GDALGetRasterDataType(hBandIn), 0, 0);
										for(int j=0; j<cols; j++) 
										{
											iv = (int)(((pLineIn[j]-min)*255.0)/(max-min));
											if(iv<0) iv=0;
											else if(iv>255) iv = 255;
											hist[iv]++;
										}
										oldpr=progress(band, i, rows-1, oldpr);
									}
									
									int imax = hist[0]; for(int k=1; k<256; k++) if(hist[k]>imax) imax = hist[k];
									int findPr = (int)((PersentThreshold*(double)imax)/100.0);
				
									// max-min = 255
									// x-min   = res
									// x = ((res*(max-min))/255.0)+min;
									
									for(int k=255; k>=0; k--) if(hist[k]>=findPr) { vThreshold = ((k*(max-min))/255.0)+min; break; }
									
									fprintf(stderr, "Threshold Reflectance Level: %lf\n", vThreshold);
								}
								
								fprintf(stderr, "Recompute Relectance Level...\n");
								
								oldpr = -1;
								for(int i=0; i<rows; i++)
								{
									GDALRasterIO(hBandIn,  GF_Read, 0, i, cols, 1, pLineIn, cols, 1, GDALGetRasterDataType(hBandIn), 0, 0);
									
									for(int j=0; j<cols; j++) pLineOut[j] = getCorrectReflectance(pLineIn[j], method, vThreshold, NoDataValue);
									
									GDALRasterIO(hBandOut, GF_Write, 0, i, cols, 1, pLineOut, cols, 1, GDT_Float32, 0, 0 );
									
									oldpr=progress(band, i, rows-1, oldpr);
								}
																
								if(bSuccess) GDALSetRasterNoDataValue(hBandOut, NoDataValue);
								else NoDataValue = 0;
								
								///////////////////////////////////////////////
								// Recalc Statistics
								
								fputs("\nCreate Statistics...\n", stderr);
								
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

float getCorrectReflectance(float rTOA, int method, float threshold, double NoDataValue)
{
	float res = 0;
	switch(method)
	{
		case 1:
			return (rTOA<0)?0:((rTOA>1)?1:rTOA);
		case 2:
			if(threshold>1)
			{
				if(rTOA<0) res = 0;
				else
				{
					if(rTOA>=threshold) res = 1;
					else res = rTOA/threshold;
				}
			}else res = rTOA;
			return res;
		case 3:
			return (float)(rTOA<0)?NoDataValue:((rTOA>1)?NoDataValue:rTOA);
		case 4:
			if(threshold>1)
			{
				if(rTOA<0) res = NoDataValue;
				else
				{
					if(rTOA>=threshold) res = NoDataValue;
					else res = rTOA/threshold;
				}
			}else res = rTOA;
			return res;			
		default:
			return rTOA;
	}
}
