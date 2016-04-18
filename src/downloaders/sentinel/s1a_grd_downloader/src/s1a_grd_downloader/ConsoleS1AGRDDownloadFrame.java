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
import java.io.*;

public class ConsoleS1AGRDDownloadFrame extends DownLoadMainFrameAdapter
{
	// args[0] - WorkDir
	// args[1] - ProductID
	// args[2] - ProductName
	// args[3] - POLARIZATION
	// args[4] - username:password
	public ConsoleS1AGRDDownloadFrame(String[] args)
	{
		if(args!=null)
		{
			if(args.length == 5)
			{
				m_WorkDir = args[0];
				m_ProductID = args[1];
				m_ProductName = args[2];
				m_POLARIZATION = args[3];
				m_UserNameAndPasword = args[4];
				/*
				if( m_POLARIZATION.compareToIgnoreCase("all") == 0 )
				{
					band_indexes = new int[2];
					band_indexes[0] = 0;
					band_indexes[1] = 1;
				}
				else
				*/
				if( m_POLARIZATION.compareToIgnoreCase("vv") == 0 )
				{
					band_indexes = new int[1];
					band_indexes[0] = 0;
				}
				else
				if( m_POLARIZATION.compareToIgnoreCase("vh") == 0 )
				{
					band_indexes = new int[1];
					band_indexes[0] = 1;
				}				
			}
		}
	}
	
	public String getUI_WorkDir()
	{
		return m_WorkDir;
	}
	
	public String getUI_ProductID()
	{
		return m_ProductID;
	}
	
	public String getUI_ProductName()
	{
		return m_ProductName;
	}
	
	public String getUI_Polarization()
	{
		return m_POLARIZATION;
	}
	
	public String getUI_UserNameAndPassword()
	{
		return m_UserNameAndPasword;
	}
		
	public String getProductXML()
	{
		String productName = m_ProductName;
        int begin_pos = 0;
        int end_pos = 0;
        int count = 0;

        int i=0;
        while(productName.charAt(i)!='.')
        {
            if(productName.charAt(i)=='_') count++;
            if(count == 4) { begin_pos = i+1; break; }
            i++;
        }
        i=productName.length()-1;
        while(productName.charAt(i)!='_') i--;
        end_pos = i;
        
        String mainProductName = productName.substring(begin_pos, end_pos).replace('_', '-').toLowerCase();
                
        return mainProductName;
	}
		
	public String getBandName(String granuleName, int index)
	{
		String bandName = "";
		switch(index)
		{
			case 0: // vv
					bandName = "s1a-iw-grd-vv-";
					bandName += getProductXML();
					bandName += "-001.tiff";
					break;
			case 1: // vh
					bandName = "s1a-iw-grd-vh-";
					bandName += getProductXML();
					bandName += "-002.tiff";
					break;
		}
		return bandName;
	}
	
	public int[] getDownloadBandsIndexes()
	{
		return band_indexes;
	}
	
	protected String m_WorkDir = "";	
	protected String m_UserNameAndPasword = ""; // "username:password"
	protected String m_POLARIZATION = "";
	protected String m_ProductName = "";
	protected String m_ProductID = "";
	protected int [] band_indexes = null;
}
