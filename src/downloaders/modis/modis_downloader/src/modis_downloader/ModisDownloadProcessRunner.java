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

package modis_downloader;

import org.rsutils.*;
import org.rsutils.downloader.*;
import java.net.*;
import java.io.*;
import javax.swing.*;
import javax.swing.text.*;

public class ModisDownloadProcessRunner extends ProcessRunner implements Runnable
{
	public ModisDownloadProcessRunner(DownLoadMainFrameInterface mainFrameDownloadProgram)
	{
		super(mainFrameDownloadProgram, null, null);
	}
	
	public ModisDownloadProcessRunner(DownLoadMainFrameInterface mainFrameDownloadProgram, JProgressBar prBar, JTextArea txtInfo)
	{
		super(mainFrameDownloadProgram, prBar, txtInfo);
	}
		
	@Override
    public void run()
	{		
		if(getMainFrame().getUI_btnDownload() != null) getMainFrame().getUI_btnDownload().setEnabled(false);
		
		String DIR_NAME = "";
        String textInfo = "";
        String prevExt = " ";
		
        try {
				Utils.printInfo(getTextInfoArea(), "Processing for product "+getMainFrame().getUI_ProductName()+"...\n");
				
				StringBuffer text = null;				
				HttpURLConnection conn = null;
				InputStreamReader in = null;
				BufferedReader buff = null;
				
				HttpURLConnection conn2 = null;
				InputStreamReader in2 = null;
				BufferedReader buff2 = null;
								
				String line, line2;
				int begin_pos = 0, end_pos = 0;
				
				int fromMonth = Integer.parseInt(getMainFrame().getUI_FromMonth());
				int toMonth = Integer.parseInt(getMainFrame().getUI_ToMonth());

				URL page_product_dates = new URL(productModisURL + getMainFrame().getUI_ProductName() +"/");
				// For Example:
				// Example select date: <a href="2015.03.14/">2015.03.14/</a> 
				// URL page = new URL("http://e4ftl01.cr.usgs.gov/MOLT/MOD17A2H.006/2015.08.21/");
				// Example select data: <a href="MOD17A2H.A2015233.h20v04.006.2015306021314.hdf">
				// 						<a href="MOD17A2H.A2015233.h20v04.006.2015306021314.hdf.xml">
				//						<a href="BROWSE.MOD17A2H.A2015233.h20v04.006.2015306021314.1.jpg">
				//						<a href="BROWSE.MOD17A2H.A2015233.h20v04.006.2015306021314.2.jpg">
				text = new StringBuffer();
				conn = (HttpURLConnection) page_product_dates.openConnection();
				conn.connect();
				in = new InputStreamReader((InputStream) conn.getContent());
				buff = new BufferedReader(in);
				
				Utils.printInfo(getTextInfoArea(), "Getting data ...\n");
				do
				{
					line = buff.readLine();
					if(line != null)
					{
						begin_pos = line.indexOf("<a href=\"2");
						if(begin_pos != -1)
						{
							begin_pos += 9;
							end_pos = begin_pos;
							while(line.charAt(end_pos)!='/') end_pos++;
							
							String strDate = line.substring(begin_pos, end_pos);

							begin_pos = strDate.indexOf(getMainFrame().getUI_Year());
							if(begin_pos != -1) 
							{
								String month = "";
								month += strDate.charAt(5);
								month += strDate.charAt(6);
								int iMonth = Integer.parseInt(month);
																	
								if( (iMonth>=fromMonth)&&(iMonth<=toMonth) )
								{
									DIR_NAME = getMainFrame().getUI_WorkDir();
									DIR_NAME += "/"+getMainFrame().getUI_ProductName();
									Utils.makeDir(DIR_NAME);
									DIR_NAME += "/"+strDate;

									if(Utils.makeDir(DIR_NAME))
									{											
										URL page_product_data = new URL(productModisURL + getMainFrame().getUI_ProductName() + "/"+ strDate +"/");
		
										conn2 = (HttpURLConnection)page_product_data.openConnection();
										conn2.connect();
										in2 = new InputStreamReader((InputStream)conn2.getContent());
										buff2 = new BufferedReader(in2);
										
										Utils.printInfo(getTextInfoArea(), "Find tiles for "+ strDate +"...\n");
										do
										{
											line2 = buff2.readLine();
											if(line2 != null)
											{
												for(int i=0; i<getMainFrame().getDownloadBandsIndexes().length; i++)
												{
													int isTile = line2.indexOf(getMainFrame().getBandName(null, i));
													if(isTile != -1)
													{
														begin_pos = line2.indexOf("<a href=\"MOD");
														if(begin_pos != -1)
														{
															begin_pos += 9;
															end_pos = begin_pos;
															while(line2.charAt(end_pos)!='\"') end_pos++;
															String fileName = line2.substring(begin_pos, end_pos);
															/*														
															if(prevExt.compareToIgnoreCase("hdf") == 0)
															{
																textInfo = "\nTimeout 15 seconds...\n";
																getTextInfoArea().append(textInfo);
																try{ getTextInfoArea().setCaretPosition(getTextInfoArea().getLineStartOffset(getTextInfoArea().getLineCount()-1));
																}catch(BadLocationException e){}
																Utils.timeout();
															}
															*/
															textInfo = "Download: "+ fileName+"...\n";
															
															FileDownload.doDownload(new URL(productModisURL + getMainFrame().getUI_ProductName() +"/"+ strDate +"/"+ fileName), 
																					DIR_NAME+"/"+fileName, getProgressBar(), textInfo, getTextInfoArea(), false);
															
															prevExt = Utils.getFileNameExt(fileName);
														}
													}
												}
											}
										}while(line2 != null);
													
										in2.close();
										in2 = null;
										buff2 = null;
										conn2 = null;											
									}
								}
							}
						}
					}
				}while(line != null);
				
				in.close();
				in = null;
				buff = null;
				conn = null;
			}
			catch(MalformedURLException e){}
			catch(IOException e){}
				
			Utils.printInfo(getTextInfoArea(), "Download COMPLETE.\n");
				
		if(getMainFrame().getUI_btnDownload() != null) getMainFrame().getUI_btnDownload().setEnabled(true);
	}

	private String productModisURL = "http://e4ftl01.cr.usgs.gov/MOLT/";
}
