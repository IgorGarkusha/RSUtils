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

#include <gdal_priv.h>
#include <cpl_conv.h>
#include <ogrsf_frmts.h>
#include <ogr_core.h>
#include <ogr_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/utils.h"

#define PROG_VERSION "1"
#define DATE_VERSION "10.07.2016"

#define ERROR	-1

#ifdef __MSVC__
#define strcasecmp _stricmp
#endif

void doTif2Con(char fileName[], int headerFormat, int BandNumber, char separator, char SHPMaskFile[]);
int getHeaderFormat(char* arg1, char* arg2);
int getBand(char* arg1, char* arg2);
void printHelp();
void printHeader(int headerFormat, int BandNumber, int bands, int rows, int cols, char separator);
void printData(const float *** pdata, int headerFormat, int bands, int rows, int cols,
				float xOrigin, float yOrigin, float pixelWidth, float pixelHeight, char separator, OGRDataSourceH poDS);
				
int main(int argc, char* argv[])
{
	fprintf(stderr, "GeoTIFF to CONSOLE PRINTER\nVersion %s.%s. Free software. GNU General Public License, version 3\n", PROG_VERSION, DATE_VERSION);
	fprintf(stderr, "Copyright (C) 2016 Igor Garkusha.\nUkraine, Dnipro (Dnipropetrovsk)\n\n");
	
	if(!((argc==6)||(argc==8)||(argc==10)))
	{
		fputs("Input parameters not found!\n", stderr);
		printHelp();
		return 1;
	}
		
	int headerFormat = 1;
	int BandNumber = 0;
	char separator = '\x20';
	char SHPMaskFile[MAX_PATH] = "";
	
	for(int i=1; i<argc-2; i++)
	{
		if(0 == strcmp(argv[i], "-f")) headerFormat = getHeaderFormat(argv[i], argv[i+1]);
		else
		if(0 == strcmp(argv[i], "-b")) BandNumber = getBand(argv[i], argv[i+1]);
		else
		if(0 == strcmp(argv[i], "-s"))
		{
			if(0 == strcmp(argv[i+1], "space")) separator = '\x20';
			else
			if(0 == strcmp(argv[i+1], "tab")) separator = '\t';
			else
			if(1 == strlen(argv[i+1])) separator = argv[i+1][0];
			// else default -> '\x20'
		}
		else
		if(0 == strcmp(argv[i], "-m")) strcpy(SHPMaskFile, argv[i+1]);
	}

	OGRRegisterAll();
	GDALAllRegister();
		
	if((ERROR != headerFormat)&&(ERROR != BandNumber)) { doTif2Con(argv[argc-1], headerFormat, BandNumber, separator, SHPMaskFile); return 0; }
	else { printHelp(); return 1; }
}

void doTif2Con(char fileName[], int headerFormat, int BandNumber, char separator, char SHPMaskFile[])
{
	bool flMask = false;
	
	OGRDataSourceH poDS = NULL;
	OGRSFDriverH poDriver = NULL;
		
	if(strlen(SHPMaskFile)>0) 
	{
		flMask = true;
		poDS = OGROpen(SHPMaskFile, FALSE, &poDriver);
	}

	if(flMask && (poDS == NULL)) 
	{
		fputs("\nError open mask file!!!\n\n", stderr);
		return ;
	}

    GDALDatasetH  pDataset;
    GDALRasterBandH pBand;	
			
    pDataset = GDALOpen( fileName, GA_ReadOnly );

    if(pDataset!=NULL)
    {
		int bands = 0;
				
		if(0 == BandNumber) bands = GDALGetRasterCount( pDataset );
		else bands = 1;
		
		int cols = GDALGetRasterXSize(pDataset);
		int rows = GDALGetRasterYSize(pDataset);
				
		double adfGeoTransform[6];
		
		float xOrigin 	  = 0;
		float yOrigin 	  = 0;
		float pixelWidth  = 0;
		float pixelHeight = 0;
		
		if( GDALGetGeoTransform( pDataset, adfGeoTransform ) == CE_None )		
		{
			xOrigin 	= adfGeoTransform[0];
			yOrigin 	= adfGeoTransform[3];
			pixelWidth 	= adfGeoTransform[1];
			pixelHeight = adfGeoTransform[5];
		}
		
		float *** pdata = NULL;
		pdata = new float**[bands];
		for(int i=0; i<bands; i++) pdata[i] = new float*[rows];
		for(int i=0; i<bands; i++) for(int j=0; j<rows; j++) pdata[i][j] = new float[cols];
		for(int i=0; i<bands; i++) for(int j=0; j<rows; j++) for(int k=0; k<cols; k++) pdata[i][j][k] = 0;
		
		void *pbuf = NULL;
		pBand = GDALGetRasterBand(pDataset, 1);
		pbuf = CUtils::mallocData(pBand, pbuf, cols);
		
		printHeader(headerFormat, BandNumber, bands, rows, cols, separator);
		
		if(0 == BandNumber)
		{
			for(int i=1; i<=bands; i++)
			{
				pBand = GDALGetRasterBand(pDataset, i);
				for(int j=0; j<rows; j++)
				{					
					CUtils::getRasterLine(pBand, j, cols, pbuf);
					for(int k=0; k<cols; k++) pdata[i-1][j][k] = CUtils::getDataAsFloat(pBand, pbuf, k);
				}
			}
		}
		else
		{
				pBand = GDALGetRasterBand(pDataset, BandNumber);
				for(int j=0; j<rows; j++)
				{
					CUtils::getRasterLine(pBand, j, cols, pbuf);
					for(int k=0; k<cols; k++) pdata[0][j][k] = CUtils::getDataAsFloat(pBand, pbuf, k);
				}
		}
		
		CPLFree(pbuf);
		
		GDALClose(pDataset);
	
		printData((const float ***) pdata, headerFormat, bands, rows, cols,
				  xOrigin, yOrigin, pixelWidth, pixelHeight, separator, poDS);
		
		if(poDS != NULL) OGR_DS_Destroy(poDS);
		
		for(int i=0; i<bands; i++) for(int j=0; j<rows; j++) delete [] pdata[i][j];
		for(int i=0; i<bands; i++) delete [] pdata[i];
		delete [] pdata;
		pdata = NULL;
		fputs("\nProcessing COMPLETE.\n\n", stderr);
	}
	else
	fputs("\nError open input image!!!\n\n", stderr);
}

void printHeader(int headerFormat, int BandNumber, int bands, int rows, int cols, char separator)
{
	if(0 == BandNumber)
	{
		switch(headerFormat)
		{
			case 1:	 printf("ID%cX%cY%cXGEO%cYGEO", separator, separator, separator, separator); for(int i=0; i<bands; i++) printf("%cBand%d", separator, i+1);
					 printf("\n");
					 break;
					 
			case 2:  printf("XGEO%cYGEO", separator); for(int i=0; i<bands; i++) printf("%cBand%d", separator, i+1);
					 printf("\n");
					 break;
			case 3:  printf("%d\n",rows*cols);
					 break;						 
		}
	}
	else
	{
		switch(headerFormat)
		{
			case 1:	 printf("ID%cX%cY%cXGEO%cYGEO%cBand%d\n", separator, separator, separator, separator, separator,
							BandNumber); break;
			case 2:  printf("XGEO%cYGEO%cBand%d\n", separator, separator, BandNumber); break;
			case 3:  printf("%d\n",rows*cols); break;	
		}
	}
}

void printData(const float *** pdata, int headerFormat, int bands, int rows, int cols,
				float xOrigin, float yOrigin, float pixelWidth, float pixelHeight, char separator, OGRDataSourceH poDS)
{
	double x = 0;
	double y = 0;

	int id = 0;
	int oldpr = -1;
		
	bool flWriteValue = true;
	
	double *min = new double[bands];
	double *max = new double[bands];
	double *summ = new double[bands];
	double *count = new double[bands];
	bool *flFirst = new bool[bands];
	for(int i=0; i<bands; i++) { min[i] = max[i] = summ[i] = count[i] = 0; flFirst[i] = true; }
		
	switch(headerFormat)
	{
		case 1: // "ID X Y XGEO YGEO Band1 Band2 ..."
			for(int j=0; j<rows; j++)
			{
				for(int k=0; k<cols; k++)
				{
					//xOffset = int((x - xOrigin) / pixelWidth);
					//yOffset = int((y - yOrigin) / pixelHeight);
					x = (pixelWidth/2.0f + (double)(k) * pixelWidth) + xOrigin;
					y = (pixelHeight/2.0f + (double)(j) * pixelHeight) + yOrigin;
					
					flWriteValue = true;
					if(poDS != NULL)
					{
						if(!CUtils::insideInPolygons(poDS, x, y)) flWriteValue = false;
					}
					
					if(flWriteValue)
					{
						printf("%d%c%d%c%d%c%.6lf%c%.6lf", id, separator, k, separator, j, separator, x, separator, y);
						for(int i=0; i<bands; i++) 
						{
							printf("%c%.7f", separator, pdata[i][j][k]);
							
							if(flFirst[i])
							{
								min[i] = max[i] = pdata[i][j][k];
								flFirst[i] = false;
							}
							else
							{
								min[i] = (pdata[i][j][k]<min[i])?pdata[i][j][k]:min[i];
								max[i] = (pdata[i][j][k]>max[i])?pdata[i][j][k]:max[i];
							}
							summ[i] += pdata[i][j][k];
							count[i]++;
						}
						printf("\n");
						id++;
					}
				}				
				oldpr = CUtils::progress_ln(stderr, j, rows-1, oldpr);
			}
			break;
				
		case 2: // "XGEO YGEO Band1 Band2 ...\"
		case 3: // "Value" -> Value = ROWS x COLUMNS
		case 4: // NO HEADER
			for(int j=0; j<rows; j++)
			{
				for(int k=0; k<cols; k++)
				{
					//x = ((double)(k) * pixelWidth) + xOrigin;
					//y = ((double)(j) * pixelHeight) + yOrigin;					
					x = (pixelWidth/2.0f + (double)(k) * pixelWidth) + xOrigin;
					y = (pixelHeight/2.0f + (double)(j) * pixelHeight) + yOrigin;
					
					flWriteValue = true;
					if(poDS != NULL)
					{
						if(!CUtils::insideInPolygons(poDS, x, y)) flWriteValue = false;
					}
					
					if(flWriteValue)
					{
						printf("%.6lf%c%.6lf", x, separator, y);
						for(int i=0; i<bands; i++) 
						{
							printf("%c%.7f", separator, pdata[i][j][k]);
												
							if(flFirst[i])
							{
								min[i] = max[i] = pdata[i][j][k];
								flFirst[i] = false;
							}
							else
							{
								min[i] = (pdata[i][j][k]<min[i])?pdata[i][j][k]:min[i];
								max[i] = (pdata[i][j][k]>max[i])?pdata[i][j][k]:max[i];
							}
							summ[i] += pdata[i][j][k];
							count[i]++;
						}
						printf("\n");
					}
				}
				oldpr = CUtils::progress_ln(stderr, j, rows-1, oldpr);
			}
			break;
		case 5: // NO HEADER and ONLY BANDS VALUES!
			for(int j=0; j<rows; j++)
			{
				for(int k=0; k<cols; k++)
				{
					//x = ((double)(k) * pixelWidth) + xOrigin;
					//y = ((double)(j) * pixelHeight) + yOrigin;					
					x = (pixelWidth/2.0f + (double)(k) * pixelWidth) + xOrigin;
					y = (pixelHeight/2.0f + (double)(j) * pixelHeight) + yOrigin;
					
					flWriteValue = true;
					if(poDS != NULL)
					{
						if(!CUtils::insideInPolygons(poDS, x, y)) flWriteValue = false;
					}
					
					if(flWriteValue)
					{
						for(int i=0; i<(bands-1); i++) 
						{
							printf("%.7f%c", pdata[i][j][k], separator);
							
							if(flFirst[i])
							{
								min[i] = max[i] = pdata[i][j][k];
								flFirst[i] = false;
							}
							else
							{
								min[i] = (pdata[i][j][k]<min[i])?pdata[i][j][k]:min[i];
								max[i] = (pdata[i][j][k]>max[i])?pdata[i][j][k]:max[i];
							}
							summ[i] += pdata[i][j][k];
							count[i]++;
							
						}
						printf("%.7f\n", pdata[bands-1][j][k]);
						
						min[bands-1] = (pdata[bands-1][j][k]<min[bands-1])?pdata[bands-1][j][k]:min[bands-1];
						max[bands-1] = (pdata[bands-1][j][k]>max[bands-1])?pdata[bands-1][j][k]:max[bands-1];
						summ[bands-1] += pdata[bands-1][j][k];
						count[bands-1]++;
					}
				}
				oldpr = CUtils::progress_ln(stderr, j, rows-1, oldpr);
			}
	}
	
	fprintf(stderr, "\nStatistics:\n");
	for(int i = 0; i<bands; i++)
	{
		fprintf(stderr, "Band %d ---> Count Values: %d\tMIN Value: %lf\tMAX Value: %lf\tMEAN Value: %lf\n\n", i+1, (int)count[i], min[i], max[i], summ[i]/count[i]);
	}
	delete [] min;
	delete [] max;
	delete [] summ;
	delete [] count;
	delete [] flFirst;
}

int getHeaderFormat(char* arg1, char* arg2)
{
	int res = ERROR;
	if(0 == strcmp(arg1, "-f"))
	{
		res = atoi(arg2);
		if(!((res>=1) && (res<=5))) res = ERROR;
	}
	
	if(ERROR == res) fputs("ERROR: INVALID OUTPUT FORMAT NUMBER!\n", stderr);
	return res;
}

int getBand(char* arg1, char* arg2)
{
	int res = ERROR;
	if(0 == strcmp(arg1, "-b"))
	{
		if(0 == strcasecmp(arg2, "ALL")) res = 0;
		else
		{
			res = atoi(arg2);
			if(!(res>=1)) res = ERROR;
		}
	}
	if(ERROR == res) fputs("ERROR: INVALID BAND NUMBER!\n", stderr);
	return res;
}

void printHelp()
{
	fputs("Examples:\n", stderr);
	fputs("tif2con -f OutputFormatNumber -b BandNumber [-s Separator] [-m SHPMaskFile] input.tif > imgdata.txt\n", stderr);
	fputs("tif2con -f OutputFormatNumber -b BandNumber [-s Separator] [-m SHPMaskFile] multi_band_input.tif > imgdata.txt\n", stderr);
	fputs("OutputFormatNumber:\n", stderr);
	fputs("\t1: HEADER --> \"ID X Y XGEO YGEO Band1 Band2 ...\"\n", stderr);
	fputs("\t2: HEADER --> \"XGEO YGEO Band1 Band2 ...\"\n", stderr);
	fputs("\t3: HEADER --> \"Value\"\n\t\tValue = ROWS x COLUMNS\n", stderr);
	fputs("\t4: NO HEADER\n", stderr);
	fputs("\t5: NO HEADER and ONLY BANDS VALUES!\n", stderr);
	fputs("BandNumber: 1, 2, ..., ALL\n", stderr);
	fputs("Separator: space (default), tab, any symbol (\":\", \";\", etc.)\n", stderr);
	fputs("SHPMaskFile: ESRI ShapeFile Format File with POLYGON objects!\n\n", stderr);
	fputs("tif2con -f 2 -b 1 input.tif > imgdata.txt\n", stderr);
	fputs("tif2con -f 1 -b ALL multi_band_input.tif > imgdata.txt\n", stderr);
	fputs("tif2con -f 2 -b 3 multi_band_input.tif > imgdata.txt\n", stderr);
	fputs("tif2con -f 5 -b ALL multi_band_input.tif > imgdata.txt\n", stderr);
	fputs("tif2con -f 2 -b 1 -s space input.tif > imgdata.txt\n", stderr);
	fputs("tif2con -f 2 -b 1 -s tab input.tif > imgdata.txt\n", stderr);
	fputs("tif2con -f 2 -b 1 -s \";\" input.tif > imgdata.txt\n", stderr);
	fputs("tif2con -f 2 -b 1 -m masks.shp input.tif > imgdata.txt\n", stderr);
	fputs("tif2con -f 2 -b 1 -s \";\" -m masks.shp input.tif > imgdata.txt\n\n", stderr);
}
