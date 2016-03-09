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

package org.rsutils.downloader;

import javax.swing.*;

public interface DownLoadMainFrameInterface
{
	public String getUI_WorkDir();
	public String getUI_ProductID();
	public String getUI_ProductName();
	public String getUI_UTM_TILE();
	public String getUI_UserNameAndPassword();
	public String getSUFFIX();
	public String getProductXML();
	public String getGranuleName(String manifestSafePathName);
	public String getGranuleXMLFileName(String granuleName);
	public String getBandName(String granuleName, int index);
	public int[] getDownloadBandsIndexes();
	public String getUI_Polarization();
	public JButton getUI_btnDownload();
	public String getUI_Year();
	public String getUI_FromMonth();
	public String getUI_ToMonth();
}
