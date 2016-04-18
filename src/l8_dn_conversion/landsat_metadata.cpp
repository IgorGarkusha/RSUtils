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

#include "landsat_metadata.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <string>
using namespace std;

CLandsatMetadata::CLandsatMetadata()
{
	m_fin = NULL;
	clearMetadata();
}

CLandsatMetadata::~CLandsatMetadata()
{
	closeMetadataFile();
}

void CLandsatMetadata::closeMetadataFile()
{
	if(m_fin!=NULL) { fclose(m_fin); m_fin = NULL; }
	clearMetadata();
}

void CLandsatMetadata::clearMetadata()
{
	for(int i = 0; i<L8_BAND_COUNT; i++)
	{
		m_L8RadianceMultCoeff[i] = 0;
		m_L8RadianceAddCoeff[i] = 0;
		m_L8ReflectanceMultCoeff[i] = 0;
		m_L8ReflectanceAddCoeff[i] = 0;
	}
	m_L8K1Const[0] = m_L8K1Const[1] = 0;
	m_L8K2Const[0] = m_L8K2Const[1] = 0;
	m_SUN_ELEVATION = 0;
}

bool CLandsatMetadata::loadMetadataFromFile(const char * fileName)
{
	if((m_fin=fopen(fileName, "rt"))!=NULL) { initVariables(); return true; }
	else return false;
}

FILE* CLandsatMetadata::getStream()
{
	return m_fin;
}

bool CLandsatMetadata::isSpace(char ch)
{
	return ( (ch == '\x20') || (ch == '\t') || (ch == '\n') || (ch == '\x0D') || (ch == '\x0A') );
}

char * CLandsatMetadata::trim_all(const char * inputString, char * outputString)
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

char * CLandsatMetadata::trim_all_symbol(const char * inputString, char * outputString, char symbol)
{
	int start_pos = 0;
	int end_pos = 0;
	for(int i=0; inputString[i]!='\0'; i++) if(inputString[i] != symbol) { start_pos = i; break; }	
	for(int i=strlen(inputString)-1; i>=0; i--) if(inputString[i] != symbol ) { end_pos = i; break; }
	int j = 0;
	for(int i=start_pos; i<=end_pos; i++, j++) outputString[j] = inputString[i];
	outputString[j] = '\0';
	return outputString;
}

void CLandsatMetadata::split_string(const char * inputString, char separator1, char separator2, vector<string>* str_parts)
{
	string str = "";
	for(int i=0; inputString[i]!='\0'; i++)
	{
		if((inputString[i]!=separator1)&&(inputString[i]!=separator2)) str+=inputString[i];
		else { if(str.length()>0) { str_parts->push_back(str); str = ""; } }
	}
	if(str.length()>0) { str_parts->push_back(str); str = ""; }
}

void CLandsatMetadata::getParameterValueFromMetadata(FILE* finMetaData, string parameter, string* value)
{
	rewind(finMetaData);
	
	char str[MAX_STR_LENGTH] = "";
	char trim_str[MAX_STR_LENGTH] = "";
	vector<string> str_parts;
	
	while(!feof(finMetaData))
	{
		strcpy(str,"");
		strcpy(trim_str,"");
		str_parts.clear();
		
		fgets(str, MAX_STR_LENGTH, finMetaData);
		
		if(strlen(str)>1)
		{
			trim_all(str, trim_str);
			split_string(trim_str, '\x20', '=', &str_parts);
			string param = str_parts.at(0);
			if(param.compare(parameter) == 0)
			{
				*value = str_parts.at(1);
				break;
			}
		}
	}
	str_parts.clear();
}

void CLandsatMetadata::initVariables()
{
	string value = "";
	char str[255] = "";
	
	for(int i = 0; i<L8_BAND_COUNT; i++)
	{
		sprintf(str, "RADIANCE_MULT_BAND_%d", i+1);
		getParameterValueFromMetadata(m_fin, str, &value);
		m_L8RadianceMultCoeff[i] = atof(value.c_str());
	
		sprintf(str, "RADIANCE_ADD_BAND_%d", i+1);
		getParameterValueFromMetadata(m_fin, str, &value);
		m_L8RadianceAddCoeff[i]  = atof(value.c_str());
		
		if(i<9)
		{
			sprintf(str, "REFLECTANCE_MULT_BAND_%d", i+1);
			getParameterValueFromMetadata(m_fin, str, &value);
			m_L8ReflectanceMultCoeff[i] = atof(value.c_str());
		
			sprintf(str, "REFLECTANCE_ADD_BAND_%d", i+1);
			getParameterValueFromMetadata(m_fin, str, &value);
			m_L8ReflectanceAddCoeff[i]  = atof(value.c_str());
		}
	}
	
	getParameterValueFromMetadata(m_fin, "K1_CONSTANT_BAND_10", &value);
	m_L8K1Const[0] = atof(value.c_str());
	
	getParameterValueFromMetadata(m_fin, "K1_CONSTANT_BAND_11", &value);
	m_L8K1Const[1] = atof(value.c_str());
	
	getParameterValueFromMetadata(m_fin, "K2_CONSTANT_BAND_10", &value);
	m_L8K2Const[0] = atof(value.c_str());
	
	getParameterValueFromMetadata(m_fin, "K2_CONSTANT_BAND_11", &value);
	m_L8K2Const[1] = atof(value.c_str());
	
	getParameterValueFromMetadata(m_fin, "SUN_ELEVATION", &value);
	m_SUN_ELEVATION = atof(value.c_str());
}

double CLandsatMetadata::getL8RadianceMultCoeff(int bandNumber)
{
	return ((bandNumber>=1)&&(bandNumber<=L8_BAND_COUNT))?m_L8RadianceMultCoeff[bandNumber-1]:0;
}

double CLandsatMetadata::getL8RadianceAddCoeff(int bandNumber)
{
	return ((bandNumber>=1)&&(bandNumber<=L8_BAND_COUNT))?m_L8RadianceAddCoeff[bandNumber-1]:0;
}

double CLandsatMetadata::getL8ReflectanceMultCoeff(int bandNumber)
{
	return ((bandNumber>=1)&&(bandNumber<=L8_BAND_COUNT))?m_L8ReflectanceMultCoeff[bandNumber-1]:0;
}

double CLandsatMetadata::getL8ReflectanceAddCoeff(int bandNumber)
{
	return ((bandNumber>=1)&&(bandNumber<=L8_BAND_COUNT))?m_L8ReflectanceAddCoeff[bandNumber-1]:0;
}

double CLandsatMetadata::getL8K1Const(int bandNumber)
{
	return (((bandNumber-10)>=0)&&((bandNumber-10)<=1))?m_L8K1Const[bandNumber-10]:0;
}

double CLandsatMetadata::getL8K2Const(int bandNumber)
{
	return (((bandNumber-10)>=0)&&((bandNumber-10)<=1))?m_L8K2Const[bandNumber-10]:0;
}

double CLandsatMetadata::getL8SunElevationRad()
{
	return (m_SUN_ELEVATION * M_PI)/180.0;
}

double CLandsatMetadata::getL8SunElevationGrad()
{
	return m_SUN_ELEVATION;
}
