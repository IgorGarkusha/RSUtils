/*
 * Project: Remote Sensing Utilities (Extentions GDAL/OGR)
 * Author:  Igor Garkusha <rsutils.gis@gmail.com>
 *          Ukraine, Dnipropetrovsk
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

package org.rsutils.downloader;

import javax.swing.*;

public class DownLoadMainFrameAdapter implements DownLoadMainFrameInterface
{
	public String getUI_WorkDir() { return ""; }
	public String getUI_ProductID() { return ""; }
	public String getUI_ProductName() { return ""; }
	public String getUI_UTM_TILE() { return ""; }
	public String getUI_UserNameAndPassword() { return ""; }
	public String getSUFFIX() { return ""; }
	public String getProductXML() { return ""; }
	public String getGranuleName(String manifestSafePathName) { return ""; }
	public String getGranuleXMLFileName(String granuleName) { return ""; }
	public String getBandName(String granuleName, int index) { return ""; }
	public int[] getDownloadBandsIndexes() { return null; }
	public String getUI_Polarization() { return ""; }
	public JButton getUI_btnDownload() { return null; }
	public String getUI_Year() { return ""; }
	public String getUI_FromMonth() { return ""; }
	public String getUI_ToMonth() { return ""; }
	public void loadFromManifestData(String manifestSafePathName) {}
}
