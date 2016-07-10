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
#define COUNT_OUT_BANDS 4

#define PROG_VERSION "1"
#define DATE_VERSION "10.07.2016"

void printHelp();
//bool loadTCCoeffTableForSensor(const char * fileName);
float getTasseledCapValue(float v, int sensorIndexFlag, int resultBandNumber, int currentBandNumber);

float OLI_TCCoeff[COUNT_OUT_BANDS][6]={ {0.3029, 0.2786, 0.4733, 0.5599, 0.5080, 0.1872},
										{-0.2941, -0.2430, -0.5424, 0.7276, 0.0713, -0.1608},
										{0.1511, 0.1973, 0.3283, 0.3407, -0.7117, -0.4559},
										{-0.8239, 0.0849, 0.4396, -0.0580, 0.2013, -0.2773}
									  //{-0.3294, 0.0557, 0.1056, 0.1855, -0.4349, 0.8085},
									  //{0.1079, -0.9023, 0.4119, 0.0575, -0.0259, 0.0252}
									  };
//int OLI_TCBands[6]={2, 3, 4, 5, 6, 7};

float ETM_TCCoeff[COUNT_OUT_BANDS][6]={ {0.3561, 0.3972, 0.3904, 0.6966, 0.2286, 0.1596},
										{-0.3344, -0.3544, -0.4556, 0.6966, -0.0242, -0.2630},
										{0.2626, 0.2141, 0.0926, 0.0656, -0.7629, -0.5388},
										{0.0805, -0.0498, 0.1950, -0.1327, 0.5752, -0.7775}
									  //{-0.7252, -0.0202, 0.6683, 0.0631, -0.1494, -0.0274},
									  //{0.4000, -0.8172, 0.3832, 0.0602, -0.1095, 0.0985}
									  };
//int ETM_TCBands[6]={1, 2, 3, 4, 5, 7};

float TM5_TCCoeff[COUNT_OUT_BANDS][7]={ {0.2909, 0.2493, 0.4806, 0.5568, 0.4438, 0.1706, 10.3695},
										{-0.2728, -0.2174, -0.5508, 0.7221, 0.0733, -0.1648, -0.7310},
										{0.1446, 0.1761, 0.3322, 0.3396, -0.6210, -0.4186, -3.3828},
										{0.8461, -0.0731, -0.4640, -0.0032, -0.0492, -0.0119, 0.7879}
									  };
//int TM5_TCBands[7]={1, 2, 3, 4, 5, 7, -1};

float TM4_TCCoeff[COUNT_OUT_BANDS][6]={ {0.3037, 0.2793, 0.4743, 0.5585, 0.5082, 0.1863},
										{-0.2848, -0.2435, -0.5436, 0.7243, 0.0840, -0.1800},
										{0.1509, 0.1973, 0.3279, 0.3406, -0.7112, -0.4572},
										{0.8832, -0.0819, -0.4580, -0.0032, -0.0563, 0.0130}
									  };
//int TM4_TCBands[6]={1, 2, 3, 4, 5, 7};


float S2AMSI1_TCCoeff[COUNT_OUT_BANDS][4]={ {0.2909, 0.2493, 0.4806, 0.5568},
											{-0.2728, -0.2174, -0.5508, 0.7221},
											{0.1446, 0.1761, 0.3322, 0.3396},
											{0.8461, -0.0731, -0.4640, -0.0032}
										  };
float S2AMSI2_TCCoeff[COUNT_OUT_BANDS][6]={ {0.2909, 0.2493, 0.4806, 0.5568, 0.4438, 0.1706},
										    {-0.2728, -0.2174, -0.5508, 0.7221, 0.0733, -0.1648},
										    {0.1446, 0.1761, 0.3322, 0.3396, -0.6210, -0.4186},
										    {0.8461, -0.0731, -0.4640, -0.0032, -0.0492, -0.0119}
										  };
//int S2AMSI1_TCBands[4]={1, 2, 3, 4}; 			// bands (10m): 2,3,4,8
//int S2AMSI2_TCBands[6]={1, 2, 3, 4, 5, 6}; 	// bands (10m): 2,3,4,8 and bands(20m): 11,12

typedef enum { OLI, ETM, TM5, TM4, S2AMSI10, S2AMSI } SENSOR;

int main(int argc, char* argv[])
{
	fprintf(stderr, "TASSELED CAP TRANSFORMATION\nVersion %s.%s. Free software. GNU General Public License, version 3\n", PROG_VERSION, DATE_VERSION);
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
	int sensorIndexFlag = OLI;
	
	if( strcmp(argv[argc-2], "oli") == 0) 	sensorIndexFlag = OLI;
	else
	if( strcmp(argv[argc-2], "etm") == 0) 	sensorIndexFlag = ETM;
	else
	if( strcmp(argv[argc-2], "tm4") == 0) 	sensorIndexFlag = TM4;
	else
	if( strcmp(argv[argc-2], "tm5") == 0) 	sensorIndexFlag = TM5;
	else
	//if( strcmp(argv[argc-2], "msi10") == 0) sensorIndexFlag = S2AMSI10;
	//else
	if( strcmp(argv[argc-2], "msi") == 0) 	sensorIndexFlag = S2AMSI;
		    
    pSrcDataset = GDALOpen( argv[1], GA_ReadOnly );
    
    int bands = 0;
    
    if(pSrcDataset!=NULL)
    {
		if(CUtils::isFloat32DataType(pSrcDataset))
		{
			bands = GDALGetRasterCount(pSrcDataset);
			if( ((bands == 6)||(bands == 4)) )
			{
					pSrcBand = GDALGetRasterBand(pSrcDataset, 1);
					if(pSrcBand != NULL)
					{
							int cols  = GDALGetRasterBandXSize(pSrcBand);
							int rows  = GDALGetRasterBandYSize(pSrcBand);

							float NoDataValue = 0;

							pDriver = GDALGetDriverByName("GTiff");
							char **papszOptions = NULL;
							pDstDataset = GDALCreate(pDriver, argv[2], cols, rows, COUNT_OUT_BANDS, GDT_Float32, papszOptions);
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
							
							
							if(flagNoData == false)
							{
								for(int resultBandNumber=1; resultBandNumber <= COUNT_OUT_BANDS; resultBandNumber++)
								{
									fprintf(stderr, "Processing for band %d...\n", resultBandNumber);
									
									pDstBand = GDALGetRasterBand(pDstDataset, resultBandNumber);

									int pr = CUtils::progress_ln_ex(stderr, 0, 0, START_PROGRESS);
									for(int i=0; i<rows; i++)
									{
										for(int j=0; j<cols; j++) pDstLine[j] = 0;
								
										for(int currentBandNumber=1; currentBandNumber <= bands; currentBandNumber++)
										{
											pSrcBand = GDALGetRasterBand(pSrcDataset, currentBandNumber);		

											GDALRasterIO(pSrcBand, GF_Read, 0, i, cols, 1, pSrcLine, cols, 1, GDALGetRasterDataType(pSrcBand), 0, 0 );
										
											for(int j=0; j<cols; j++) pDstLine[j] += getTasseledCapValue(pSrcLine[j], sensorIndexFlag, resultBandNumber, currentBandNumber);
										}
										
										if(sensorIndexFlag == TM5) for(int j=0; j<cols; j++) pDstLine[j] += TM5_TCCoeff[resultBandNumber][6];
										
										GDALRasterIO(pDstBand, GF_Write, 0, i, cols, 1, pDstLine, cols, 1, GDT_Float32, 0, 0 );
										
										pr = CUtils::progress_ln_ex(stderr, i, rows, pr);
									}
									CUtils::progress_ln_ex(stderr, 0, 0, END_PROGRESS);
								}
							}
							else // WITH NODATA VALUE - pixel(1,1)
							{
								for(int resultBandNumber=1; resultBandNumber <= COUNT_OUT_BANDS; resultBandNumber++)
								{
									fprintf(stderr, "Processing for band %d...\n", resultBandNumber);
									
									pDstBand = GDALGetRasterBand(pDstDataset, resultBandNumber);
									
									int pr = CUtils::progress_ln_ex(stderr, 0, 0, START_PROGRESS);
									for(int i=0; i<rows; i++)
									{
										for(int j=0; j<cols; j++) pDstLine[j] = 0;
								
										for(int currentBandNumber=1; currentBandNumber <= bands; currentBandNumber++)
										{
											pSrcBand = GDALGetRasterBand(pSrcDataset, currentBandNumber);		
											NoDataValue = CUtils::getFloatNoDataValueAsBackground(pSrcBand);
											
											GDALRasterIO(pSrcBand, GF_Read, 0, i, cols, 1, pSrcLine, cols, 1, GDALGetRasterDataType(pSrcBand), 0, 0 );
										
											for(int j=0; j<cols; j++) 
											{
												if(NoDataValue == pSrcLine[j]) pDstLine[j] = NoDataValue;
												else pDstLine[j] += getTasseledCapValue(pSrcLine[j], sensorIndexFlag, resultBandNumber, currentBandNumber);
											}
										}
										
										if(sensorIndexFlag == TM5)
										{
											for(int j=0; j<cols; j++) if(NoDataValue != pDstLine[j]) pDstLine[j] += TM5_TCCoeff[resultBandNumber][6];
										}
										
										GDALRasterIO(pDstBand, GF_Write, 0, i, cols, 1, pDstLine, cols, 1, GDT_Float32, 0, 0 );
										
										pr = CUtils::progress_ln_ex(stderr, i, rows, pr);
									}
									CUtils::progress_ln_ex(stderr, 0, 0, END_PROGRESS);
								}
							}
							
							CPLFree(pSrcLine); pSrcLine = NULL;
							CPLFree(pDstLine); pDstLine = NULL;
							
							if(flagNoData) CUtils::calculateFloatGeoTIFFStatistics(pDstDataset, -1, true);
							else CUtils::calculateFloatGeoTIFFStatistics(pDstDataset, -1, false);
							
							fputs("Output band:\n\tband1: Brightness, band2: Greenness (Vegetation), band3: Wetness, band4: Haze\n\n", stderr);
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

    return 0;
}

/*
bool loadTCCoeffTableForSensor(const char * fileName)
{
	FILE* fin = NULL;
	if((fin = fopen(fileName, "rt")) != NULL)
	{
		// ...
		fclose(fin);
		return true;
	}
	else return false;
}
*/

float getTasseledCapValue(float v, int sensorIndexFlag, int resultBandNumber, int currentBandNumber)
{
	switch(sensorIndexFlag)
	{
		case OLI:
			return v*OLI_TCCoeff[resultBandNumber-1][currentBandNumber-1];
			
		case ETM:
			return v*ETM_TCCoeff[resultBandNumber-1][currentBandNumber-1];
		
		case TM5:
			return v*TM5_TCCoeff[resultBandNumber-1][currentBandNumber-1];
		
		case TM4:
			return v*TM4_TCCoeff[resultBandNumber-1][currentBandNumber-1];
		
		//case S2AMSI10:
			//return v*S2AMSI1_TCCoeff[resultBandNumber-1][currentBandNumber-1];
		
		case S2AMSI:
			return v*S2AMSI2_TCCoeff[resultBandNumber-1][currentBandNumber-1];
	}
	return 0;
}

void printHelp()
{
	fputs("Example:\n", stderr);
	fputs("tcap Input_Multiband.tif Output_TC_Multiband.tif SensorIndexFlag NoDataModeFlag\n", stderr);
	fputs("tcap test.tif tc.tif oli 0\n", stderr);
	fputs("tcap test.tif tc.tif oli 1\n", stderr);
	fputs("tcap test.tif tc.tif msi 0\n", stderr);
	fputs("tcap test.tif tc.tif msi 1\n\n", stderr);
		
	fputs("SensorIndexFlag:\n", stderr);
	fputs("\toli   -- Landsat-8 OLI   (bands: 2,3,4,5,6,7)\n", stderr);
	fputs("\tetm   -- Landsat-7 ETM+  (bands: 1,2,3,4,5,7)\n", stderr);
	fputs("\ttm5   -- Landsat-5 TM    (bands: 1,2,3,4,5,7)\n", stderr);
	fputs("\ttm4   -- Landsat-4 TM    (bands: 1,2,3,4,5,7)\n", stderr);
	//fputs("\tmsi10 -- Sentinel-2A MSI (bands: 2,3,4,8)\n", stderr);
	fputs("\tmsi   -- Sentinel-2A MSI (based on Landsat-5 TM coeff.; bands: 2,3,4,8,11,12)\n\n", stderr);
	
	fputs("NoDataModeFlag: Value of pixel with coordinates (1,1)\n\n", stderr);
}
