/*
 * Project: Remote Sensing Utilities (Extentions GDAL/OGR)
 * Author:  Igor Garkusha <rsutils.gis@gmail.com>
 *          Ukraine, Dnipro (Dnipropetrovsk)
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

import java.io.*;
import java.util.*;
import org.rsutils.downloader.*;
import java.net.*;
import javax.swing.*;
import javax.swing.text.*;
import org.rsutils.*;

public class AWS_S2_DownloadProcessRunner extends ProcessRunner implements Runnable
{
	public AWS_S2_DownloadProcessRunner(DownLoadMainFrameInterface mainFrameDownloadProgram)
	{
		super(mainFrameDownloadProgram, null, null);
		
		m_optionDecompress = true;
		m_optionCheckReflectance = true;
		m_optionGdalMerge = false;
	}
	
	public AWS_S2_DownloadProcessRunner(DownLoadMainFrameInterface mainFrameDownloadProgram, JProgressBar prBar, JTextArea txtInfo,
										boolean optionDecompress, boolean optionCheckReflectance, boolean optionGdalMerge)
	{
		super(mainFrameDownloadProgram, prBar, txtInfo);
		m_optionDecompress = optionDecompress;
		m_optionCheckReflectance = optionCheckReflectance;
		m_optionGdalMerge = optionGdalMerge;
	}
	
	private String getProductPath(String jsonProductFileName)
	{
		String res = "";
		Scanner fin = null;
		try
		{
			fin = new Scanner(new File(jsonProductFileName));
			int countLines = 0;
			while(fin.hasNextLine())
			{
				String str = fin.nextLine();
				if( str.indexOf("path") >= 0 )
				{
					int start_pos = str.indexOf("products");
					int end_pos = str.lastIndexOf("\",");
					res = str.substring(start_pos, end_pos);
					break;
				}
			}			
		}
		catch(FileNotFoundException ex)
		{
		}
		finally
		{
			if( null != fin ) { fin.close(); fin = null; }
		}
		
		return res;
	}
		
	@Override
    public void run()
	{		
		if(getMainFrame().getUI_btnDownload() != null) getMainFrame().getUI_btnDownload().setEnabled(false);
			
		String DIR_NAME = "";
        String textInfo = "";

        try {
				DIR_NAME = getMainFrame().getUI_WorkDir();
				
				Utils.makeDir(DIR_NAME);
				DIR_NAME += Utils.getDirSlash();
				
				String tileURL = getMainFrame().getUI_ProductName();
				
				int start_pos_part_base_url = tileURL.lastIndexOf("#tiles")+7;
				
				String baseURL = "http://sentinel-s2-l1c.s3.amazonaws.com/tiles/" + tileURL.substring(start_pos_part_base_url, tileURL.length() );
				
				if( baseURL.charAt( baseURL.length()-1 ) != '/' ) baseURL += "/";
				
				textInfo = "Processing for link:\n"+getMainFrame().getUI_ProductName()+"\n" + "Download information of product...\n";
				
				FileDownload.doDownload(new URL(baseURL + "productInfo.json"), DIR_NAME + "productInfo.json", getProgressBar(), textInfo, getTextInfoArea(), false);
				
				String productMetadataURL = "http://sentinel-s2-l1c.s3.amazonaws.com/" + getProductPath(DIR_NAME + "productInfo.json") + "/metadata.xml";
				
				textInfo = "Download product metadata...\n";
				FileDownload.doDownload(new URL(productMetadataURL), DIR_NAME + "product_metadata.xml", getProgressBar(), textInfo, getTextInfoArea(), false);
								
				textInfo = "Download tile metadata...\n";
				
				FileDownload.doDownload(new URL(baseURL + "metadata.xml"), DIR_NAME + "tile_metadata.xml", getProgressBar(), textInfo, getTextInfoArea(), false);
												
				textInfo = "Download tile preview...\n";
				
				FileDownload.doDownload(new URL(baseURL + "preview.jpg"), DIR_NAME + "preview.jpg", getProgressBar(), textInfo, getTextInfoArea(), false);
				
				
				int [] band_indexes = getMainFrame().getDownloadBandsIndexes();
				if(null == band_indexes)
				{
					for(int i=1; i<=13; i++)
					{
						String bandFileName = "";
						if( i == 13 ) bandFileName = "B8A";
						else
						if( i<10 ) bandFileName = "B0" + Integer.toString(i);
						else
						bandFileName = "B" + Integer.toString(i);
												
						textInfo = "Download "+bandFileName+"...\n";
						
						FileDownload.doDownload(new URL(baseURL + bandFileName + ".jp2"), DIR_NAME + bandFileName + ".jp2", getProgressBar(), textInfo, getTextInfoArea(), false);
						
						postDownloading(DIR_NAME + bandFileName + ".jp2", getTextInfoArea(), Utils.getSpatialResolution(Utils.SENTINEL2_MSI, i) );
					}
				}
				else
				{
					for(int i : band_indexes)
					{
						String bandFileName = "";
						if( i == 13 ) bandFileName = "B8A";
						else
						if( i<10 ) bandFileName = "B0" + Integer.toString(i);
						else
						bandFileName = "B" + Integer.toString(i);
												
						textInfo = "Download "+bandFileName+"...\n";
						
						FileDownload.doDownload(new URL(baseURL + bandFileName + ".jp2"), DIR_NAME + bandFileName + ".jp2", getProgressBar(), textInfo, getTextInfoArea(), false);
						
						postDownloading(DIR_NAME + bandFileName + ".jp2", getTextInfoArea(), Utils.getSpatialResolution(Utils.SENTINEL2_MSI, i) );
					}
				}
				
				if( m_optionGdalMerge )
				{
					if(null != band_indexes)
					{
						String bandNames = "";
						String mergeFileName = DIR_NAME + Integer.toString( Utils.getSpatialResolution(Utils.SENTINEL2_MSI, band_indexes[0]) ) + "M.tif";
						
						for(int i : band_indexes)
						{
							String bandFileName = "";
							if( i == 13 ) bandFileName = "B8A";
							else
							if( i<10 ) bandFileName = "B0" + Integer.toString(i);
							else
							bandFileName = "B" + Integer.toString(i);
						
							bandNames += DIR_NAME + bandFileName + ".tif ";
						}
						
						Utils.printInfo(getTextInfoArea(), "Execute GDAL_MERGE for " + mergeFileName + "...\n");
						if( Utils.getOSType() == Utils.OS_UNIX ) Utils.runExternProgram("gdal_merge.py -separate -o "+mergeFileName+" "+bandNames);
						else Utils.runExternProgram("cmd /c create_stack.cmd "+mergeFileName+" "+bandNames);
					}
					
				}
				
				Utils.printInfo(getTextInfoArea(), "Process COMPLETE.\n");
					
            }catch(Exception ex) {}
		
		if(getMainFrame().getUI_btnDownload() != null) getMainFrame().getUI_btnDownload().setEnabled(true);
	}
	
	private void postDownloading(String fileName, JTextArea txtInfo, int resolution)
	{
		if( m_optionDecompress )
		{
			String name = fileName.substring( 0, fileName.lastIndexOf(".jp2") );
			String DirName = name.substring( 0, name.lastIndexOf(Utils.getDirSlash())+1 );
			String outputFileName = name + "_tmp.tif";
			String outputRefFileName = "";
					
			if( false == m_optionCheckReflectance )	outputRefFileName = name + ".tif";
			else outputRefFileName = name + "_tmp2.tif";
			
			String outputCorrRefFileName = name + ".tif";
						
			Utils.printInfo(txtInfo, "Decompress " + fileName + "...\n");
			if( Utils.getOSType() == Utils.OS_UNIX ) Utils.runExternProgram("opj_decompress_jp2 "+fileName+" "+outputFileName);
			else Utils.runExternProgram("cmd.exe /C opj_decompress_jp2.cmd "+fileName+" "+outputFileName);
			
			Utils.printInfo(txtInfo, "Set projection " + outputRefFileName + "...\n");
			if( Utils.getOSType() == Utils.OS_UNIX ) Utils.runExternProgram("s2_set_projection "+outputFileName+" "+DirName+"product_metadata.xml "+
													 DirName+"tile_metadata.xml "+Integer.toString(resolution)+" "+outputRefFileName+" 0");
			else Utils.runExternProgram("cmd /c s2_set_projection "+outputFileName+" "+DirName+"product_metadata.xml "+
										 DirName+"tile_metadata.xml "+Integer.toString(resolution)+" "+outputRefFileName+" 0");
			
			if( Utils.getOSType() == Utils.OS_UNIX ) Utils.runExternProgram("rm -f "+outputFileName);
			else Utils.runExternProgram("cmd /c del "+outputFileName);
			
			if( m_optionCheckReflectance )
			{
				Utils.printInfo(txtInfo, "Check and correction TOA spectral reflectance " + outputCorrRefFileName + "...\n");
				if( Utils.getOSType() == Utils.OS_UNIX ) Utils.runExternProgram("check_reflectance "+outputRefFileName+" 1 "+outputCorrRefFileName);
				else Utils.runExternProgram("cmd /c check_reflectance "+outputRefFileName+" 1 "+outputCorrRefFileName);
				
				if( Utils.getOSType() == Utils.OS_UNIX ) Utils.runExternProgram("rm -f "+outputRefFileName);
				else Utils.runExternProgram("cmd /c del "+outputRefFileName);
			}
		}
	}
	
	private boolean m_optionDecompress = false;
	private boolean m_optionCheckReflectance =false;
	private boolean m_optionGdalMerge = false;
}
