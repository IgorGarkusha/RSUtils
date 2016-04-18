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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "landsat_metadata.h"
#include "../lib/utils.h"

using namespace std;

#define PROG_VERSION "1"
#define DATE_VERSION "17.04.2016"

#ifdef __MSVC__
#define strcasecmp _stricmp
#endif

#ifdef __WINDOWS__
#define	SLASH	'\\'
#else
#define	SLASH	'/'
#endif

#define MODE_UNKNOWN	-1
#define MODE_ALL		1
#define MODE_OLI		2
#define MODE_TIRS		3
#define MODE_45			4
#define MODE_2345		5
#define MODE_234567		6
#define MODE_451011		7

#define MODE_DN_TO_RAD		101
#define MODE_DN_TO_REFL		102
#define MODE_DN_TO_T_BR		103
#define MODE_DN_TO_TC_BR	104

CLandsatMetadata L8Metadata;
bool flCheckReflectance = false;
bool flOnlyRadCalc = true;
bool flOnlyReflCalc = true;

void printHelp();
char * getArchiveName(char * metadataFileName, char * archiveFileName);
char * getDataDir(char * metadataFileName, char * dataDir);
void l8_dn_conversion(FILE* finMetaData, char dataDir[], int mode_processing);
char * l8_band_dn_processing(const char fileName[], int processing_mode, char * outputFileName, bool flNext=false);
void l8_band_dn_to_TOARad(GDALRasterBandH hBandIn, GDALRasterBandH hBandOut,
						  void *pbufIn, float *pbufOut, int rows, int cols, 
						  int bandNumber);
void l8_band_dn_to_TOARefl(GDALRasterBandH hBandIn, GDALRasterBandH hBandOut,
						  void *pbufIn, float *pbufOut, int rows, int cols, 
						  int bandNumber);
void l8_band_dn_to_T_Brightness(GDALRasterBandH hBandIn, GDALRasterBandH hBandOut,
								void *pbufIn, float *pbufOut, int rows, int cols, 
								int bandNumber);
void l8_band_dn_to_TC_Brightness(GDALRasterBandH hBandIn, GDALRasterBandH hBandOut,
								 void *pbufIn, float *pbufOut, int rows, int cols, 
								 int bandNumber);

int main(int argc, char* argv[])
{
	fprintf(stderr, "LANDSAT-8 DN CONVERTOR\nVersion %s.%s. Free software. GNU General Public License, version 3\n", PROG_VERSION, DATE_VERSION);
	fprintf(stderr, "Copyright (C) 2016 Igor Garkusha.\nUkraine, Dnipropetrovsk\n\n");
	
	if(!((argc>=2)&&(argc<=7)))
	{
		fputs("Input parameters not found!\n", stderr);
		printHelp();
		return 1;
	}
		
	bool flDecompressArchive = false;
	int mode_processing = MODE_ALL;
	
	if(argc>2)
	{
		for(int i=1; i<argc-1; i++)
		{
			if(0 == strcmp(argv[i], "-e")) flDecompressArchive = true;
			else
			if(0 == strcmp(argv[i], "-c")) flCheckReflectance = true;
			else
			if(0 == strcmp(argv[i], "--refl")) { flOnlyReflCalc = true; flOnlyRadCalc = false; }
			else
			if(0 == strcmp(argv[i], "--rad"))  { flOnlyRadCalc = true; flOnlyReflCalc = false; }
			else		
			if(0 == strcmp(argv[i], "-b"))
			{
				if(0 == strcmp(argv[i+1], "all")) 		mode_processing = MODE_ALL;
				else
				if(0 == strcmp(argv[i+1], "oli")) 		mode_processing = MODE_OLI;
				else
				if(0 == strcmp(argv[i+1], "tirs")) 		mode_processing = MODE_TIRS;
				else
				if(0 == strcmp(argv[i+1], "45")) 		mode_processing = MODE_45;
				else
				if(0 == strcmp(argv[i+1], "2-5")) 		mode_processing = MODE_2345;
				else
				if(0 == strcmp(argv[i+1], "2-7")) 		mode_processing = MODE_234567;
				else
				if(0 == strcmp(argv[i+1], "451011")) 	mode_processing = MODE_451011;
			}
		}
	}
		
	char metadataFileName[MAX_PATH] = "";
	char dataDir[MAX_PATH] = "";

	strcpy(metadataFileName, argv[argc-1]);
	strcpy(dataDir, getDataDir(metadataFileName, dataDir));

	if(flDecompressArchive) 
	{
		char cmd[2048] = "tar zxvf ";
		
		char archiveFileName[MAX_PATH] = "";
		strcat(cmd, getArchiveName(metadataFileName, archiveFileName));
		strcat(cmd, " -C ");
		strcat(cmd, dataDir);
	
		fprintf(stderr, "DECOMPRESS ARCHIVE %s...\n", archiveFileName);
	
		printf("%s\n", cmd);
		system(cmd);
		
		fprintf(stderr, "\n");
	}

	GDALAllRegister();

	if( L8Metadata.loadMetadataFromFile(metadataFileName) )
	{
		l8_dn_conversion(L8Metadata.getStream(), dataDir, mode_processing);
		L8Metadata.closeMetadataFile();
		fputs("\nProcessing COMPLETE.\n\n", stderr);
	}
	else
	{
		fprintf(stderr, "ERROR open metadata file %s!\n", metadataFileName);
		printHelp();
		return 2;
	}

	return 0;
}

void printHelp()
{
	fputs("Examples:\n", stderr);
	fputs("l8_dn_conversion [options] <Landsat-8_Metadata_File>\n", stderr);
	fputs("l8_dn_conversion -b oli C:\\RSProcessing\\L8\\30\\LC81780262016030LGN00_MTL.txt\n", stderr);
	fputs("l8_dn_conversion -c -b oli C:\\RSProcessing\\L8\\30\\LC81780262016030LGN00_MTL.txt\n", stderr);
	fputs("l8_dn_conversion -e C:\\RSProcessing\\L8\\30\\LC81780262016030LGN00_MTL.txt\n", stderr);
	fputs("l8_dn_conversion -e -b tirs C:\\RSProcessing\\L8\\30\\LC81780262016030LGN00_MTL.txt\n", stderr);
	fputs("\nOptions:\n-e -- decompress archive with extern programs\n", stderr);
	fputs("-b all|oli|tirs|45|2-5|2-7|451011| -- processing bands\n", stderr);
	fputs("-c -- execute check_reflectance with method = 3!\n", stderr);
	fputs("--refl -- only DN to TOA Reflectance\n", stderr);
	fputs("--rad  -- only DN to TOA Radiance\n\n", stderr);
}

char * getArchiveName(char * metadataFileName, char * archiveFileName)
{
	int i = 0, len = strlen(metadataFileName)-1;
	int end_pos = len;
	for(i=len; i>=0; i--) if(metadataFileName[i] == '_') { end_pos = i; break; }
	for(i=0; i<end_pos; i++) archiveFileName[i] = metadataFileName[i];
	archiveFileName[i] = '\0';
	strcat(archiveFileName, ".tar.gz");
	return archiveFileName;
}

char * getDataDir(char * metadataFileName, char * dataDir)
{
	int i = 0, len = strlen(metadataFileName)-1;
	int end_pos = len;
	for(i=len; i>=0; i--) if(metadataFileName[i] == SLASH) { end_pos = i; break; }
	for(i=0; i<=end_pos; i++) dataDir[i] = metadataFileName[i];
	dataDir[i] = '\0';
	return dataDir;
}

void l8_dn_conversion(FILE* finMetaData, char dataDir[], int mode_processing)
{
	if(finMetaData != NULL)
	{
		char **fileNames = NULL;
		int count_bands = 11;
		char outputFileName[MAX_PATH] = "";
		char tmpStr[MAX_PATH] = "";
		string value = "";
				
		fileNames = new char*[count_bands];
		for(int i=0; i<count_bands; i++) fileNames[i] = new char[255];
				
		int mode_processing_mode = mode_processing;
		
		while(mode_processing_mode != MODE_UNKNOWN)
		{
			mode_processing_mode = (mode_processing_mode == MODE_ALL)?MODE_OLI:mode_processing_mode;
			switch(mode_processing_mode)
			{
				case MODE_45:
					
					value = "";
					CLandsatMetadata::getParameterValueFromMetadata(finMetaData, "FILE_NAME_BAND_4", &value);
					CLandsatMetadata::trim_all_symbol(value.c_str(), fileNames[3], '"');
											
					
					strcpy(tmpStr, dataDir);
					strcat(tmpStr, fileNames[3]);

					if(flOnlyRadCalc) l8_band_dn_processing(tmpStr, MODE_DN_TO_RAD, outputFileName);
					if(flOnlyReflCalc) 
					{
						l8_band_dn_processing(tmpStr, MODE_DN_TO_REFL, outputFileName);
						if(flCheckReflectance) 
						{
							char cmd2[2048] = "check_reflectance ";
							strcat(cmd2, outputFileName);
							strcat(cmd2, " 3 ");
							strcat(cmd2, outputFileName);
							strcat(cmd2, ".corr.tif");
							system(cmd2);
						}
					}
					
					value = "";
					CLandsatMetadata::getParameterValueFromMetadata(finMetaData, "FILE_NAME_BAND_5", &value);
					CLandsatMetadata::trim_all_symbol(value.c_str(), fileNames[4], '"');
					strcpy(tmpStr, dataDir);
					strcat(tmpStr, fileNames[4]);
					if(flOnlyRadCalc) l8_band_dn_processing(tmpStr, MODE_DN_TO_RAD, outputFileName);
					if(flOnlyReflCalc) 
					{
						l8_band_dn_processing(tmpStr, MODE_DN_TO_REFL, outputFileName);
						if(flCheckReflectance) 
						{
							char cmd2[2048] = "check_reflectance ";
							strcat(cmd2, outputFileName);
							strcat(cmd2, " 3 ");
							strcat(cmd2, outputFileName);
							strcat(cmd2, ".corr.tif");
							system(cmd2);
						}
					}
					break;
				case MODE_2345:
					for(int i=1; i< 5; i++)
					{
						char param[20] = "";
						sprintf(param, "FILE_NAME_BAND_%d", i+1);
						
						value = "";
						string parameter(param);
						CLandsatMetadata::getParameterValueFromMetadata(finMetaData, parameter, &value);
						
						CLandsatMetadata::trim_all_symbol(value.c_str(), fileNames[i], '"');
												
						strcpy(tmpStr, dataDir);
						strcat(tmpStr, fileNames[i]);

						if(flOnlyRadCalc) l8_band_dn_processing(tmpStr, MODE_DN_TO_RAD, outputFileName);
						if(flOnlyReflCalc) 
						{
							l8_band_dn_processing(tmpStr, MODE_DN_TO_REFL, outputFileName);
							if(flCheckReflectance) 
							{
								char cmd2[2048] = "check_reflectance ";
								strcat(cmd2, outputFileName);
								strcat(cmd2, " 3 ");
								strcat(cmd2, outputFileName);
								strcat(cmd2, ".corr.tif");
								system(cmd2);
							}
						}
					}
					break;
				case MODE_234567:
					for(int i=1; i< 7; i++)
					{
						char param[20] = "";
						sprintf(param, "FILE_NAME_BAND_%d", i+1);

						value = "";
						string parameter(param);
						CLandsatMetadata::getParameterValueFromMetadata(finMetaData, parameter, &value);
						
						CLandsatMetadata::trim_all_symbol(value.c_str(), fileNames[i], '"');
												
						strcpy(tmpStr, dataDir);
						strcat(tmpStr, fileNames[i]);

						if(flOnlyRadCalc) l8_band_dn_processing(tmpStr, MODE_DN_TO_RAD, outputFileName);
						if(flOnlyReflCalc) 
						{
							l8_band_dn_processing(tmpStr, MODE_DN_TO_REFL, outputFileName);
							if(flCheckReflectance) 
							{
								char cmd2[2048] = "check_reflectance ";
								strcat(cmd2, outputFileName);
								strcat(cmd2, " 3 ");
								strcat(cmd2, outputFileName);
								strcat(cmd2, ".corr.tif");
								system(cmd2);
							}
						}
					}
					break;				
				case MODE_451011:
					for(int i=3; i< 5; i++)
					{
						char param[20] = "";
						sprintf(param, "FILE_NAME_BAND_%d", i+1);
						
						value = "";
						string parameter(param);
						CLandsatMetadata::getParameterValueFromMetadata(finMetaData, parameter, &value);
						
						CLandsatMetadata::trim_all_symbol(value.c_str(), fileNames[i], '"');
												
						strcpy(tmpStr, dataDir);
						strcat(tmpStr, fileNames[i]);

						if(flOnlyRadCalc) l8_band_dn_processing(tmpStr, MODE_DN_TO_RAD, outputFileName);
						if(flOnlyReflCalc) 
						{
							l8_band_dn_processing(tmpStr, MODE_DN_TO_REFL, outputFileName);
							if(flCheckReflectance) 
							{
								char cmd2[2048] = "check_reflectance ";
								strcat(cmd2, outputFileName);
								strcat(cmd2, " 3 ");
								strcat(cmd2, outputFileName);
								strcat(cmd2, ".corr.tif");
								system(cmd2);
							}
						}
					}
					
					for(int i=9; i< 11; i++)
					{
						char parameter[20] = "";
						sprintf(parameter, "FILE_NAME_BAND_%d", i+1);
						
						value = "";
						CLandsatMetadata::getParameterValueFromMetadata(finMetaData, parameter, &value);
						CLandsatMetadata::trim_all_symbol(value.c_str(), fileNames[i], '"');
						
						strcpy(tmpStr, dataDir);
						strcat(tmpStr, fileNames[i]);
												
						l8_band_dn_processing(tmpStr, MODE_DN_TO_RAD, outputFileName);
						l8_band_dn_processing(outputFileName, MODE_DN_TO_TC_BR, outputFileName, true);
					}
					
					break;
				case MODE_OLI:	
					for(int i=0; i< 9; i++)
					{
						char param[20] = "";
						sprintf(param, "FILE_NAME_BAND_%d", i+1);
						
						value = "";
						string parameter(param);
						CLandsatMetadata::getParameterValueFromMetadata(finMetaData, parameter, &value);
						
						CLandsatMetadata::trim_all_symbol(value.c_str(), fileNames[i], '"');
												
						strcpy(tmpStr, dataDir);
						strcat(tmpStr, fileNames[i]);

						if(flOnlyRadCalc) l8_band_dn_processing(tmpStr, MODE_DN_TO_RAD, outputFileName);
						if(flOnlyReflCalc) 
						{
							l8_band_dn_processing(tmpStr, MODE_DN_TO_REFL, outputFileName);
							if(flCheckReflectance) 
							{
								char cmd2[2048] = "check_reflectance ";
								strcat(cmd2, outputFileName);
								strcat(cmd2, " 3 ");
								strcat(cmd2, outputFileName);
								strcat(cmd2, ".corr.tif");
								system(cmd2);
							}
						}
					}
					
					break;
				case MODE_TIRS:	
					for(int i=9; i< 11; i++)
					{
						char parameter[20] = "";
						sprintf(parameter, "FILE_NAME_BAND_%d", i+1);
						
						value = "";
						CLandsatMetadata::getParameterValueFromMetadata(finMetaData, parameter, &value);
						CLandsatMetadata::trim_all_symbol(value.c_str(), fileNames[i], '"');
						
						strcpy(tmpStr, dataDir);
						strcat(tmpStr, fileNames[i]);
												
						l8_band_dn_processing(tmpStr, MODE_DN_TO_RAD, outputFileName);
						l8_band_dn_processing(outputFileName, MODE_DN_TO_TC_BR, outputFileName, true);
					}
					break;
			}
			
			if((mode_processing == MODE_ALL)&&(mode_processing_mode == MODE_OLI)) mode_processing_mode = MODE_TIRS;
			else if((mode_processing == MODE_ALL)&&(mode_processing_mode == MODE_TIRS)) mode_processing_mode = MODE_UNKNOWN;
			else mode_processing_mode = MODE_UNKNOWN;
		}
		
		for(int i=0; i<count_bands; i++) delete [] fileNames[i];
		delete [] fileNames;
	}
}

char * l8_band_dn_processing(const char fileName[], int processing_mode, char * outputFileName, bool flNext)
{
    char tmpOutputFileName[MAX_PATH] = "";  
      
    char szBandNumber[3] = "";
    int bandNumber = 0;

	char tmpInputFileName[MAX_PATH] = "";
    
	GDALDatasetH  hDatasetIn = NULL;
	GDALDatasetH  hDatasetOut = NULL;
    GDALRasterBandH hBandIn = NULL;
    GDALRasterBandH hBandOut = NULL;
    
    strcpy(tmpInputFileName, fileName);
    int i = 0, j = 0;
    
    int index = strlen(tmpInputFileName)-1;
    i = 0, j = 0;
    for(i=index; tmpInputFileName[i]!='.'; i--);
    for(j=0; j<i; j++) tmpOutputFileName[j] = tmpInputFileName[j];
    tmpOutputFileName[j] = '\0';
    
    switch(processing_mode)
	{
		case MODE_DN_TO_RAD: 
			strcat(tmpOutputFileName, "_rad.tif");
			break;
		case MODE_DN_TO_REFL:
			strcat(tmpOutputFileName, "_refl.tif");
			break;
		case MODE_DN_TO_T_BR:
			strcat(tmpOutputFileName, "_tK.tif");
			break;
		case MODE_DN_TO_TC_BR:
			strcat(tmpOutputFileName, "_tC.tif");
			break;
		default: strcat(tmpOutputFileName, "_rad.tif");				
	}

	strcpy(outputFileName, tmpOutputFileName);
    
    for(i=index; tmpInputFileName[i]!='_'; i--);
    if(flNext) for(i--; tmpInputFileName[i]!='_'; i--);
    i+=2;
    szBandNumber[0] = tmpInputFileName[i++];
    szBandNumber[1] = tmpInputFileName[i++];
    if( (szBandNumber[1]>='0')&&(szBandNumber[1]<='9') ) szBandNumber[2] = '\0';
    else szBandNumber[1] = '\0';
    
    bandNumber = atoi(szBandNumber);
    
    hDatasetIn = GDALOpen( tmpInputFileName, GA_ReadOnly );

    if(hDatasetIn!=NULL)
    {
		hBandIn =  GDALGetRasterBand(hDatasetIn, 1);
		int cols = GDALGetRasterBandXSize(hBandIn);
		int rows = GDALGetRasterBandYSize(hBandIn);
		
		double adfGeoTransform[6]={0};
		char szProjection[512] = "";
		GDALGetGeoTransform(hDatasetIn, adfGeoTransform );
		
		strcpy(szProjection, GDALGetProjectionRef(hDatasetIn));
		
		CUtils::createNewFloatGeoTIFF(outputFileName, 1, rows, cols, adfGeoTransform, szProjection, 0, 0);
		
		hDatasetOut = GDALOpen( outputFileName, GA_Update );
		if(hDatasetOut!=NULL)
		{
			hBandOut = GDALGetRasterBand(hDatasetOut, 1);
					
			void *pbufIn = NULL;
			pbufIn = CUtils::mallocData(hBandIn, pbufIn, cols);
			
			float *pbufOut = NULL;
			pbufOut = (float *)CPLMalloc(sizeof(float)*cols);
			
			switch(processing_mode)
			{
				case MODE_DN_TO_RAD: 
					l8_band_dn_to_TOARad(hBandIn, hBandOut, pbufIn, pbufOut, rows, cols, bandNumber);
					CUtils::calculateFloatGeoTIFFStatistics(hDatasetOut, bandNumber, false, -9999);
					break;
				case MODE_DN_TO_REFL:
					l8_band_dn_to_TOARefl(hBandIn, hBandOut, pbufIn, pbufOut, rows, cols, bandNumber);
					CUtils::calculateFloatGeoTIFFStatistics(hDatasetOut, bandNumber, false, 0);
					break;
				case MODE_DN_TO_T_BR:
					l8_band_dn_to_T_Brightness(hBandIn, hBandOut, pbufIn, pbufOut, rows, cols, bandNumber);
					CUtils::calculateFloatGeoTIFFStatistics(hDatasetOut, bandNumber, false, 0);
					break;
				case MODE_DN_TO_TC_BR:
					l8_band_dn_to_TC_Brightness(hBandIn, hBandOut, pbufIn, pbufOut, rows, cols, bandNumber);
					CUtils::calculateFloatGeoTIFFStatistics(hDatasetOut, bandNumber, false, -273.15);
					break;					
			}
			
			CPLFree(pbufIn);
			CPLFree(pbufOut);
			
			GDALClose(hDatasetOut);
		}
		else fputs("\nError open input image!!!\n\n", stderr);
		
		GDALClose(hDatasetIn);
	}
	else
	fputs("\nError open input image!!!\n\n", stderr);


	return outputFileName;
}

void l8_band_dn_to_TOARad(GDALRasterBandH hBandIn, GDALRasterBandH hBandOut,
						  void *pbufIn, float *pbufOut, int rows, int cols, 
						  int bandNumber)
{
	if(bandNumber<10) fprintf(stderr, "\nOLI. Band %d. Calculate TOA Radiance..\n", bandNumber);
	else fprintf(stderr, "\nTIRS. Band %d. Calculate TOA Radiance..\n", bandNumber);
	
	int pr = CUtils::progress_ln_ex(stderr, 0, 0, START_PROGRESS);
	for(int i=0; i<rows; i++)
	{					
		CUtils::getRasterLine(hBandIn, i, cols, pbufIn);
		double NoDataValue = CUtils::getDataAsFloat(hBandIn, pbufIn, 1);
		for(int j=0; j<cols; j++)
		{
			double v = CUtils::getDataAsFloat(hBandIn, pbufIn, j);
			if(v!=NoDataValue) pbufOut[j] = L8Metadata.getL8RadianceMultCoeff(bandNumber)*v+L8Metadata.getL8RadianceAddCoeff(bandNumber);
			else pbufOut[j] = -9999;
		}
		CUtils::setRasterLine(hBandOut, i, cols, pbufOut);
		pr = CUtils::progress_ln_ex(stderr, i, rows, pr);
	}
	CUtils::progress_ln_ex(stderr, 0, 0, END_PROGRESS);
}

void l8_band_dn_to_TOARefl(GDALRasterBandH hBandIn, GDALRasterBandH hBandOut,
						  void *pbufIn, float *pbufOut, int rows, int cols, 
						  int bandNumber)
{
	fprintf(stderr, "\nOLI. Band %d. Calculate TOA Reflectance...\n", bandNumber);
	int pr = CUtils::progress_ln_ex(stderr, 0, 0, START_PROGRESS);
	for(int i=0; i<rows; i++)
	{					
		CUtils::getRasterLine(hBandIn, i, cols, pbufIn);
		double NoDataValue = CUtils::getDataAsFloat(hBandIn, pbufIn, 1);
		for(int j=0; j<cols; j++) 
		{
			double v = CUtils::getDataAsFloat(hBandIn, pbufIn, j);
			if(v!=NoDataValue) pbufOut[j] = ( L8Metadata.getL8ReflectanceMultCoeff(bandNumber)*v+L8Metadata.getL8ReflectanceAddCoeff(bandNumber) )/sin(L8Metadata.getL8SunElevationRad());
			else pbufOut[j] = 0;
		}
		CUtils::setRasterLine(hBandOut, i, cols, pbufOut);
		pr = CUtils::progress_ln_ex(stderr, i, rows, pr);
	}
	CUtils::progress_ln_ex(stderr, 0, 0, END_PROGRESS);
}

void l8_band_dn_to_T_Brightness(GDALRasterBandH hBandIn, GDALRasterBandH hBandOut,
								void *pbufIn, float *pbufOut, int rows, int cols, 
								int bandNumber)
{
	fprintf(stderr, "\nTIRS. Band %d. Calculate TOA Brightness Temperature (in Kelvin)...\n", bandNumber);
	int pr = CUtils::progress_ln_ex(stderr, 0, 0, START_PROGRESS);
	for(int i=0; i<rows; i++)
	{					
		CUtils::getRasterLine(hBandIn, i, cols, pbufIn);
		double NoDataValue = CUtils::getDataAsFloat(hBandIn, pbufIn, 1);
		for(int j=0; j<cols; j++) 
		{
			double v = CUtils::getDataAsFloat(hBandIn, pbufIn, j);
			if(v!=NoDataValue) 
			{
				double k1 = L8Metadata.getL8K1Const(bandNumber);
				double k2 = L8Metadata.getL8K2Const(bandNumber);
				pbufOut[j] = k2/log((k1/v)+1.0);
			}
			else pbufOut[j] = 0;
		}
		CUtils::setRasterLine(hBandOut, i, cols, pbufOut);
		pr = CUtils::progress_ln_ex(stderr, i, rows, pr);
	}
	CUtils::progress_ln_ex(stderr, 0, 0, END_PROGRESS);
}

void l8_band_dn_to_TC_Brightness(GDALRasterBandH hBandIn, GDALRasterBandH hBandOut,
								 void *pbufIn, float *pbufOut, int rows, int cols, 
								 int bandNumber)
{
	fprintf(stderr, "\nTIRS. Band %d. Calculate TOA Brightness Temperature (in Celcsium)...\n", bandNumber);
	int pr = CUtils::progress_ln_ex(stderr, 0, 0, START_PROGRESS);
	for(int i=0; i<rows; i++)
	{					
		CUtils::getRasterLine(hBandIn, i, cols, pbufIn);
		double NoDataValue = CUtils::getDataAsFloat(hBandIn, pbufIn, 1);
		for(int j=0; j<cols; j++) 
		{
			double v = (double)CUtils::getDataAsFloat(hBandIn, pbufIn, j);
			if(v!=NoDataValue) 
			{
				double k1 = L8Metadata.getL8K1Const(bandNumber);
				double k2 = L8Metadata.getL8K2Const(bandNumber);
				pbufOut[j] = (k2/log((k1/v)+1.0))-273.15;
			}
			else pbufOut[j] = -273.15;
		}
		CUtils::setRasterLine(hBandOut, i, cols, pbufOut);
		pr = CUtils::progress_ln_ex(stderr, i, rows, pr);
	}
	CUtils::progress_ln_ex(stderr, 0, 0, END_PROGRESS);
}
