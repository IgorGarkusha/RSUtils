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

package s2a_tile_downloader;

import org.rsutils.downloader.*;
import java.net.*;
import javax.swing.*;
import javax.swing.text.*;
import org.rsutils.*;

public class S2ADownloadProcessRunner extends ProcessRunner implements Runnable
{
	public S2ADownloadProcessRunner(DownLoadMainFrameInterface mainFrameDownloadProgram)
	{
		super(mainFrameDownloadProgram, null, null);
	}
	
	public S2ADownloadProcessRunner(DownLoadMainFrameInterface mainFrameDownloadProgram, JProgressBar prBar, JTextArea txtInfo)
	{
		super(mainFrameDownloadProgram, prBar, txtInfo);
	}
		
	@Override
    public void run()
	{		
		if(getMainFrame().getUI_btnDownload() != null) getMainFrame().getUI_btnDownload().setEnabled(false);
		
		String DIR_NAME = "";
        String textInfo = "";

        try {
				textInfo = "Processing for product "+getMainFrame().getUI_ProductName()+"\n" + "Download manifest.safe...\n";
				
				DIR_NAME = getMainFrame().getUI_WorkDir();
				DIR_NAME += "/granule_"+ getMainFrame().getUI_UTM_TILE()+"_"+getMainFrame().getUI_ProductName(); // Only POI
			  
				Utils.makeDir(DIR_NAME);
				
				FileDownload.doDownload(new URL(
						"https://scihub.copernicus.eu/dhus/odata/v1/Products('"+
						getMainFrame().getUI_ProductID()+"')/Nodes('"+getMainFrame().getUI_ProductName()+
						"')/Nodes('manifest.safe')/$value"),
						DIR_NAME+"/manifest.safe", getMainFrame().getUI_UserNameAndPassword(), 
						getProgressBar(), textInfo, getTextInfoArea(), false);
				
				String productXMLFileName = getMainFrame().getProductXML();
				textInfo = "Download "+productXMLFileName+"...\n";
				
				FileDownload.doDownload(new URL(
						"https://scihub.copernicus.eu/dhus/odata/v1/Products('"+
						getMainFrame().getUI_ProductID()+"')/Nodes('"+getMainFrame().getUI_ProductName()+
						"')/Nodes('"+productXMLFileName+"')/$value"), 
						DIR_NAME+"/"+productXMLFileName, getMainFrame().getUI_UserNameAndPassword(),
						getProgressBar(), textInfo, getTextInfoArea(), false);
				
				Utils.makeDir(DIR_NAME+"/GRANULE");
							
				String granuleName = getMainFrame().getGranuleName(DIR_NAME+"/manifest.safe");
				String granuleXMLFileName = getMainFrame().getGranuleXMLFileName(granuleName);
				
				textInfo = "Download "+granuleXMLFileName+"...\n";
							
				String granuleDir = granuleName + "_T" + getMainFrame().getUI_UTM_TILE() + "_" +getMainFrame().getSUFFIX();
										
				Utils.makeDir(DIR_NAME+"/GRANULE/"+granuleDir);
				
				FileDownload.doDownload(new URL(
						"https://scihub.copernicus.eu/dhus/odata/v1/Products('"+
						getMainFrame().getUI_ProductID()+"')/Nodes('"+getMainFrame().getUI_ProductName()+
						"')/Nodes('GRANULE')/Nodes('"+granuleDir+"')/Nodes('"+granuleXMLFileName+"')/$value"), 
						  DIR_NAME+"/GRANULE/"+granuleDir+"/"+granuleXMLFileName, getMainFrame().getUI_UserNameAndPassword(), 
						getProgressBar(), textInfo, getTextInfoArea(), false);
				
				Utils.makeDir(DIR_NAME+"/GRANULE/"+granuleDir+"/IMG_DATA");
				
				int [] band_indexes = getMainFrame().getDownloadBandsIndexes();
				if(null == band_indexes)
				{
					for(int i=1; i<=13; i++)
					{
						String bandFileName = getMainFrame().getBandName(granuleName, i);                
						textInfo = "Download "+bandFileName+"...\n";
						
						FileDownload.doDownload(new URL(
							"https://scihub.copernicus.eu/dhus/odata/v1/Products('"+
							getMainFrame().getUI_ProductID()+"')/Nodes('"+getMainFrame().getUI_ProductName()+
							"')/Nodes('GRANULE')/Nodes('"+granuleDir+"')/Nodes('IMG_DATA')/Nodes('"+bandFileName+"')/$value"), 
							DIR_NAME+"/GRANULE/"+granuleDir+"/IMG_DATA/"+bandFileName, getMainFrame().getUI_UserNameAndPassword(), 
							getProgressBar(), textInfo, getTextInfoArea(), true);
					}
				}
				else
				{
					for(int i : band_indexes)
					{
						String bandFileName = getMainFrame().getBandName(granuleName, i);                
						textInfo = "Download "+bandFileName+"...\n";
						
						FileDownload.doDownload(new URL(
							"https://scihub.copernicus.eu/dhus/odata/v1/Products('"+
							getMainFrame().getUI_ProductID()+"')/Nodes('"+getMainFrame().getUI_ProductName()+
							"')/Nodes('GRANULE')/Nodes('"+granuleDir+"')/Nodes('IMG_DATA')/Nodes('"+bandFileName+"')/$value"), 
							DIR_NAME+"/GRANULE/"+granuleDir+"/IMG_DATA/"+bandFileName, getMainFrame().getUI_UserNameAndPassword(), 
							getProgressBar(), textInfo, getTextInfoArea(), true);
					}
				}
				
				Utils.printInfo(getTextInfoArea(), "Download COMPLETE.\n");
				
            } catch (MalformedURLException ex) {}
		
		if(getMainFrame().getUI_btnDownload() != null) getMainFrame().getUI_btnDownload().setEnabled(true);
	}
}
