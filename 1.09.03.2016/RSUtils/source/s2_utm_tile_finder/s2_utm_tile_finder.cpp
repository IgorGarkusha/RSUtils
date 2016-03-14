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
#include <string.h>

#define PROG_VERSION "1"
#define DATE_VERSION "09.03.2016"

#define MAX_STR_SIZE 2048

int isPlacemarkTag(char * str);
char* getUtmTileName(FILE* fin, char* str);
void getUtmTileCoords(FILE* fin, float * x1, float * y1, float * x2, float * y2,
					  float * x3, float * y3, float * x4, float * y4);
char* trim_all(char * str);
int isblank_(char ch);
int checkAndGetValueCoord(const char * srcTagName, char dstTagName[], const char * str, int start_pos, float * res);

int main(int argc, char* argv[])
{
	fprintf(stderr, "SENTINEL-2 UTM TILE FINDER (for product L1C S2A MSI)\nVersion %s.%s. Free software. GNU General Public License, version 3\n", PROG_VERSION, DATE_VERSION);
	fprintf(stderr, "Copyright (C) 2016 Igor Garkusha.\nUkraine, Dnipropetrovsk\n\n");
	
	if(argc!=4) 
	{
		fputs("Input parameters not found!\n", stderr);
		fputs("Examples:\n", stderr);
		fputs("s2_utm_tile_finder <UtmKmlTilesFileNamePath> <X_coord_longitude> <Y_coord_latitude>\n", stderr);
		fputs("s2_utm_tile_finder /home/username/s2a/S2A_OPER_GIP_TILPAR_20150622T000000_21000101T000000_ZZ_0001.kml 2.364807 48.843480\n", stderr);
		fputs("s2_utm_tile_finder /Users/username/s2a/S2A_OPER_GIP_TILPAR_20150622T000000_21000101T000000_ZZ_0001.kml 2.364807 48.843480\n", stderr);
		fputs("s2_utm_tile_finder C:\\s2a\\S2A_OPER_GIP_TILPAR_20150622T000000_21000101T000000_ZZ_0001.kml 2.364807 48.843480\n\n", stderr);
				
		fputs("Sentinel-2 Level-1C product tiling grid released (17 July 2015)\n", stderr);
		fputs("The tiling grid for Sentinel-2 Level-1C products is now available for download:\n", stderr);
		fputs("https://sentinel.esa.int/documents/247904/1955685/S2A_OPER_GIP_TILPAR_20150622T000000_21000101T000000_ZZ_0001\n\n", stderr);
		fputs("Sentinel-2 products are a compilation of elementary granules of fixed size,\n", stderr);
		fputs("along with a single orbit.\n", stderr);
		fputs("A granule is the minimum indivisible partition of a product (containing all possible spectral bands).\n", stderr);
		fputs("For Level-1C and Level-2A, the granules, also called tiles, are 100 km2 ortho-images in UTM/WGS84 projection.\n", stderr);
		fputs("More information on Sentinel-2 products can be found on the Data Products page:\n", stderr);
		fputs("https://sentinel.esa.int/web/sentinel/missions/sentinel-2/data-products\n\n", stderr);	
		
		return 1;
	}
	
	FILE* fin = NULL;
	char * UTM_TILE_NAME = (char*)malloc(7);
	strcpy(UTM_TILE_NAME, "");
	char str[2048]="";
	float x = atof(argv[2]);
	float y = atof(argv[3]);
	int flFind = 0;
	if((fin=fopen(argv[1],"rt"))!=NULL)
	{
		while(!feof(fin))
		{
			strcpy(str, "");
			if( fgets(str, MAX_STR_SIZE, fin) != NULL)
			{
				if( isPlacemarkTag(str) )
				{
					strcpy(UTM_TILE_NAME, "");
					UTM_TILE_NAME = getUtmTileName(fin, UTM_TILE_NAME);
					
					float x1 = 0, y1 = 0;
					float x2 = 0, y2 = 0;
					float x3 = 0, y3 = 0;
					float x4 = 0, y4 = 0;
					getUtmTileCoords(fin, &x1, &y1, &x2, &y2, &x3, &y3, &x4, &y4);
										
					if( (x1 < x) && (x4 < x) && (x2 > x) && (x3 > x) &&
					    (y1 > y) && (y2 > y) && (y3 < y) && (y4 < y) )
						{
							printf("\nUTM Tile: %s\n\n", UTM_TILE_NAME);
							flFind = 1;
							break;
						}
				}
			}
		}
		fclose(fin);
	}
	
	free(UTM_TILE_NAME);
	
	if(!flFind) printf("\nUTM Tile not found!\n\n");
	
	return 0;
}

int isblank_(char ch)
{
	return (ch == '\x20')?1:(ch == '\t')?1:0;
}

char* trim_all(char * str)
{
	if(str[strlen(str)-1] == '\n') str[strlen(str)-1] = '\0';
	int start_pos = 0;
	int end_pos = 0;
	for(int i=0; isblank_(str[i])!=0; i++) start_pos = i;
	for(int i=strlen(str)-1; isblank_(str[i]) != 0; i--) end_pos = i;
	str[end_pos] = '\0';
	int j = 0;
	for(int i=start_pos+1; str[i]!='\0'; i++, j++) str[j] = str[i];
	str[j] = '\0';
	return str;
}

int isPlacemarkTag(char * str)
{
	str = trim_all(str);
	if(strcmp("<Placemark>", str) == 0) return 1;
	else return 0;
}

char* getUtmTileName(FILE* fin, char* str)
{
	char * tmp_str = (char*)malloc(MAX_STR_SIZE);
	strcpy(tmp_str,"");
	char name_tag[5]="";
	if( fgets(tmp_str, MAX_STR_SIZE, fin) != NULL)
	{
		tmp_str = trim_all(tmp_str);
		for(int i=1; i<5; i++) name_tag[i-1] = tmp_str[i];
		name_tag[4]='\0';
		if( strcmp("name", name_tag)==0 )
		{
			int j=0;
			for(int i=6; tmp_str[i]!='<'; i++, j++) str[j] = tmp_str[i];
			str[j]='\0';
		}
	}
	free(tmp_str);
	return str;
}

int checkAndGetValueCoord(const char * srcTagName, char dstTagName[], const char * str, int start_pos, float * res)
{
	char szvalue[20]="";
	if(strcmp(srcTagName, dstTagName) == 0) 
	{
		int j = 0;
		for(int i=start_pos; str[i]!='<'; i++, j++) szvalue[j] = str[i];
		szvalue[j] = '\0';
		*res = atof(szvalue);
		return 1;
	}
	return 0;
}

void getUtmTileCoords(FILE* fin, float * x1, float * y1, float * x2, float * y2,
					  float * x3, float * y3, float * x4, float * y4)
{
	char * tmp_str = (char*)malloc(MAX_STR_SIZE);
	
	int count_find = 0;
	while(count_find < 8)
	{
		strcpy(tmp_str,"");
		if( fgets(tmp_str, MAX_STR_SIZE, fin) != NULL)
		{
			tmp_str = trim_all(tmp_str);
			if(strlen(tmp_str)>32)
			{
				char tag_name[MAX_STR_SIZE] = "";
				int i = 0;
				int start_pos = 0;
				for(;tmp_str[i]!='>'; i++) tag_name[i] = tmp_str[i];
				tag_name[i]='>';
				start_pos = i+1;
				tag_name[start_pos]='\0';
				if( checkAndGetValueCoord("<SimpleData name=\"ll_lat\">", tag_name, (const char *)tmp_str, start_pos, y1) ) count_find++;
				if( checkAndGetValueCoord("<SimpleData name=\"ll_long\">", tag_name, (const char *)tmp_str, start_pos, x1) ) count_find++;
				if( checkAndGetValueCoord("<SimpleData name=\"lr_lat\">", tag_name, (const char *)tmp_str, start_pos, y2) ) count_find++;
				if( checkAndGetValueCoord("<SimpleData name=\"lr_long\">", tag_name, (const char *)tmp_str, start_pos, x2) ) count_find++;
				if( checkAndGetValueCoord("<SimpleData name=\"tr_lat\">", tag_name, (const char *)tmp_str, start_pos, y3) ) count_find++;
				if( checkAndGetValueCoord("<SimpleData name=\"tr_long\">", tag_name, (const char *)tmp_str, start_pos, x3) ) count_find++;
				if( checkAndGetValueCoord("<SimpleData name=\"tl_lat\">", tag_name, (const char *)tmp_str, start_pos, y4) ) count_find++;
				if( checkAndGetValueCoord("<SimpleData name=\"tl_long\">", tag_name, (const char *)tmp_str, start_pos, x4) ) count_find++;
			}
		}
	}
	
	free(tmp_str);
}
