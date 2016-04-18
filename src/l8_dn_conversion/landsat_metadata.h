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

#ifndef __LANDSAT_METADATA_H__
#define __LANDSAT_METADATA_H__

#include <stdio.h>
#include <vector>
#include <string>
using namespace std;

#define MAX_STR_LENGTH 2048

#define L8_BAND_COUNT	11

class CLandsatMetadata
{
	private:
		FILE* m_fin;
		double m_L8RadianceMultCoeff[L8_BAND_COUNT];
		double m_L8RadianceAddCoeff[L8_BAND_COUNT];
		double m_L8ReflectanceMultCoeff[L8_BAND_COUNT];
		double m_L8ReflectanceAddCoeff[L8_BAND_COUNT];
		double m_L8K1Const[2];
		double m_L8K2Const[2];
		double m_SUN_ELEVATION;
		void initVariables();
		void clearMetadata();
	public:
		CLandsatMetadata();
		~CLandsatMetadata();
		bool loadMetadataFromFile(const char * fileName);
		FILE* getStream();
		void closeMetadataFile();
		static bool isSpace(char ch);
		static char * trim_all(const char * inputString, char * outputString);
		static char * trim_all_symbol(const char * inputString, char * outputString, char symbol);
		static void split_string(const char * inputString, char separator1, char separator2, vector<string>* str_parts);
		static void getParameterValueFromMetadata(FILE* finMetaData, string parameter, string* value);
		double getL8RadianceMultCoeff(int bandNumber);
		double getL8RadianceAddCoeff(int bandNumber);
		double getL8ReflectanceMultCoeff(int bandNumber);
		double getL8ReflectanceAddCoeff(int bandNumber);
		double getL8K1Const(int bandNumber);
		double getL8K2Const(int bandNumber);
		double getL8SunElevationRad();
		double getL8SunElevationGrad();
};

#endif
