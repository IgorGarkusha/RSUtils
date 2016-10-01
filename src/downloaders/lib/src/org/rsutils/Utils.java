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

package org.rsutils;

import java.io.*;
import java.util.*;
import javax.swing.*;
import javax.swing.text.*;

public class Utils
{
	private Utils(){}
	
	public static String getFileNameExt(String fileName) 
	{
       if(fileName.lastIndexOf(".") > 0)
			return fileName.substring(fileName.lastIndexOf(".")+1);
       else 
			return "";
    }
    
    public static boolean fileExists(String fileName)
    {
		File f = new File(fileName);
		return f.exists();
	}
	
	public static long getFileSize(String fileName)
	{
		File f = new File(fileName);
		return f.length();
	}
	
	public static void printInfo(JTextArea txtInfo, String infoString)
	{
		if(txtInfo != null) 
		{
			txtInfo.append(infoString);
			try{ txtInfo.setCaretPosition(txtInfo.getLineStartOffset(txtInfo.getLineCount()-1));
			}catch(BadLocationException e){}
		}
		else System.out.println(infoString);
	}
	
	public static void printProgress(JProgressBar prBar, int progressValue)
    {
		if(prBar != null) prBar.setValue( progressValue );
		else 
		{
			System.out.print("\rProgress: "+progressValue+"%");
			if(progressValue == 100) System.out.println();
		}
	}
	
	public static void timeout()
	{
		try{ Thread.sleep(15000); }catch(InterruptedException e){}
	}
	
	public static boolean makeDir(String newDirName)
	{
		if(fileExists(newDirName)) return true;
		File dir = new File(newDirName);
		return dir.mkdir();
	}
	
	public static String getHumanSize(long sz)
    {
        double d_sz = (double)sz/1024.0;
        if( (long)d_sz < 1024L ) return String.format("%.2f KB", d_sz);
        else
        if( ( (long)d_sz >= 1024L ) && ( (long)d_sz < 1048576L ) ) return String.format("%.2f MB", d_sz/1024.0);
        else
        return String.format("%.2f GB", d_sz/1024.0/1024.0);
    }
    
    public static void Msg(JComponent parent, Object message)
    {
        JOptionPane.showMessageDialog(parent, message);
    }
    
    public static String extractDirName(String fullPathFileName)
    {
		String typeOS = System.getProperty("os.name");
		int startPos = 0;
		if( -1 != typeOS.indexOf("Win") ) startPos = 1;
		if( (0 == fullPathFileName.indexOf("/"))&&(2 == fullPathFileName.indexOf(":"))&&(startPos == 0) ) startPos=1;
		String path = new String("");
		try
		{
			path = fullPathFileName.substring( startPos, fullPathFileName.lastIndexOf(File.separator) );
		}catch(StringIndexOutOfBoundsException e)
		{
			path = fullPathFileName.substring( startPos, fullPathFileName.lastIndexOf('/') );
		}
		return path;
    }
	
	public static String getDirSlash()
	{
		String typeOS = System.getProperty("os.name");
		if( typeOS.indexOf("Win") >= 0 ) return "\\";
		else return "/";
	}
	
    public static String getApplicationName(Object obj)
    {
		return obj.getClass().getProtectionDomain().getCodeSource().getLocation().getPath();
    }
    
    // numberOfBand -- first band: 1
    // S2 Band 8a -- 13
    public static int getSpatialResolution(int sensorID, int numberOfBand)
    {
		switch(sensorID)
		{
			case Utils.LANDSAT8_OLI:
				switch(numberOfBand)
				{
					case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 9: return 30;
					case 8: return 15;
				}
				break;
			
			case Utils.LANDSAT8_TIRS:
				switch(numberOfBand)
				{
					case 10: case 11: return 100;
				}
				break;
				
			case Utils.SENTINEL2_MSI:
				switch(numberOfBand)
				{
					case 2: case 3: case 4: case 8: return 10;
					case 5: case 6: case 7: case 11: case 12: case 13: return 20;
					case 1: case 9: case 10: return 60;
				}
				break;
		}
		return 0;
	}
    
    public static int getOSType()
	{
		String typeOS = System.getProperty("os.name");
		if( typeOS.indexOf("Win") >= 0 ) return Utils.OS_WINDOWS;
		else
		if( typeOS.indexOf("mac") >= 0 ) return Utils.OS_MAC;
		else return Utils.OS_UNIX;
	}
    
    public static void runExternProgram(String cmd_with_parameters)
	{
		List<String> cmd = Arrays.asList( cmd_with_parameters.split(" ") );
		
		try 
		{			
			ProcessBuilder pb = new ProcessBuilder(cmd);
			pb.directory(new File(System.getenv().get("RSUTILS_HOME")));
			pb.redirectErrorStream(true);
			Process p = pb.start();
			
			String outputStr = "";
			BufferedReader buffReader = new BufferedReader(new InputStreamReader(p.getInputStream()));
			while ((outputStr = buffReader.readLine()) != null);
			p.waitFor();
		} 
		catch (Exception ex) { ex.printStackTrace(); }
	}
	
	public static void openWebPage(String strUrl)
	{
		Runtime rt = Runtime.getRuntime();
		
		String[] browsers = {"epiphany", "firefox", "mozilla", "konqueror",
                             "netscape","opera","links","lynx"};
		StringBuffer cmd = null;
		try
		{
			switch( getOSType() )
			{
				case Utils.OS_WINDOWS:
					rt.exec( "rundll32 url.dll,FileProtocolHandler " + strUrl);
					break;
				case Utils.OS_MAC:
					rt.exec( "open" + strUrl);
					break;
				case Utils.OS_UNIX:
					cmd = new StringBuffer();
					for(int i=0; i<browsers.length; i++)
						cmd.append( (i==0  ? "" : " || " ) + browsers[i] +" \"" + strUrl + "\" ");
					rt.exec(new String[] { "sh", "-c", cmd.toString() });
					break;
			}
		}catch(Exception ex) { ex.printStackTrace(); }
	}
	
	public static final int OS_UNIX			= 1;
	public static final int OS_WINDOWS		= 2;
	public static final int OS_MAC			= 3;
	
	public static final int LANDSAT8_OLI 	= 1;
	public static final int LANDSAT8_TIRS	= 2;
	public static final int SENTINEL2_MSI 	= 3;
}
