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

#ifndef __MSVC__
#include <strings.h>
#endif

#include "../lib/utils.h"

using namespace std;

#define PROG_VERSION "1"
#define DATE_VERSION "24.09.2016"

#ifdef __MSVC__
#define strcasecmp _stricmp
#endif

#ifdef __WINDOWS__
#define	SLASH	'\\'
#else
#define	SLASH	'/'
#endif

void printHelp();
bool checkTXTExtention(const char * fileName);
bool isSpace(char ch);
char * trim_all(const char * inputString, char * outputString);
int execute_l8_dn_conversion(const char * fileName, const char * options);

int main(int argc, char* argv[])
{
	fprintf(stderr, "LANDSAT-8 DN BATCH CONVERTOR\nVersion %s.%s. Free software. GNU General Public License, version 3\n", PROG_VERSION, DATE_VERSION);
	fprintf(stderr, "Copyright (C) 2016 Igor Garkusha.\nUkraine, Dnipro (Dnipropetrovsk)\n\n");
	
	if(!((argc>=2)&&(argc<=3)))
	{
		fputs("Input parameters not found!\n", stderr);
		printHelp();
		return 1;
	}
	
	char scenesFileName[MAX_PATH] = "";
	char options[50] = "";
	
	if(2 == argc) strcpy(scenesFileName, argv[1]);
	else 
	{
		strcpy(options, argv[1]);
		strcpy(scenesFileName, argv[ argc-1 ]);
	}
	
	if(checkTXTExtention(scenesFileName))
	{
		FILE * fin = NULL;
		
		char fnString[MAX_PATH] = "";
		char fnModString[MAX_PATH] = "";
		
		if(( fin=fopen(scenesFileName, "rt") ) != NULL)
		{
			while( ! feof(fin) )
			{
				strcpy(fnString, "");
				if( fgets(fnString, MAX_PATH, fin) != NULL )
				{
					if( strlen(fnString)>1 )
					{
						trim_all(fnString, fnModString);			
						if( execute_l8_dn_conversion(fnModString, options) == 1 ) break;
					}
				}else break;
			}
			
			fclose(fin);
		}
	}
	else
	{
		printHelp();
		return 1;
	}
	
	return 0;
}

void printHelp()
{
	fputs("Examples:\n", stderr);
	fputs("l8_dn_batch_convert [\"l8_dn_conversion options\"] <Landsat-8_File_Scenes_List.txt>\n", stderr);
	fputs("l8_dn_batch_convert C:\\RSProcessing\\L8_DATA\\scenes.txt\n", stderr);
	fputs("l8_dn_batch_convert \"-ed -c -b oli\" C:\\RSProcessing\\L8_DATA\\scenes.txt\n", stderr);
	fputs("l8_dn_batch_convert \"-ed -b 2-7 --stack\" /home/user1/processing/scenes.txt\n", stderr);
	fputs("l8_dn_batch_convert \"-x /home/user1/Data -b tirs --stack\" /home/user1/processing/scenes.txt\n", stderr);
	
	fputs("\nExamples scenes file. Windows-style:\n", stderr);
	fputs(".\\LC81780262016030LGN00.tar.gz\n", stderr);
	fputs(".\\LC81760282013199LGN00.tar.gz\n", stderr);
	fputs("....\n", stderr);
	
	fputs("\nExamples scenes file. Unix-style:\n", stderr);
	fputs("./LC81780262016030LGN00.tar.gz\n", stderr);
	fputs("./LC81760282013199LGN00.tar.gz\n", stderr);
	fputs("....\n", stderr);
	fputs("\tOR\n", stderr);
	fputs("/home/user1/data/LC81780262016030LGN00.tar.gz\n", stderr);
	fputs("/home/user1/data/LC81760282013199LGN00.tar.gz\n", stderr);
	fputs("....\n\n", stderr);
}

bool isSpace(char ch)
{
	return ( (ch == '\x20') || (ch == '\t') || (ch == '\n') || (ch == '\x0D') || (ch == '\x0A') );
}

char * trim_all(const char * inputString, char * outputString)
{
	int start_pos = 0;
	int end_pos = 0;
	for(int i=0; inputString[i]!='\0'; i++) if(!isSpace(inputString[i])) { start_pos = i; break; }	
	for(int i=strlen(inputString)-1; i>=0; i--) if(!isSpace(inputString[i])) { end_pos = i; break; }
	int j = 0;
	for(int i=start_pos; i<=end_pos; i++, j++) outputString[j] = inputString[i];
	outputString[j] = '\0';
	return outputString;
}

int execute_l8_dn_conversion(const char * fileName, const char * options)
{
	char cmd[2048] = "";
	sprintf(cmd, "l8_dn_conversion %s %s", options, fileName);
	printf("%s\n", cmd);
	system(cmd);
    return 0;
}

bool checkTXTExtention(const char * fileName)
{
	char ext[4] ="";
	int len = strlen(fileName);
	for(int i = len-1, j = 2; j>=0; i--, j--) ext[j] = fileName[i];
	ext[3] = '\0';
	if( strcasecmp(ext, "txt") == 0 )
	{
		FILE * fin = NULL;
		if(( fin=fopen(fileName, "rt") )!=NULL)
		{
			fclose(fin);
			return true;
		}else return false;
	}
	else return false;
}
