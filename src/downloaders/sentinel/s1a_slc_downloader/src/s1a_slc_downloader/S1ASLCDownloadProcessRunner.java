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

package s1a_slc_downloader;

import org.rsutils.downloader.*;
import java.net.*;
import javax.swing.*;
import javax.swing.text.*;
import org.rsutils.*;

public class S1ASLCDownloadProcessRunner extends ProcessRunner implements Runnable
{
	public S1ASLCDownloadProcessRunner(DownLoadMainFrameInterface mainFrameDownloadProgram)
	{
		super(mainFrameDownloadProgram, null, null);
	}
	
	public S1ASLCDownloadProcessRunner(DownLoadMainFrameInterface mainFrameDownloadProgram, JProgressBar prBar, JTextArea txtInfo)
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
								
				getMainFrame().loadFromManifestData(DIR_NAME+"/manifest.safe");

				Utils.makeDir(DIR_NAME+"/measurement");
				Utils.makeDir(DIR_NAME+"/annotation");
				
				String [] anotationXML = null;
				
				int [] band_indexes = getMainFrame().getDownloadBandsIndexes();
				
				if(null == band_indexes)
				{
					anotationXML = new String[6];
					for(int i=0; i<6; i++) anotationXML[i] = new String( getMainFrame().getBandName(null, i+1) );
				}
				else
				{
					anotationXML = new String[band_indexes.length];
					int j = 0;
					for(int i=0; i<band_indexes.length; i++, j++) anotationXML[j] = new String( getMainFrame().getBandName(null, band_indexes[i]) );
				}
				
				for(int i=0; i<anotationXML.length; i++)
				{
					// filename.tiff -> filename.xml
					anotationXML[i] = anotationXML[i].substring( 0, anotationXML[i].lastIndexOf('.') );
					anotationXML[i] += ".xml";
					
					textInfo = "Download "+anotationXML[i]+"...\n";
					FileDownload.doDownload(new URL(
							"https://scihub.copernicus.eu/dhus/odata/v1/Products('"+
							getMainFrame().getUI_ProductID()+"')/Nodes('"+getMainFrame().getUI_ProductName()+
							"')/Nodes('annotation')/Nodes('"+anotationXML[i]+"')/$value"), 
							DIR_NAME+"/annotation/"+anotationXML[i], getMainFrame().getUI_UserNameAndPassword(), 
							getProgressBar(), textInfo, getTextInfoArea(), false);
				}
			
				Utils.makeDir(DIR_NAME+"/annotation/calibration");
				
				for(int i=0; i<anotationXML.length; i++)
				{
					String calibrationXML 	= "calibration-";
					String noiseXML 		= "noise-";
					
					calibrationXML += anotationXML[i];
					noiseXML += anotationXML[i];
					
					textInfo = "Download "+calibrationXML+"...\n";
					FileDownload.doDownload(new URL(
							"https://scihub.copernicus.eu/dhus/odata/v1/Products('"+
							getMainFrame().getUI_ProductID()+"')/Nodes('"+getMainFrame().getUI_ProductName()+
							"')/Nodes('annotation')/Nodes('calibration')/Nodes('"+calibrationXML+"')/$value"), 
							DIR_NAME+"/annotation/calibration/"+calibrationXML, getMainFrame().getUI_UserNameAndPassword(), 
							getProgressBar(), textInfo, getTextInfoArea(), false);
							
					textInfo = "Download "+noiseXML+"...\n";
					FileDownload.doDownload(new URL(
							"https://scihub.copernicus.eu/dhus/odata/v1/Products('"+
							getMainFrame().getUI_ProductID()+"')/Nodes('"+getMainFrame().getUI_ProductName()+
							"')/Nodes('annotation')/Nodes('calibration')/Nodes('"+noiseXML+"')/$value"), 
							DIR_NAME+"/annotation/calibration/"+noiseXML, getMainFrame().getUI_UserNameAndPassword(), 
							getProgressBar(), textInfo, getTextInfoArea(), false);
				}
								
				if(null == band_indexes)
				{
					for(int i=0; i<6; i++)
					{
						String bandFileName = getMainFrame().getBandName(null, i+1);                
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
