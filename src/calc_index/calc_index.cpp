/*
 * Project: Remote Sensing Utilities (Extentions GDAL/OGR)
 * Author:  Igor Garkusha <rsutils.gis@gmail.com>
 *          Ukraine, Dnipro (Dnipropetrovsk)
 * 
 * Copyright (C) 2012-2016, Igor Garkusha <rsutils.gis@gmail.com>
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

#define PROG_VERSION "3"
#define DATE_VERSION "06.12.2016"

int progress(int index, int count, int oldpersent);
float getIndexValue(float vBand1, float vBand2, int typeIndexFlag, bool *flState);
float getIndexValue(float vBand1, float vBand2, float vBand3, int typeSubIndexFlag, bool *flState);

float getDataAsFloat(void* pbuf, int index, GDALDataType dataType);

bool isNoData(void* pbuf1, int index1, GDALDataType dataType1, float NoDataBandValue1,
			  void* pbuf2, int index2, GDALDataType dataType2, float NoDataBandValue2,
			  void* pbuf3, int index3, GDALDataType dataType3, float NoDataBandValue3);
			  
bool isNoData(void* pbuf1, int index1, GDALDataType dataType1, float NoDataBandValue1,
			  void* pbuf2, int index2, GDALDataType dataType2, float NoDataBandValue2);

int main(int argc, char* argv[])
{
	fprintf(stderr, "CALCULATION OF VEGETATION INDEX\nVersion %s.%s. Free software. GNU General Public License, version 3\n", PROG_VERSION, DATE_VERSION);
	fprintf(stderr, "Copyright (C) 2012-2016 Igor Garkusha.\nUkraine, Dnipro (Dnipropetrovsk)\n\n");
	
	if((argc!=6)&&(argc!=7))
	{
		fputs("Input parameters not found!\n", stderr);
		fputs("Example:\n", stderr);
		
		fputs("calc_index Band1.tif Band2.tif index_output.tif TypeIndexFlag NoDataModeFlag\n", stderr);
		fputs("calc_index Band1.tif Band2.tif Band3.tif index_output.tif 10<SubIndex> NoDataModeFlag\n", stderr);
		fputs("calc_index Band1.tif Band2.tif index_ndvi.tif 1 1\n", stderr);
		fputs("calc_index Band1.tif Band2.tif Band3.tif index_evi.tif 101 1\n", stderr);		
				
		fputs("\n", stderr);
		
		fputs("Type Index Flag:\n", stderr);
		
		fputs("1 -- (Band1 - Band2)/(Band1 + Band2)\n", stderr);
		fputs("      NDVI: (NIR-RED)/(NIR+RED)         -- Normalized Difference Vegetation Index\n", stderr);
		fputs("      GNDVI:(NIR-GREEN)/(NIR+GREEN)     -- Green NDVI\n", stderr);
		fputs("      BNDVI:(NIR-BLUE)/(NIR+BLUE)       -- Blue NDVI\n", stderr);
		fputs("      NBR:  (NIR-SWIR2)/(NIR+SWIR2)     -- Normalized Burn Ratio\n", stderr);
		fputs("      NDSII:(GREEN-SWIR1)/(GREEN+SWIR1) -- Normalized Difference Snow/Ice Index\n", stderr);
		fputs("      NDWI: (NIR-SWIR1)/(NIR+SWIR1)     -- Normalized Difference Water Index\n", stderr);
		fputs("      NDSI: (SWIR1-SWIR2)/(SWIR1+SWIR2) -- Normalized Difference Salinity Index\n", stderr);
		fputs("      NDSI2:(RED-NIR)/(RED+NIR)         -- Normalized Difference Salinity Index 2\n", stderr);
		
		fputs("2 -- (Band1/Band2)\n", stderr);
		fputs("      RVI:  (NIR)/(RED)                 -- Simple Ratio 800/670 Ratio Vegetation Index\n", stderr);
		fputs("      SW:   (SWIR1)/(SWIR2)             -- Simple Ratio SWIRI/SWIRII Clay Minerals\n", stderr);
		fputs("      RB:   (Red)/(Blue)                -- Simple Ratio Red/Blue Iron Oxide\n", stderr);
		fputs("      SN:   (SWIR1)/(NIR)               -- Simple Ratio SWIRI/NIR Ferrous Minerals\n", stderr);
		
		fputs("3 -- (Band1 - Band2)\n", stderr);
		fputs("      DVI:  (NIR - RED)                 -- Difference Vegetation Index\n", stderr);
		
		fputs("4 -- (Band1)/(Band1+Band2)\n", stderr);
		fputs("      IPVI: (NIR)/(NIR+RED)             -- Infrared Percentage Vegetation Index\n", stderr);
		
		fputs("5 -- EVI2 -- Enhanced Vegetation Index 2\n", stderr);
		fputs("            2.4*[(Band1 - Band2)/(Band1 + Band2 + 1)]\n", stderr);
		fputs("            2.4*[(NIR - RED)/(NIR + RED + 1)]\n", stderr);
		
		fputs("6 -- SAVI -- Soil-Adjusted Vegetation Index\n", stderr);
		fputs("            [(Band1 - Band2)/(Band1 + Band2 + L)]*[1 + L]\n", stderr);
		fputs("            L = 0.5\n", stderr);
		fputs("            [(NIR - RED)/(NIR + RED + 0.5)]*[1 + 0.5]\n", stderr);
		
		fputs("7 -- SI -- Salinity Index\n", stderr);
		fputs("            SQRT(Band1 * Band2)\n", stderr);
		fputs("            SQRT(BLUE * RED)  <-- SI\n", stderr);
		fputs("            SQRT(GREEN * RED) <-- SI1\n", stderr);
//-		fputs("            (Band1 + Band2)/2.0\n", stderr);
//-		fputs("            (GREEN + RED)/2.0\n", stderr);
		
		fputs("8 -- BI -- Brightness Index\n", stderr);
		fputs("            SQRT(Band1^2 + Band2^2)\n", stderr);
		fputs("            SQRT( (RED * RED) + (NIR * NIR) )\n", stderr);
		fputs("            SQRT( (GREEN * GREEN) + (RED * RED) )\n", stderr);
		
		fputs("9 -- CI -- Chlorophyll Index\n", stderr);
		fputs("            (Band1/Band2)-1\n", stderr);
		fputs("            (NIR/GREEN)-1               -- Green Chlorophyll Index\n", stderr);
		fputs("            (NIR/RED)-1                 -- Red-edge Chlorophyll Index\n", stderr);
		
		fputs("10 -- WDRVI -- Wide dynamic range vegetation index\n", stderr);
		fputs("            (0.2*Band1-Band2)/(0.2*Band1+band2)\n", stderr);
		fputs("            (0.2*NIR-RED)/(0.2*NIR+RED)\n", stderr);
		
//?		fputs("10 -- LAI -- 0.2273*exp(4.972*NDVI)     -- White et al. 1997\n", stderr);
//		fputs("      NDVI: (NIR-RED)/(NIR+RED) --> (Band1 - Band2)/(Band1 + Band2)\n", stderr);
				
		fputs("10? -- Three channels combination!\n", stderr);
		fputs("Type SubIndex Flag:\n", stderr);
		fputs("  1 -- EVI    -- Enhanced Vegetation Index\n", stderr);
		fputs("                 2.5*[(Band1 - Band2)/(1 + Band1 + 6*Band2 - 7.5*Band3)]\n", stderr);
		fputs("                 2.5*[(NIR - RED)/(1 + NIR + 6*RED - 7.5*BLUE)]\n", stderr);
		fputs("  2 -- GBNDVI -- Green-Blue NDVI\n", stderr);
		fputs("            	    [Band1 - (Band2 + Band3)]/[Band1 + (Band2 + Band3)]\n", stderr);
		fputs("                 [NIR - (GREEN + BLUE)]/[NIR + (GREEN + BLUE)]\n", stderr);
		fputs("  3 -- VARI   -- Visible Atmospherically Resistant Index\n", stderr);
		fputs("                 (Band1 - Band2)/(Band1 + Band2 + Band3)\n", stderr);
		fputs("                 (GREEN - RED)/(GREEN + RED + BLUE)\n", stderr);
		fputs("  4 -- LAI    -- Green Leaf Area Index\n", stderr);
		fputs("                 Reference: Boegh, E., H. Soegaard, N. Broge,\n",stderr);
		fputs("                 C. Hasager, N. Jensen, K. Schelde, and A. Thomsen.\n",stderr);
		fputs("                 \"Airborne Multi-spectral Data for Quantifying Leaf Area Index,\n",stderr);
		fputs("                 Nitrogen Concentration and Photosynthetic Efficiency in Agriculture\".\n",stderr);
		fputs("                 Remote Sensing of Environment 81, no. 2-3 (2002): 179-193.\n", stderr);
		fputs("                 3.618*EVI-0.118\n", stderr);
		fputs("                 3.618*(2.5*[(Band1 - Band2)/(1 + Band1 + 6*Band2 - 7.5*Band3)])-0.118\n", stderr);
		fputs("                 3.618*(2.5*[(NIR - RED)/(1 + NIR + 6*RED - 7.5*BLUE)])-0.118\n", stderr);
		fputs("  5 -- NMDI   -- Normalized Multi-band Drought Index\n", stderr);
		fputs("                 [Band1 - (Band2 - Band3)]/[Band1 + (Band2 - Band3)]\n", stderr);
		fputs("                 [NIR - (SWIR1640 - SWIR2130)]/[NIR + (SWIR1640 - SWIR2130)]\n", stderr);
		fputs("                 Reference: Wang, L., and J. Qu. \"NMDI: A Normalized Multi-Band Drought Index\n", stderr);
		fputs("                 for Monitoring Soil and Vegetation Moisture with Satellite Remote Sensing.\"\n", stderr);
		fputs("                 Geophysical Research Letters 34 (2007): L20405.\n", stderr);
		
		fputs("\n", stderr);
		
		return 1;
	}
	
    GDALDatasetH  pDataset1 = NULL;
    GDALDatasetH  pDataset2 = NULL;
    GDALDatasetH  pDataset3 = NULL;
    GDALDatasetH  pDataset4 = NULL;
    GDALRasterBandH pBand1 = NULL;
    GDALRasterBandH pBand2 = NULL;
    GDALRasterBandH pBand3 = NULL;
    GDALRasterBandH pBand4 = NULL;
    GDALDriverH pDriver = NULL;

    GDALAllRegister();
	
	bool flagNoData = false;
	int typeIndexFlag = 0;
	char outputFileName[512] = "";
	
	if(argc==6)
	{
		strcpy(outputFileName, argv[3]);
		typeIndexFlag = atoi(argv[4]);
		flagNoData = (bool)(atoi(argv[5]));
	}
	else // argc == 7
	{
		strcpy(outputFileName, argv[4]);
		typeIndexFlag = atoi(argv[5]); // typeIndexFlag > 100!
		flagNoData = (bool)(atoi(argv[6]));
	}
    
    pDataset1 = GDALOpen( argv[1], GA_ReadOnly );
    pDataset2 = GDALOpen( argv[2], GA_ReadOnly );
    
    bool flContinue = true;
    
    if(100 < typeIndexFlag) 
		pDataset4 = GDALOpen( argv[3], GA_ReadOnly );
	    
    if(pDataset4 != NULL)
		if(!CUtils::isFloat32DataType(pDataset4)) flContinue = false;
    
    if( (pDataset1!=NULL) && (pDataset2!=NULL) && flContinue )
    {
		if( CUtils::isFloat32DataType(pDataset1) && CUtils::isFloat32DataType(pDataset2) )
		{
				
		
		pBand1 = GDALGetRasterBand(pDataset1, 1);
		pBand2 = GDALGetRasterBand(pDataset2, 1);
		if(100 < typeIndexFlag) 
		pBand4 = GDALGetRasterBand(pDataset4, 1);

		int cols = GDALGetRasterBandXSize(pBand1);
		int rows = GDALGetRasterBandYSize(pBand1);

		pDriver = GDALGetDriverByName("GTiff");
		
		char **papszOptions = NULL;
		pDataset3 = GDALCreate(pDriver, outputFileName, cols, rows, 1, GDT_Float32, papszOptions);

		double adfGeoTransform[6]={0};
		GDALGetGeoTransform(pDataset1, adfGeoTransform );
		const char *szProjection = GDALGetProjectionRef(pDataset1);
		
		GDALSetGeoTransform(pDataset3, adfGeoTransform );
		GDALSetProjection(pDataset3, szProjection );

		pBand3 = GDALGetRasterBand(pDataset3, 1);
        		
		void *pline1 = NULL; // for read data buffer
		void *pline2 = NULL; // for read data buffer
		float *pline3 = NULL; // for write data buffer and read statistics
		void *pline4 = NULL; // for read data buffer
        pline1 = CPLMalloc(sizeof(GDALGetRasterDataType(pBand1))*cols);
        pline2 = CPLMalloc(sizeof(GDALGetRasterDataType(pBand2))*cols);
        pline3 = (float *)CPLMalloc(sizeof(float)*cols);
        
        if(100 < typeIndexFlag) pline4 = CPLMalloc(sizeof(GDALGetRasterDataType(pBand4))*cols);
        
        fputs("\nCalculate index...\n", stderr);
        
        bool flFirst = true;
        double min = 0, max = 0, mean = 0, stddev = 0;
        double summ = 0;
        int count = 0;
        
        int oldpr = -1;
        
        float NoDataBand1 = NODATAVALUE;
        float NoDataBand2 = NODATAVALUE;
        float NoDataBand3 = NODATAVALUE;
             
        bool flState = false;
        
        if(flagNoData)
        {
			GDALRasterIO(pBand1, GF_Read, 0, 0, cols, 1, pline1, cols, 1, GDALGetRasterDataType(pBand1), 0, 0 );
			GDALRasterIO(pBand2, GF_Read, 0, 0, cols, 1, pline2, cols, 1, GDALGetRasterDataType(pBand2), 0, 0 );
			
			NoDataBand1 = getDataAsFloat(pline1, 1, GDALGetRasterDataType(pBand1));
			NoDataBand2 = getDataAsFloat(pline2, 1, GDALGetRasterDataType(pBand2));
			if(100 < typeIndexFlag) { 
				GDALRasterIO(pBand4, GF_Read, 0, 0, cols, 1, pline4, cols, 1, GDALGetRasterDataType(pBand4), 0, 0 );
				NoDataBand3 = getDataAsFloat(pline4, 1, GDALGetRasterDataType(pBand4));
			}
		}
        
        if(flagNoData==false)
        {
			for(int i=0; i<rows; i++)
			{
				GDALRasterIO(pBand1, GF_Read, 0, i, cols, 1, pline1, cols, 1, GDALGetRasterDataType(pBand1), 0, 0 );
				GDALRasterIO(pBand2, GF_Read, 0, i, cols, 1, pline2, cols, 1, GDALGetRasterDataType(pBand2), 0, 0 );
				
				if(100 < typeIndexFlag) 
				{
					GDALRasterIO(pBand4, GF_Read, 0, i, cols, 1, pline4, cols, 1, GDALGetRasterDataType(pBand4), 0, 0 );
					for(int j=0; j<cols; j++) 
					{		
							float res = getIndexValue(getDataAsFloat(pline1, j, GDALGetRasterDataType(pBand1)),
													  getDataAsFloat(pline2, j, GDALGetRasterDataType(pBand2)),
													  getDataAsFloat(pline4, j, GDALGetRasterDataType(pBand4)),
													  typeIndexFlag, &flState);
							if(flState)
							{
								if(flFirst) { min = res; max = res; flFirst = false; }
								else 
								{  
									if(res<min) min = res;
									if(res>max) max = res;
								}
								count++;
								mean += res;
							}
						pline3[j] = res;
					}

				}
				else
				{
					for(int j=0; j<cols; j++) 
					{		
							float res = getIndexValue(getDataAsFloat(pline1, j, GDALGetRasterDataType(pBand1)),
													  getDataAsFloat(pline2, j, GDALGetRasterDataType(pBand2)),
													  typeIndexFlag, &flState);

							if(flState)
							{
								if(flFirst) { min = res; max = res; flFirst = false; }
								else 
								{  
									if(res<min) min = res;
									if(res>max) max = res;
								}
								count++;
								mean += res;
							}
						pline3[j] = res;
					}
				}
				
				GDALRasterIO(pBand3, GF_Write, 0, i, cols, 1, pline3, cols, 1, GDT_Float32, 0, 0 );
				
				oldpr = progress(i, rows-1, oldpr);
			}
			
			mean /= (double)(count);
			
			fputs("\nCalculate statistics...\n", stderr);
			
			oldpr = -1;
						
				for(int i=0; i<rows; i++)
				{
					GDALRasterIO(pBand3, GF_Read, 0, i, cols, 1, pline3, cols, 1, GDT_Float32, 0, 0 );
					for(int j=0; j<cols; j++) summ += (pline3[j]-mean)*(pline3[j]-mean);
					oldpr = progress(i, rows-1, oldpr);
				}
		}
		else // if(flagNoData==true)
		{
			for(int i=0; i<rows; i++)
			{
				GDALRasterIO(pBand1, GF_Read, 0, i, cols, 1, pline1, cols, 1, GDALGetRasterDataType(pBand1), 0, 0 );
				GDALRasterIO(pBand2, GF_Read, 0, i, cols, 1, pline2, cols, 1, GDALGetRasterDataType(pBand2), 0, 0 );
				
				if(100 < typeIndexFlag) 
				{
					GDALRasterIO(pBand4, GF_Read, 0, i, cols, 1, pline4, cols, 1, GDALGetRasterDataType(pBand4), 0, 0 );
					for(int j=0; j<cols; j++) 
					{		
						float res = NODATAVALUE;
						
						if(isNoData(pline1,j,GDALGetRasterDataType(pBand1),NoDataBand1,
									pline2,j,GDALGetRasterDataType(pBand2),NoDataBand2,
									pline4,j,GDALGetRasterDataType(pBand4),NoDataBand3))
						{
							res = getIndexValue(getDataAsFloat(pline1, j, GDALGetRasterDataType(pBand1)),
												getDataAsFloat(pline2, j, GDALGetRasterDataType(pBand2)),
												getDataAsFloat(pline4, j, GDALGetRasterDataType(pBand4)),
												typeIndexFlag, &flState);
							
							if(flState)
							{
								if(flFirst) { min = res; max = res; flFirst = false; }
								else 
								{  
									if(res<min) min = res;
									if(res>max) max = res;
								}
								count++;
								mean += res;
							}
						}
						pline3[j] = res;
					}
				}
				else
				{
					for(int j=0; j<cols; j++) 
					{		
						float res = NODATAVALUE;
						
						if(isNoData(pline1,j,GDALGetRasterDataType(pBand1),NoDataBand1,
									pline2,j,GDALGetRasterDataType(pBand2),NoDataBand2))
						{
							res = getIndexValue(getDataAsFloat(pline1, j, GDALGetRasterDataType(pBand1)),
												getDataAsFloat(pline2, j, GDALGetRasterDataType(pBand2)),
												typeIndexFlag, &flState);
							
							if(flState)
							{
								if(flFirst) { min = res; max = res; flFirst = false; }
								else 
								{  
									if(res<min) min = res;
									if(res>max) max = res;
								}
								count++;
								mean += res;
							}
						}
						
						pline3[j] = res;
					}
				}
				
				GDALRasterIO(pBand3, GF_Write, 0, i, cols, 1, pline3, cols, 1, GDT_Float32, 0, 0 );
				
				oldpr = progress(i, rows-1, oldpr);
			}
			
			mean /= (double)(count);
			
			fputs("\nCalculate statistics...\n", stderr);

			oldpr = -1;
				
				for(int i=0; i<rows; i++)
				{
					GDALRasterIO(pBand3, GF_Read, 0, i, cols, 1, pline3, cols, 1, GDT_Float32, 0, 0 );
					for(int j=0; j<cols; j++) if(pline3[j]!=NODATAVALUE) summ += (pline3[j]-mean)*(pline3[j]-mean);
					oldpr = progress(i, rows-1, oldpr);
				}
		}
		
		summ /= (double)(count-1);
		stddev = sqrt(summ);
		
		GDALSetRasterStatistics(pBand3, min, max, mean, stddev);
		if(flagNoData) GDALSetRasterNoDataValue(pBand3, NODATAVALUE);
		
		CPLFree(pline1);
		CPLFree(pline2);
		CPLFree(pline3);
		if(100 < typeIndexFlag) CPLFree(pline4);
		if(pDataset3!=NULL) { GDALClose(pDataset3); }
		
	 }else fputs("\nERROR: input data is not Float32-data type!!!\n", stderr);
	
	
	 
	if(pDataset1!=NULL) { GDALClose(pDataset1); }
	if(pDataset2!=NULL) { GDALClose(pDataset2); }
	if(100 < typeIndexFlag) if(pDataset4!=NULL) { GDALClose(pDataset4); }
	}
	
	fputs("\nEnd Processing.\n\n", stderr);
        
    return 0;
}

int progress(int index, int count, int oldpersent)
{
	int pr = index*100/count;
	if(oldpersent!= pr)	fprintf(stderr, "\rProgress: %d%%", pr);
	return pr;
}

// Calculation for TWO channels combination!
float getIndexValue(float vBand1, float vBand2, int typeIndexFlag, bool *flState)
{
	float index = NODATAVALUE;
	*flState = false;
		
	vBand1 = (vBand1<0)?0:vBand1;
	vBand2 = (vBand2<0)?0:vBand2;

		switch(typeIndexFlag)
		{
			case 1: // (Band1 - Band2)/(Band1 + Band2)
					if((vBand1+vBand2)!=0) { index = (vBand1-vBand2)/(vBand1+vBand2); *flState = true; } else index = NODATAVALUE;
					break;
			case 2: // (Band1/Band2)
					if(vBand2!=0) { index = vBand1/vBand2;  *flState = true; } else index = NODATAVALUE;
					break;
			case 3: // (Band1 - Band2)
					index = vBand1-vBand2;  *flState = true; 
					break;
			case 4: // (Band1)/(Band1+Band2)
					if((vBand1+vBand2)!=0) { index = vBand1/(vBand1+vBand2); *flState = true; } else index = NODATAVALUE;
					break;
			case 5: // EVI2 = 2.4*[(Band1 - Band2)/(Band1 + Band2 + 1)]
					index = 2.4f*((vBand1 - vBand2)/(vBand1 + vBand2 + 1.0f)); *flState = true; 
					break;
			case 6: // SAVI = [(Band1 - Band2)/(Band1 + Band2 + 0.5)]*[1 + 0.5]
					index = ((vBand1 - vBand2)/(vBand1 + vBand2 + 0.5f))*(1.0f + 0.5f); *flState = true; 
					break;
			case 7: // SI = [(Band1 - Band2)/(Band1 + Band2 + 0.5)]*[1 + 0.5]
					//index = (vBand1 + vBand2)/2.0f; *flState = true; 
					// SI = SQRT(Band1*Band2)
					if((vBand1*vBand2)>=0) { index = sqrtf(vBand1*vBand2); *flState = true;  } else index = NODATAVALUE;
					break;
			case 8: // BI = SQRT((Band1*Band1) + (Band2*Band2))
					index = sqrtf((vBand1*vBand1)+(vBand2*vBand2)); *flState = true; 
					break;
			case 9: // Chlorophyll Index
					// CI = (Band1/Band2)-1
					if(vBand2!=0) { index = (vBand1/vBand2)-1.0; *flState = true; } else index = NODATAVALUE;
					break;
			case 10: // WDRVI
					if((0.2f*vBand1+vBand2)!=0) { index = (0.2f*vBand1-vBand2)/(0.2f*vBand1+vBand2); *flState = true; } else index = NODATAVALUE;
					break;
			//case 10:// LAI = 0.2273*exp(4.972*NDVI)
				//	if((vBand1+vBand2)!=0) { index = (vBand1-vBand2)/(vBand1+vBand2); index = 0.2273*exp(4.972*index); *flState = true; } else index = NODATAVALUE;
					//break;					
										
			default: index = NODATAVALUE;//-1.0f;
		}	
	return index;
}

// Calculation for THREE channels combination!
float getIndexValue(float vBand1, float vBand2, float vBand3, int typeSubIndexFlag, bool *flState)
{
	float index = NODATAVALUE;
	*flState = false; 
	
	vBand1 = (vBand1<0)?0:vBand1;
	vBand2 = (vBand2<0)?0:vBand2;
	vBand3 = (vBand3<0)?0:vBand3;
	
	float zn = 0;
	
	switch(typeSubIndexFlag)
	{
		case 101: // EVI = 2.5*[(Band1 - Band2)/(1 + Band1 + 6*Band2 - 7.5*Band3)]
				zn = (1.0f+vBand1 + 6.0f*vBand2 - 7.5f*vBand3);
				if(0 == zn) index = NODATAVALUE;
				else { index = 2.5f*((vBand1 - vBand2)/zn); *flState = true; }
				break;
				
		case 102: // GBNDVI = [Band1 - (Band2 + Band3)]/[Band1 + (Band2 + Band3)]
				if((vBand1 + vBand2 + vBand3)!=0) { index = (vBand1 - (vBand2 + vBand3))/(vBand1 + (vBand2 + vBand3)); *flState = true; }
				else index = NODATAVALUE;
				break;
	
		case 103: // VARI = (Band1 - Band2)/(Band1 + Band2 + Band3)
				if((vBand1 + vBand2 + vBand3)!=0) { index = (vBand1 - vBand2)/(vBand1 + vBand2 + vBand3); *flState = true; }
				else index = NODATAVALUE;
				break;
				
		case 104: // LAI = 3.618*EVI-0.118
				zn = (1.0f+vBand1 + 6.0f*vBand2 - 7.5f*vBand3);
				if(0 == zn) index = NODATAVALUE;
				else { index = 3.618f*(2.5f*((vBand1 - vBand2)/zn))-0.118f; *flState = true; }
				break;
		case 105: // NMDI = [Band1 - (Band2 - Band3)]/[Band1 + (Band2 - Band3)]
				if((vBand1 + vBand2 - vBand3)!=0) { index = (vBand1 - vBand2 + vBand3)/(vBand1 + vBand2 - vBand3); *flState = true; }
				else index = NODATAVALUE;
				break;
					
		default: index = NODATAVALUE;
	}
		
	return index;
}

float getDataAsFloat(void* pbuf, int index, GDALDataType dataType)
{
	switch((int)dataType)
	{
		case GDT_Byte: 	  return (float)((GByte*)pbuf)[index];
		case GDT_UInt16:  return (float)((GUInt16*)pbuf)[index];
		case GDT_Int16:   return (float)((GInt16*)pbuf)[index];
		case GDT_UInt32:  return (float)((GUInt32*)pbuf)[index];
		case GDT_Int32:   return (float)((GInt32*)pbuf)[index];
		case GDT_Float32: return ((float*)pbuf)[index];
		case GDT_Float64: return (float)((double*)pbuf)[index];
	}
	return 0;
}

bool isNoData(void* pbuf1, int index1, GDALDataType dataType1, float NoDataBandValue1,
			  void* pbuf2, int index2, GDALDataType dataType2, float NoDataBandValue2,
			  void* pbuf3, int index3, GDALDataType dataType3, float NoDataBandValue3)
{
	float v1 = getDataAsFloat(pbuf1, index1, dataType1);
	float v2 = getDataAsFloat(pbuf2, index2, dataType2);
	float v3 = getDataAsFloat(pbuf3, index3, dataType3);
	if( (v1!=NoDataBandValue1)&&(v2!=NoDataBandValue2)&&(v3!=NoDataBandValue3) ) return true;
	else return false;
}

bool isNoData(void* pbuf1, int index1, GDALDataType dataType1, float NoDataBandValue1,
			  void* pbuf2, int index2, GDALDataType dataType2, float NoDataBandValue2)
{
	float v1 = getDataAsFloat(pbuf1, index1, dataType1);
	float v2 = getDataAsFloat(pbuf2, index2, dataType2);

	if( (v1!=NoDataBandValue1)&&(v2!=NoDataBandValue2) ) return true;
	else return false;
}
