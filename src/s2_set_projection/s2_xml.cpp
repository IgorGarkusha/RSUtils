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

#ifndef __MSVC__
#include <strings.h>
#endif

#include "s2_xml.h"

#ifdef __MSVC__
#define strcasecmp _stricmp
#endif

double QUANTIFICATION_VALUE = 0;
int epsg_code = 0;
S2BANDGEOMETRICINFO bandsInfo[3];

void InitXML()
{
	LIBXML_TEST_VERSION
	
	for(int i=0; i<3; i++) 
	{
		switch(i)
		{
			case 0: 
				bandsInfo[i].resolution = 10; 
				bandsInfo[i].XDIM = 10;
				bandsInfo[i].YDIM = -10;
				break;
			case 1: 
				bandsInfo[i].resolution = 20; 
				bandsInfo[i].XDIM = 20;
				bandsInfo[i].YDIM = -20;
				break;
			case 2: 
				bandsInfo[i].resolution = 60;
				bandsInfo[i].XDIM = 60;
				bandsInfo[i].YDIM = -60;
		}
		bandsInfo[i].rows = 0;	
		bandsInfo[i].cols = 0;
		bandsInfo[i].ULX = 0;
		bandsInfo[i].ULY = 0;
	}
}

void DestroyXML()
{
	xmlCleanupParser();
	xmlMemoryDump();
}

void StartParseXML(const char *fileName)
{
	xmlTextReaderPtr reader;
    int res = 0;
    reader = xmlReaderForFile(fileName, NULL, 0);
    if(reader != NULL) 
    {
		res = xmlTextReaderRead(reader);
        while(res == 1) 
        {
            readNode(&reader);
            res = xmlTextReaderRead(reader);
        }
        xmlFreeTextReader(reader);
        if(res != 0) fprintf(stderr, "\nError of parsing file %s!\n\n", fileName);
    } 
    else fprintf(stderr, "\nError open file %s!\n\n", fileName);
}

void printS2BandInfo()
{
	fprintf(stderr, "\nInput parameters:\n");
	fprintf(stderr, "QUANTIFICATION VALUE: %lf\n", QUANTIFICATION_VALUE);
	fprintf(stderr, "EPSG CODE: %d\n", epsg_code);
    for(int i=0; i<3; i++) 
	{
        fprintf(stderr, "Resolution Band: %d\tNROWS: %d\tNCOLS: %d\n", 
        bandsInfo[i].resolution, bandsInfo[i].rows, bandsInfo[i].cols);
        fprintf(stderr, "                   \tULX: %lf\tULY: %lf\tXDIM: %d\tYDIM: %d\n\n", 
        bandsInfo[i].ULX, bandsInfo[i].ULY, bandsInfo[i].XDIM, bandsInfo[i].YDIM);
	}
}

void readGeoPosition(xmlTextReaderPtr *reader, int resolution)
{
	const xmlChar *name, *value;
	
	xmlTextReaderRead(*reader);
	xmlTextReaderRead(*reader);
	name = xmlTextReaderConstName(*reader);
		
	if(strcasecmp("ULX", (const char*)name) == 0)
	{
		xmlTextReaderRead(*reader);
		name = xmlTextReaderConstName(*reader);
		if(strcasecmp("#text", (const char*)name) == 0)
		{
			value = xmlTextReaderConstValue(*reader);
			switch(resolution)
			{
				case 10: bandsInfo[0].ULX = atof((const char*)value); break;
				case 20: bandsInfo[1].ULX = atof((const char*)value); break;
				case 60: bandsInfo[2].ULX = atof((const char*)value);
			}
		}
		xmlTextReaderRead(*reader);
	}
	
	xmlTextReaderRead(*reader);
	xmlTextReaderRead(*reader);
	name = xmlTextReaderConstName(*reader);
	if(strcasecmp("ULY", (const char*)name) == 0)
	{
		xmlTextReaderRead(*reader);
		name = xmlTextReaderConstName(*reader);
		if(strcasecmp("#text", (const char*)name) == 0)
		{
			value = xmlTextReaderConstValue(*reader);
			switch(resolution)
			{
				case 10: bandsInfo[0].ULY = atof((const char*)value); break;
				case 20: bandsInfo[1].ULY = atof((const char*)value); break;
				case 60: bandsInfo[2].ULY = atof((const char*)value);
			}
		}
		xmlTextReaderRead(*reader);
	}
	
	xmlTextReaderRead(*reader);
	xmlTextReaderRead(*reader);
	name = xmlTextReaderConstName(*reader);
	if(strcasecmp("XDIM", (const char*)name) == 0)
	{
		xmlTextReaderRead(*reader);
		name = xmlTextReaderConstName(*reader);
		if(strcasecmp("#text", (const char*)name) == 0)
		{
			value = xmlTextReaderConstValue(*reader);
			switch(resolution)
			{
				case 10: bandsInfo[0].XDIM = atof((const char*)value); break;
				case 20: bandsInfo[1].XDIM = atof((const char*)value); break;
				case 60: bandsInfo[2].XDIM = atof((const char*)value);
			}
		}
		xmlTextReaderRead(*reader);
	}
	
	xmlTextReaderRead(*reader);
	xmlTextReaderRead(*reader);
	name = xmlTextReaderConstName(*reader);
	if(strcasecmp("YDIM", (const char*)name) == 0)
	{
		xmlTextReaderRead(*reader);
		name = xmlTextReaderConstName(*reader);
		if(strcasecmp("#text", (const char*)name) == 0)
		{
			value = xmlTextReaderConstValue(*reader);
			switch(resolution)
			{
				case 10: bandsInfo[0].YDIM = atof((const char*)value); break;
				case 20: bandsInfo[1].YDIM = atof((const char*)value); break;
				case 60: bandsInfo[2].YDIM = atof((const char*)value);
			}
		}
		xmlTextReaderRead(*reader);
	}
	
	xmlTextReaderRead(*reader);
	xmlTextReaderRead(*reader); // end Geoposition tag
}
 
void readResolution(xmlTextReaderPtr *reader, int resolution)
{
	const xmlChar *name, *value;
	
	xmlTextReaderRead(*reader);
	xmlTextReaderRead(*reader);
	name = xmlTextReaderConstName(*reader);
	
	if(strcasecmp("NROWS", (const char*)name) == 0)
	{
		xmlTextReaderRead(*reader);
		name = xmlTextReaderConstName(*reader);
		if(strcasecmp("#text", (const char*)name) == 0)
		{
			value = xmlTextReaderConstValue(*reader);
			switch(resolution)
			{
				case 10: bandsInfo[0].rows = atoi((const char*)value); break;
				case 20: bandsInfo[1].rows = atoi((const char*)value); break;
				case 60: bandsInfo[2].rows = atoi((const char*)value);
			}
		}
		xmlTextReaderRead(*reader);
	}
	
	xmlTextReaderRead(*reader);
	xmlTextReaderRead(*reader);
	name = xmlTextReaderConstName(*reader);
	if(strcasecmp("NCOLS", (const char*)name) == 0)
	{
		xmlTextReaderRead(*reader);
		name = xmlTextReaderConstName(*reader);
		if(strcasecmp("#text", (const char*)name) == 0)
		{
			value = xmlTextReaderConstValue(*reader);
			switch(resolution)
			{
				case 10: bandsInfo[0].cols = atoi((const char*)value); break;
				case 20: bandsInfo[1].cols = atoi((const char*)value); break;
				case 60: bandsInfo[2].cols = atoi((const char*)value);
			}
		}
		xmlTextReaderRead(*reader);
	}
	xmlTextReaderRead(*reader);
	xmlTextReaderRead(*reader); // end Size tag
} 
 
void readNode(xmlTextReaderPtr *reader) 
{
    const xmlChar *name, *value;

    name = xmlTextReaderConstName(*reader);
    if(name != NULL)
    {
		if( strcasecmp("HORIZONTAL_CS_CODE", (const char*)name) == 0 )
		{
			xmlTextReaderRead(*reader);
			name = xmlTextReaderConstName(*reader);
			if(strcasecmp("#text", (const char*)name) == 0)
			{
				value = xmlTextReaderConstValue(*reader);
				
				char str_epsg_code[20]="";
				int i = 0;	while(value[i]!=':') i++;
				int j = 0; i++;
				while(value[i]!='\0') { str_epsg_code[j] = value[i]; i++; j++; }
				str_epsg_code[j]='\0';
				epsg_code = atoi(str_epsg_code);
			}
			xmlTextReaderRead(*reader);
		}
		else
		if( strcasecmp("Size", (const char*)name) == 0 )
		{
			xmlChar* attr = xmlTextReaderGetAttribute(*reader, (const xmlChar*)"resolution");
			if(strcasecmp("10", (const char*)attr) == 0) readResolution(reader, 10);
			else
			if(strcasecmp("20", (const char*)attr) == 0) readResolution(reader, 20);
			else
			if(strcasecmp("60", (const char*)attr) == 0) readResolution(reader, 60);
		}
		else
		if( strcasecmp("Geoposition", (const char*)name) == 0 )
		{
			xmlChar* attr = xmlTextReaderGetAttribute(*reader, (const xmlChar*)"resolution");
			if(strcasecmp("10", (const char*)attr) == 0) readGeoPosition(reader, 10);
			else
			if(strcasecmp("20", (const char*)attr) == 0) readGeoPosition(reader, 20);
			else
			if(strcasecmp("60", (const char*)attr) == 0) readGeoPosition(reader, 60);
		}
		else
		if( strcasecmp("QUANTIFICATION_VALUE", (const char*)name) == 0 )
		{
			xmlTextReaderRead(*reader);
			name = xmlTextReaderConstName(*reader);
			if(strcasecmp("#text", (const char*)name) == 0)
			{
				value = xmlTextReaderConstValue(*reader);
				QUANTIFICATION_VALUE = atof((const char*)value);
			}
			xmlTextReaderRead(*reader);
		}
	}
}
