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
import java.io.*;

public class ConsoleS1ASLCDownloadFrame extends DownLoadMainFrameAdapter
{
	// args[0] - WorkDir
	// args[1] - ProductID
	// args[2] - ProductName
	// args[3] - POLARIZATION
	// --- args[4] - Interferometric Wide Swath Number (1-3, all)
	// args[4] - username:password
	public ConsoleS1ASLCDownloadFrame(String[] args)
	{
		if(args!=null)
		{
			if(args.length == 5) // == 6)
			{
				m_WorkDir = args[0];
				m_ProductID = args[1];
				m_ProductName = args[2];
				m_POLARIZATION = args[3];
				m_IW_Number = "all"; //args[4];
				m_UserNameAndPasword = args[4];// 5];
				
				m_slcBandName = new String[6];
				
				initPolAndIW(m_POLARIZATION, m_IW_Number);
			}
		}
	}
	
	protected void initPolAndIW(String POLARIZATION, String IW_Number)
	{
		band_indexes  = null;
			
		if(( POLARIZATION.compareToIgnoreCase("vh") == 0 )&&( IW_Number.compareToIgnoreCase("all") == 0 ) )
		{
			band_indexes = new int[3];
			band_indexes[0] = 1;
			band_indexes[1] = 2;
			band_indexes[2] = 3;
		}
		else
		if(( POLARIZATION.compareToIgnoreCase("vv") == 0 )&&( IW_Number.compareToIgnoreCase("all") == 0 ) )
		{
			band_indexes = new int[3];
			band_indexes[0] = 4;
			band_indexes[1] = 5;
			band_indexes[2] = 6;
		}
		else
		if( POLARIZATION.compareToIgnoreCase("vh") == 0 )
		{
			band_indexes = new int[1];
			band_indexes[0] = Integer.parseInt(IW_Number);
		}
		else
		if( POLARIZATION.compareToIgnoreCase("vv") == 0 )
		{
			band_indexes = new int[1];
			band_indexes[0] = Integer.parseInt(IW_Number)+3;
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
            if(count == 5) { begin_pos = i+1; break; }
            i++;
        }
        i=productName.length()-1;
        while(productName.charAt(i)!='_') i--;
        end_pos = i;
        
        String mainProductName = productName.substring(begin_pos, end_pos).replace('_', '-').toLowerCase();
                
        return mainProductName;
	}
		
	public void loadFromManifestData(String manifestSafePathName)
	{
		BufferedReader in = null;
        try
        {
            String res = "";
            
            in = new BufferedReader(new FileReader(manifestSafePathName));
            int ch = (int)'0';
            String buff = "";
            int bandCount = 0;
            while((ch=in.read())!=-1)
            {
                buff += (char)ch;
                if(buff.length() == 13)
                {
                    if(buff.compareToIgnoreCase("/measurement/") == 0)
                    {
                        while((ch = in.read())!='"') res += (char)ch;
												
						m_slcBandName[bandCount] = res;
						bandCount++;

						res  = "";
						buff = "";
												
						if(bandCount == 6) break;
                    }
                    else 
                    {
						buff = buff.substring(1, buff.length());
					}
                }
            }
            in.close();
        }
        catch(IOException e){}  
	}
		
	public String getBandName(String granuleName, int index)
	{
		if(m_slcBandName != null)
		{
			for(String bandName : m_slcBandName)
			{
				String strBandNum = bandName.substring(bandName.length()-6, bandName.length()-5);
				int i = Integer.parseInt(strBandNum);
				if(i == index) return bandName;
			}
		}
		return ""; // Unknown band name
	}
		
	public int[] getDownloadBandsIndexes()
	{
		return band_indexes;
	}
	
	protected String m_WorkDir = "";	
	protected String m_UserNameAndPasword = ""; // "username:password"
	protected String m_POLARIZATION = "";
	protected String m_IW_Number = "";
	protected String m_ProductName = "";
	protected String m_ProductID = "";
	protected int [] band_indexes = null;
	protected String [] m_slcBandName = null;
}
