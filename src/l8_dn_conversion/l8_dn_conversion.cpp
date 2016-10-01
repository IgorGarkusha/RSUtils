/*
 *
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
#include <string.h>
#include <string>

#ifndef __MSVC__
#include <strings.h>
#endif

#include "landsat_metadata.h"
#include "../lib/utils.h"

using namespace std;

#define PROG_VERSION "2"
#define DATE_VERSION "24.09.2016"

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
bool flOnlyRadCalc = false;
bool flOnlyReflCalc = true;
bool flCreateStack = false;

void printHelp();
//char * getArchiveName(char * metadataFileName, char * archiveFileName);
int getArchiveName(char * metadataFileName, char * archiveFileName);
char * getDataDir(char * metadataFileName, char * dataDir);
char * getDataSubDir(char * metadataFileName, char * dataDir);
void l8_dn_conversion(FILE* finMetaData, char dataDir[], int mode_processing);
char * l8_band_dn_processing(const char * fileName, int processing_mode, char * outputFileName, bool flNext=false);
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
void createStack(const char * stackFileName, const char * outputFileName, int mode);
bool inputFileIsArchive(const char * fileName, char * extention);
bool doCommonDecompress(const char * archFileName, const char * extention, 
						const char * decompressOption, 
						char * dataDir, char * metadataFileName);

int main(int argc, char* argv[])
{
	fprintf(stderr, "LANDSAT-8 DN CONVERTOR\nVersion %s.%s. Free software. GNU General Public License, version 3\n", PROG_VERSION, DATE_VERSION);
	fprintf(stderr, "Copyright (C) 2016 Igor Garkusha.\nUkraine, Dnipro (Dnipropetrovsk)\n\n");
	
	if(!((argc>=2)&&(argc<=8)))
	{
		fputs("Input parameters not found!\n", stderr);
		printHelp();
		return 1;
	}
	
	char dataDir[MAX_PATH] = "";
	char cmd[2048] = "";
	char decompressOption[4] = "";
	
	bool flDecompressArchive = false;
	bool flDecompressToSubdir = false;
	bool flDecompressToExternDir = false;
	
	int mode_processing = MODE_ALL;
	
	if(argc>2)
	{
		for(int i=1; i<argc-1; i++)
		{
			if(0 == strcmp(argv[i], "-e")) { flDecompressArchive = true; strcpy(decompressOption, "-e"); }
			else
			if(0 == strcmp(argv[i], "-ed")) { flDecompressArchive = true; flDecompressToSubdir = true; strcpy(decompressOption, "-ed"); }
			else
			if(0 == strcmp(argv[i], "-x")) { flDecompressArchive = true; flDecompressToExternDir = true; 
											 strcpy(decompressOption, "-x");
											 strcpy(dataDir, argv[i+1]); i++; 
											 sprintf(cmd, "mkdir %s", dataDir);
											 system(cmd);
										   }
			else
			if(0 == strcmp(argv[i], "-c")) flCheckReflectance = true;
			else
			if(0 == strcmp(argv[i], "--refl")) { flOnlyReflCalc = true; flOnlyRadCalc = false; }
			else
			if(0 == strcmp(argv[i], "--stack")) flCreateStack = true;
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
	char extention[10] = "";
	
	if( inputFileIsArchive(argv[argc-1], extention) )
	{
		if(flDecompressArchive == false)
		{
			fprintf(stderr, "ERROR: The program is started without options -e or -ed or -x!\n\n");
			printHelp();
			return 3;
		}

		if( doCommonDecompress(argv[argc-1], extention, decompressOption, dataDir, metadataFileName) == false )
		{
			fprintf(stderr, "ERROR: UNSUPPORTED ARCHIVE FORMAT!\n");
			printHelp();
			return 1;
		}
	}
	else
	{
		strcpy(metadataFileName, argv[argc-1]);
		
		if(flDecompressToExternDir == false)
		{
			if(flDecompressToSubdir == false) strcpy(dataDir, getDataDir(metadataFileName, dataDir));
			else 
			{
				strcpy(dataDir, getDataSubDir(metadataFileName, dataDir));
				sprintf(cmd, "mkdir %s", dataDir);
				system(cmd);
			}
		}

		if(flDecompressArchive) 
		{
			char archiveFileName[MAX_PATH] = "";
			int arch = 0;
			
			if( (arch = getArchiveName(metadataFileName, archiveFileName)) == 0 )
			{
				fprintf(stderr, "ERROR: UNSUPPORTED ARCHIVE FORMAT!\n");
				printHelp();
				return 1;
			}
			
			if(arch == 1) strcpy(cmd, "tar zxvf ");
			else strcpy(cmd, "tar jxvf ");
			
			strcat(cmd, archiveFileName);

			strcat(cmd, " -C ");
			strcat(cmd, dataDir);
		
			fprintf(stderr, "DECOMPRESS ARCHIVE %s...\n", archiveFileName);
		
			printf("%s\n", cmd);
			system(cmd);
			
			fprintf(stderr, "\n");
		}
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
	fputs("l8_dn_conversion -ed C:\\RSProcessing\\L8\\30\\LC81780262016030LGN00_MTL.txt\n", stderr);
	fputs("l8_dn_conversion -x C:\\RSProcessing\\OUTDIR C:\\RSProcessing\\L8\\30\\LC81780262016030LGN00_MTL.txt\n", stderr);
	fputs("l8_dn_conversion -e -b tirs C:\\RSProcessing\\L8\\30\\LC81780262016030LGN00_MTL.txt\n", stderr);
	fputs("l8_dn_conversion -c -b 2-7 --stack /home/user1/processing/LC81780262016030LGN00_MTL.txt\n", stderr);
	
	fputs("\nWindows-style:\n", stderr);
	fputs("l8_dn_conversion -e | -ed | [other options] <Landsat-8_Scene_Archive_File>\n", stderr);
	fputs("l8_dn_conversion -ed -b 45 -c .\\LC81780262016030LGN00.tar.bz2\n", stderr);
	
	fputs("\nUnix-style:\n", stderr);
	fputs("l8_dn_conversion -e | -ed | -x <DIR> | [other options] <Landsat-8_Scene_Archive_File>\n", stderr);
	fputs("l8_dn_conversion -ed -b 45 -c ./LC81780262016030LGN00.tar.bz2\n", stderr);
	fputs("l8_dn_conversion -x /home/user1/processing -c -b 2-7 /home/user1/L8/30/LC81780262016030LGN00.tar.gz\n", stderr);
		
	fputs("\nOptions:\n-e       -- decompress archive (tar.gz, tar.bz, tar.bz2) with extern programs\n", stderr);
	fputs("-ed      -- decompress archive to subdirectory (tar.gz, tar.bz, tar.bz2) with extern programs\n", stderr);
	fputs("-x <DIR> -- decompress archive to directory <DIR> (tar.gz, tar.bz, tar.bz2) with extern programs\n", stderr);
	fputs("-b all|oli|tirs|45|2-5|2-7|451011| -- processing bands\n", stderr);
	fputs("-b all   -- default mode\n", stderr);
	fputs("-c       -- execute check_reflectance with method = 3!\n", stderr);
	fputs("--refl   -- only DN to TOA Reflectance (default mode)\n", stderr);
	fputs("--rad    -- only DN to TOA Radiance\n", stderr);
	fputs("--stack  -- create multiband image (only for -b values: 45, 2-5, 2-7, tirs)\n\n", stderr);
}

int getArchiveName(char * metadataFileName, char * archiveFileName)
{
	int i = 0, len = strlen(metadataFileName)-1;
	int end_pos = len;
	for(i=len; i>=0; i--) if(metadataFileName[i] == '_') { end_pos = i; break; }
	for(i=0; i<end_pos; i++) archiveFileName[i] = metadataFileName[i];
	archiveFileName[i] = '\0';
	
	char tgzFileName[MAX_PATH] = "";
	char tbzFileName[MAX_PATH] = "";
	char tbz2FileName[MAX_PATH] = "";
	
	strcpy(tgzFileName, archiveFileName);
	strcpy(tbzFileName, archiveFileName);
	strcpy(tbz2FileName, archiveFileName);
	strcat(tgzFileName, ".tar.gz");
	strcat(tbzFileName, ".tar.bz");
	strcat(tbz2FileName, ".tar.bz2");
	
	if(CUtils::fileExist(tgzFileName)) { strcat(archiveFileName, ".tar.gz");   return 1; }
	else 
	if(CUtils::fileExist(tbzFileName)) { strcat(archiveFileName, ".tar.bz");   return 2; }
	else 
	if(CUtils::fileExist(tbz2FileName)) { strcat(archiveFileName, ".tar.bz2"); return 3; }
	else 
	return 0;
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

char * getDataSubDir(char * metadataFileName, char * dataDir)
{
	int i = 0, end_pos = 0;
	for(i=strlen(metadataFileName)-1; metadataFileName[i] != '_'; i--);
	end_pos = i-1;
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
			
			if(flCreateStack)
			{
				if((mode_processing_mode != MODE_UNKNOWN)&&
				   (mode_processing_mode != MODE_ALL)&&
				   (mode_processing_mode != MODE_OLI)&&
				   (mode_processing_mode != MODE_451011) )
				{
					value = "";
					string parameter("LANDSAT_SCENE_ID");
					char paramID[30] = "";
					CLandsatMetadata::getParameterValueFromMetadata(finMetaData, parameter, &value);
					CLandsatMetadata::trim_all_symbol(value.c_str(), paramID, '"');
					char stackFileName[MAX_PATH]="";
					strcpy(stackFileName, dataDir);
					strcat(stackFileName, paramID);
					
					switch(mode_processing_mode)
					{
						case MODE_2345:
							strcat(stackFileName,"_2-5.tif");
							break;
						case MODE_234567:
							strcat(stackFileName,"_2-7.tif");
							break;
						case MODE_45:
							strcat(stackFileName,"_4_5.tif");
							break;
						case MODE_TIRS:
							strcat(stackFileName,"_10_11.tif");
					}
					
					char templateFileName[MAX_PATH]="";
					strcpy(templateFileName, dataDir);
					strcat(templateFileName, paramID);
					fprintf(stderr, "\nStacking...\n");
					createStack(stackFileName, templateFileName, mode_processing_mode);
				}
			}
			
			if((mode_processing == MODE_ALL)&&(mode_processing_mode == MODE_OLI)) mode_processing_mode = MODE_TIRS;
			else if((mode_processing == MODE_ALL)&&(mode_processing_mode == MODE_TIRS)) mode_processing_mode = MODE_UNKNOWN;
			else mode_processing_mode = MODE_UNKNOWN;
		}
		
		for(int i=0; i<count_bands; i++) delete [] fileNames[i];
		delete [] fileNames;
	}
}

char * l8_band_dn_processing(const char * fileName, int processing_mode, char * outputFileName, bool flNext)
{
	GDALDatasetH  hDatasetIn = NULL;
	GDALDatasetH  hDatasetOut = NULL;
    GDALRasterBandH hBandIn = NULL;
    GDALRasterBandH hBandOut = NULL;
    
    char tmpOutputFileName[MAX_PATH] = "";  
    char tmpInputFileName[MAX_PATH] = "";
      
    char szBandNumber[3] = "";
    int bandNumber = 0;

    int i = 0, j = 0;
        
    strcpy(tmpInputFileName, fileName);

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
		GDALGetGeoTransform(hDatasetIn, adfGeoTransform );

		////////////////////////////////////////////////////////////////
		// !!! THIS IS BUG !!!
		//char szProjection[512] = "";
		//strcpy(szProjection, GDALGetProjectionRef(hDatasetIn)); 
		////////////////////////////////////////////////////////////////
		
		//const char *szProjection = GDALGetProjectionRef(hDatasetIn);
		
		CUtils::createNewFloatGeoTIFF(outputFileName, 1, rows, cols, adfGeoTransform, GDALGetProjectionRef(hDatasetIn), 0, 0);
		
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

	// fprintf(stderr, "outputFileName: %s\n", outputFileName);
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

void createStack(const char * stackFileName, const char * outputFileName, int mode)
{
	char fullPathName1[MAX_PATH]="";
	strcpy(fullPathName1, outputFileName);
	
	int bands = 0;
	
	// Unsupported stack mode!
	if(mode == MODE_UNKNOWN) 	return;
	if(mode == MODE_ALL) 		return;
	if(mode == MODE_OLI) 		return;
	if(mode == MODE_451011)		return;
	
	switch(mode)
	{
		case MODE_2345:
		case MODE_234567:
		case MODE_45:
			if(flOnlyReflCalc) 
			{ 
				if(flCheckReflectance) strcat(fullPathName1, "_B4_refl.tif.corr.tif"); 
				else strcat(fullPathName1, "_B4_refl.tif"); 
			}
			else
			if(flOnlyRadCalc) strcat(fullPathName1, "_B4_rad.tif");
			break;
		case MODE_TIRS:
			strcat(fullPathName1, "_B10_tC.tif");
	}
	
	switch(mode)
	{
		case MODE_2345:
			bands = 4;
			break;
		case MODE_234567:
			bands = 6;
			break;
		case MODE_45:
			bands = 2;
			break;
		case MODE_TIRS:
			bands = 2;
	}
	
	GDALDatasetH hDataset = GDALOpen( fullPathName1, GA_ReadOnly );

    if(hDataset!=NULL)
    {
		GDALRasterBandH hBand =  GDALGetRasterBand(hDataset, 1);
		int cols = GDALGetRasterBandXSize(hBand);
		int rows = GDALGetRasterBandYSize(hBand);

		double adfGeoTransform[6]={0};
		GDALGetGeoTransform(hDataset, adfGeoTransform );
						
		////////////////////////////////////////////////////////////////
		// !!! THIS IS BUG !!!
		//char szProjection[512] = "";
		//strcpy(szProjection, GDALGetProjectionRef(hDataset)); 
		////////////////////////////////////////////////////////////////
		
		//const char *szProjection = GDALGetProjectionRef(hDataset);
		
		CUtils::createNewFloatGeoTIFF(stackFileName, bands, rows, cols, adfGeoTransform, GDALGetProjectionRef(hDataset), 0, 0);
				
		GDALClose(hDataset);
		hDataset = NULL;
		
		GDALDatasetH hDatasetOut = NULL;
		
		hDatasetOut = GDALOpen( stackFileName, GA_Update );
		if(hDatasetOut!=NULL)
		{
			char ** fullPathName = NULL;
			
			float **pbuf = NULL;
			
			GDALDatasetH * hDatasetIn = NULL;
			
			GDALRasterBandH * hBandIn = NULL;
			GDALRasterBandH * hBandOut = NULL;
			
			hDatasetIn = new GDALDatasetH[bands];
			hBandIn = new GDALRasterBandH[bands];
			hBandOut = new GDALRasterBandH[bands];
			fullPathName = new char*[bands];
			for(int i=0; i<bands; i++) fullPathName[i] = new char[MAX_PATH];
			
			for(int k=0; k<bands; k++)
			{
					switch(mode)
					{
						case MODE_2345:
						case MODE_234567:
							if(flOnlyReflCalc) 
							{ 
								if(flCheckReflectance) sprintf(fullPathName[k], "%s_B%d_refl.tif.corr.tif",outputFileName, 2+k);
								else sprintf(fullPathName[k], "%s_B%d_refl.tif",outputFileName, 2+k);
							}
							else
							if(flOnlyRadCalc) sprintf(fullPathName[k], "%s_B%d_rad.tif",outputFileName, 2+k);
							break;
							
						case MODE_45:
							if(flOnlyReflCalc) 
							{ 
								if(flCheckReflectance) sprintf(fullPathName[k], "%s_B%d_refl.tif.corr.tif",outputFileName, 4+k);
								else sprintf(fullPathName[k], "%s_B%d_refl.tif",outputFileName, 4+k);
							}
							else
							if(flOnlyRadCalc) sprintf(fullPathName[k], "%s_B%d_rad.tif",outputFileName, 4+k);
							break;
							
						case MODE_TIRS:
							sprintf(fullPathName[k], "%s_B%d_tC.tif",outputFileName, 10+k);
					}
											
				hDatasetIn[k] = GDALOpen( fullPathName[k], GA_ReadOnly );
				hBandIn[k] =  GDALGetRasterBand(hDatasetIn[k], 1);
				hBandOut[k] =  GDALGetRasterBand(hDatasetOut, k+1);
			}
		
			pbuf = new float*[bands];
			for(int k=0; k<bands; k++) pbuf[k] = new float[cols];
		
			int pr = CUtils::progress_ln_ex(stderr, 0, 0, START_PROGRESS);
			for(int i=0; i<rows; i++)
			{					
				for(int k=0; k<bands; k++)
				{
				 GDALRasterIO(hBandIn[k], GF_Read, 0, i, cols, 1, pbuf[k], cols, 1, GDT_Float32, 0, 0);						
				 GDALRasterIO(hBandOut[k], GF_Write, 0, i, cols, 1, pbuf[k], cols, 1, GDT_Float32, 0, 0);				
				}
				pr = CUtils::progress_ln_ex(stderr, i, rows, pr);
			}
			CUtils::progress_ln_ex(stderr, 0, 0, END_PROGRESS);
			
			delete [] hBandIn;
			
			for(int k=0; k<bands; k++) GDALClose(hDatasetIn[k]);
			
			delete [] hDatasetIn;
			
			delete [] hBandOut;
			
			for(int i=0; i<bands; i++) delete [] fullPathName[i];
			delete [] fullPathName;
			
			for(int k=0; k<bands; k++) delete [] pbuf[k];
			delete [] pbuf;
					
			CUtils::calculateFloatGeoTIFFStatistics(hDatasetOut, -1, true);
			
			GDALClose(hDatasetOut);
		}
	}
}

bool inputFileIsArchive(const char * fileName, char * extention)
{
	int end_pos = strlen(fileName)-1;
	int i = 0, j = 0, pos = 0, count = 0;
	for(i=end_pos; fileName[i] != SLASH; i--)
	{
		if( '.' == fileName[i] )
		{
			if( 1 == count ) break;
			else count++;
		}
	}
	pos = i;
	for(i=pos, j=0; i<= end_pos; i++, j++) extention[j] = fileName[i];
	extention[j] = '\0';
	
	if( strcasecmp(extention, ".tar.gz") == 0 )  return true;
	else
	if( strcasecmp(extention, ".tar.bz") == 0 )  return true;
	else
	if( strcasecmp(extention, ".tar.bz2") == 0 ) return true;
	
	return false;
}
		
bool doCommonDecompress(const char * archFileName, const char * extention, 
						const char * decompressOption, 
						char * dataDir, char * metadataFileName)
{
	char cmd[2048] = "";
	char commonName[50] = "";
	int archType = 0;
	bool flCreateDir = false;
	
	if( strlen(dataDir)<=1 )
	{
		int pos = 0, i = 0;
		int end_pos = strlen(archFileName)-1;
		for(i=end_pos; archFileName[i] != SLASH; i--);
		
		if( strcmp(decompressOption, "-e") == 0) 
		{ 
			pos = i;
			for(i=0; i<= pos; i++) dataDir[i] = archFileName[i];
			dataDir[i] = '\0';
		}
		else
		if( strcmp(decompressOption, "-ed") == 0 )
		{
			for(; archFileName[i] != '.'; i++);
			pos = i-1;
			for(i=0; i<= pos; i++) dataDir[i] = archFileName[i];
			dataDir[i] = '\0';
			flCreateDir = true;
		}
	}
	
	if( strcmp(decompressOption, "-x") == 0 ) flCreateDir = true;
	
	if(flCreateDir)
	{
		sprintf(cmd, "mkdir %s", dataDir);
		system(cmd);
	}
		
	if( strcasecmp(extention, ".tar.gz") == 0 )  archType = 1;
	else
	if( strcasecmp(extention, ".tar.bz") == 0 )  archType = 2;
	else
	if( strcasecmp(extention, ".tar.bz2") == 0 ) archType = 2;
	else
	{
		return false;
	}
	
	int len = strlen(dataDir)-1;
	if( dataDir[len] != SLASH ) 
	{
		dataDir[len+1] = SLASH;
		dataDir[len+2] = '\0';
	}
		
	if(archType == 1) sprintf(cmd, "tar zxvf %s -C %s", archFileName, dataDir);
	else sprintf(cmd, "tar jxvf %s -C %s", archFileName, dataDir);
	
	fprintf(stderr, "DECOMPRESS ARCHIVE %s...\n", archFileName);
		
	printf("%s\n", cmd);
	system(cmd);
	
	fprintf(stderr, "\n");
		
	int i = 0, j = 0;
	
	for(i=strlen(archFileName)-1; i>=0; i--)
	{
		if(archFileName[i] == SLASH) break;
	}
	
	i++;
	
	for(j=0; archFileName[i] != '.'; i++, j++) commonName[j] = archFileName[i];
	commonName[j] = '\0';
	sprintf(metadataFileName, "%s%s_MTL.txt", dataDir, commonName);
	
	return true;
}

