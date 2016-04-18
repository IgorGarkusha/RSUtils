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

package modis_downloader;

import org.rsutils.downloader.*;
import java.io.*;

public class ConsoleModisDownloadFrame extends DownLoadMainFrameAdapter
{
	// args[0] - WorkDir
	// args[1] - ProductName
	// args[2] - Year
	// args[3] - From_Month
	// args[4] - To_Month
	// args[5] - TileName1
	// args[6] - TileName2
	// ...
	public ConsoleModisDownloadFrame(String[] args)
	{
		if(args!=null)
		{
			if(args.length >= 6)
			{
				m_WorkDir = args[0];
				m_ProductName = args[1];
				m_Year = args[2];
				m_From_Month = args[3];
				m_To_Month = args[4];
				
				m_tile_name = new String[args.length-5];
				band_indexes = new int[args.length-5];
				for(int i=5; i<args.length; i++) {m_tile_name[i-5] = args[i]; band_indexes[i-5] = i-5;}
			}
		}
	}
	
	public String getUI_WorkDir()
	{
		return m_WorkDir;
	}
	
	public String getUI_ProductName()
	{
		return m_ProductName;
	}
	
	public String getUI_Year()
	{
		return m_Year;
	}
	
	public String getUI_FromMonth()
	{
		return m_From_Month;
	}
	
	public String getUI_ToMonth()
	{
		return m_To_Month;
	}
		
	public String getBandName(String granuleName, int index)
	{
		if(m_tile_name!=null)
		{
			if((index>=0)&&(index<m_tile_name.length)) return m_tile_name[index];
			else return "";
		}else return "";
	}
	
	public int[] getDownloadBandsIndexes()
	{
		return band_indexes;
	}
	
	protected String m_WorkDir = "";
	protected String m_ProductName = "";
	protected String m_Year = "";
	protected String m_From_Month = "";
	protected String m_To_Month = "";
	protected String [] m_tile_name = null;
	protected int [] band_indexes = null;
}
