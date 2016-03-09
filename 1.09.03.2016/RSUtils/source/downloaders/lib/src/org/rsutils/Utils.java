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

package org.rsutils;

import java.io.*;
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
}
