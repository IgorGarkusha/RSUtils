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

#ifndef __S2_XML_H__
#define __S2_XML_H__

#include <libxml/xmlreader.h>

typedef struct _tagBandGeometricInfo
{
	int resolution;
	int rows;
	int cols;
	double ULX;
	double ULY;
	int XDIM;
	int YDIM;
}S2BANDGEOMETRICINFO;

extern double QUANTIFICATION_VALUE;
extern int epsg_code;
extern S2BANDGEOMETRICINFO bandsInfo[];

void InitXML();
void DestroyXML();
void StartParseXML(const char *fileName);
void printS2BandInfo();
void readGeoPosition(xmlTextReaderPtr *reader, int resolution);
void readResolution(xmlTextReaderPtr *reader, int resolution);
void readNode(xmlTextReaderPtr *reader);


#endif
