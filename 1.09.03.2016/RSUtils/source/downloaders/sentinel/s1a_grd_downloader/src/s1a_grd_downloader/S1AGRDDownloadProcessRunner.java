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

package s1a_grd_downloader;

import org.rsutils.downloader.*;
import java.net.*;
import javax.swing.*;
import javax.swing.text.*;
import org.rsutils.*;

public class S1AGRDDownloadProcessRunner extends ProcessRunner implements Runnable
{
	public S1AGRDDownloadProcessRunner(DownLoadMainFrameInterface mainFrameDownloadProgram)
	{
		super(mainFrameDownloadProgram, null, null);
	}
	
	public S1AGRDDownloadProcessRunner(DownLoadMainFrameInterface mainFrameDownloadProgram, JProgressBar prBar, JTextArea txtInfo)
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
				DIR_NAME += "/" + getMainFrame().getUI_ProductName();
			  
				Utils.makeDir(DIR_NAME);
				
				FileDownload.doDownload(new URL(
						"https://scihub.copernicus.eu/dhus/odata/v1/Products('"+
						getMainFrame().getUI_ProductID()+"')/Nodes('"+getMainFrame().getUI_ProductName()+
						"')/Nodes('manifest.safe')/$value"),
						DIR_NAME+"/manifest.safe", getMainFrame().getUI_UserNameAndPassword(), 
						getProgressBar(), textInfo, getTextInfoArea(), false);


				Utils.makeDir(DIR_NAME+"/measurement");
				Utils.makeDir(DIR_NAME+"/annotation");
				
				String anotationXML1 = "s1a-iw-grd-vv-";
				anotationXML1 += getMainFrame().getProductXML();
				anotationXML1 += "-001.xml";
				
				String anotationXML2 = "s1a-iw-grd-vh-";
				anotationXML2 += getMainFrame().getProductXML();
				anotationXML2 += "-002.xml";
								
				textInfo = "Download "+anotationXML1+"...\n";
				
				FileDownload.doDownload(new URL(
							"https://scihub.copernicus.eu/dhus/odata/v1/Products('"+
							getMainFrame().getUI_ProductID()+"')/Nodes('"+getMainFrame().getUI_ProductName()+
							"')/Nodes('annotation')/Nodes('"+anotationXML1+"')/$value"), 
							DIR_NAME+"/annotation/"+anotationXML1, getMainFrame().getUI_UserNameAndPassword(), 
							getProgressBar(), textInfo, getTextInfoArea(), false);
				
				textInfo = "Download "+anotationXML2+"...\n";
				FileDownload.doDownload(new URL(
							"https://scihub.copernicus.eu/dhus/odata/v1/Products('"+
							getMainFrame().getUI_ProductID()+"')/Nodes('"+getMainFrame().getUI_ProductName()+
							"')/Nodes('annotation')/Nodes('"+anotationXML2+"')/$value"), 
							DIR_NAME+"/annotation/"+anotationXML2, getMainFrame().getUI_UserNameAndPassword(), 
							getProgressBar(), textInfo, getTextInfoArea(), false);
								
				Utils.makeDir(DIR_NAME+"/annotation/calibration");
								
				String calibrationXML1 = "calibration-s1a-iw-grd-vv-";
				calibrationXML1 += getMainFrame().getProductXML();
				calibrationXML1 += "-001.xml";
				
				String calibrationXML2 = "calibration-s1a-iw-grd-vh-";
				calibrationXML2 += getMainFrame().getProductXML();
				calibrationXML2 += "-002.xml";
				
				textInfo = "Download "+calibrationXML1+"...\n";
				FileDownload.doDownload(new URL(
							"https://scihub.copernicus.eu/dhus/odata/v1/Products('"+
							getMainFrame().getUI_ProductID()+"')/Nodes('"+getMainFrame().getUI_ProductName()+
							"')/Nodes('annotation')/Nodes('calibration')/Nodes('"+calibrationXML1+"')/$value"), 
							DIR_NAME+"/annotation/calibration/"+calibrationXML1, getMainFrame().getUI_UserNameAndPassword(), 
							getProgressBar(), textInfo, getTextInfoArea(), false);
				
				textInfo = "Download "+calibrationXML2+"...\n";
				FileDownload.doDownload(new URL(
							"https://scihub.copernicus.eu/dhus/odata/v1/Products('"+
							getMainFrame().getUI_ProductID()+"')/Nodes('"+getMainFrame().getUI_ProductName()+
							"')/Nodes('annotation')/Nodes('calibration')/Nodes('"+calibrationXML2+"')/$value"), 
							DIR_NAME+"/annotation/calibration/"+calibrationXML2, getMainFrame().getUI_UserNameAndPassword(), 
							getProgressBar(), textInfo, getTextInfoArea(), false);
				
				calibrationXML1 = "noise-s1a-iw-grd-vv-";
				calibrationXML1 += getMainFrame().getProductXML();
				calibrationXML1 += "-001.xml";
				
				calibrationXML2 = "noise-s1a-iw-grd-vh-";
				calibrationXML2 += getMainFrame().getProductXML();
				calibrationXML2 += "-002.xml";

				textInfo = "Download "+calibrationXML1+"...\n";
				FileDownload.doDownload(new URL(
							"https://scihub.copernicus.eu/dhus/odata/v1/Products('"+
							getMainFrame().getUI_ProductID()+"')/Nodes('"+getMainFrame().getUI_ProductName()+
							"')/Nodes('annotation')/Nodes('calibration')/Nodes('"+calibrationXML1+"')/$value"), 
							DIR_NAME+"/annotation/calibration/"+calibrationXML1, getMainFrame().getUI_UserNameAndPassword(), 
							getProgressBar(), textInfo, getTextInfoArea(), false);
				
				textInfo = "Download "+calibrationXML2+"...\n";
				FileDownload.doDownload(new URL(
							"https://scihub.copernicus.eu/dhus/odata/v1/Products('"+
							getMainFrame().getUI_ProductID()+"')/Nodes('"+getMainFrame().getUI_ProductName()+
							"')/Nodes('annotation')/Nodes('calibration')/Nodes('"+calibrationXML2+"')/$value"), 
							DIR_NAME+"/annotation/calibration/"+calibrationXML2, getMainFrame().getUI_UserNameAndPassword(), 
							getProgressBar(), textInfo, getTextInfoArea(), false);
					
				int [] band_indexes = getMainFrame().getDownloadBandsIndexes();
				if(null == band_indexes)
				{
					for(int i=0; i<2; i++)
					{
						String bandFileName = getMainFrame().getBandName(null, i);                
						textInfo = "Download "+bandFileName+"...\n";
						FileDownload.doDownload(new URL(
							"https://scihub.copernicus.eu/dhus/odata/v1/Products('"+
							getMainFrame().getUI_ProductID()+"')/Nodes('"+getMainFrame().getUI_ProductName()+
							"')/Nodes('measurement')/Nodes('"+bandFileName+"')/$value"), 
							DIR_NAME+"/measurement/"+bandFileName, getMainFrame().getUI_UserNameAndPassword(), 
							getProgressBar(), textInfo, getTextInfoArea(), true);
					}
				}
				else for(int i : band_indexes)
				{
					String bandFileName = getMainFrame().getBandName(null, i);                
					textInfo = "Download "+bandFileName+"...\n";
					FileDownload.doDownload(new URL(
						"https://scihub.copernicus.eu/dhus/odata/v1/Products('"+
						getMainFrame().getUI_ProductID()+"')/Nodes('"+getMainFrame().getUI_ProductName()+
						"')/Nodes('measurement')/Nodes('"+bandFileName+"')/$value"), 
						DIR_NAME+"/measurement/"+bandFileName, getMainFrame().getUI_UserNameAndPassword(), 
						getProgressBar(), textInfo, getTextInfoArea(), true);
				}
				
				Utils.printInfo(getTextInfoArea(), "Download COMPLETE.\n");
				
            } catch (MalformedURLException ex) {}
            
		if(getMainFrame().getUI_btnDownload() != null) getMainFrame().getUI_btnDownload().setEnabled(true);
	}
}
