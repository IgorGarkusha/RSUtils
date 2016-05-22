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
import java.io.*;

public class ConsoleS2ADownloadFrame extends DownLoadMainFrameAdapter
{
	// args[0] - WorkDir
	// args[1] - ProductID
	// args[2] - ProductName
	// args[3] - UTM_TILE
	// args[4] - username:password
	public ConsoleS2ADownloadFrame(String[] args)
	{
		if(args!=null)
		{
			if(args.length >= 5)
			{
				m_WorkDir = args[0];
				m_ProductID = args[1];
				m_ProductName = args[2];
				m_UTMTILE = args[3];
				m_UserNameAndPasword = args[4];

				if(args.length > 5)
				{
					band_indexes = new int[args.length - 5];
					for(int i=5, j=0; i<args.length; i++, j++)
					{
						if(args[i].compareToIgnoreCase("8a") == 0) band_indexes[j] = 13;
						else band_indexes[j] = Integer.parseInt(args[i]);
					}
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
	
	public String getUI_UTM_TILE()
	{
		return m_UTMTILE;
	}
	
	public String getUI_UserNameAndPassword()
	{
		return m_UserNameAndPasword;
	}
	
	public String getSUFFIX()
	{
		return m_SUFFIX;
	}
	
	public String getProductXML()
	{
		String productXMLFileName = "S2A_OPER_MTD_SAFL1C_PDMC_";
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
        end_pos = 0; while(productName.charAt(end_pos)!='.') end_pos++;       
        productXMLFileName += (productName.substring(begin_pos, end_pos) + ".xml");
        return productXMLFileName;
	}
	
	public String getGranuleName(String manifestSafePathName)
	{
		BufferedReader in = null;
        try
        {
            String res = "";
            in = new BufferedReader(new FileReader(manifestSafePathName));
            int ch = (int)'0';
            String buff = "";
            while((ch=in.read())!=-1)
            {
                buff += (char)ch;
                if(buff.length() == 9)
                {
                    if(buff.compareToIgnoreCase("/GRANULE/") == 0)
                    {
                        while((ch = in.read())!='/') res += (char)ch;
                        break;
                    }
                    else buff = "";
                }
            }
            in.close();
            
            int begin_pos = 0;
            int end_pos = 0;
            int count = 0;
            for(int i=res.length()-1; i>=0; i--)
            {
                if(res.charAt(i) == '_') count++;
                if(count == 2) { end_pos = i; break; }
            }
            String resStr = res.substring(begin_pos, end_pos);
            
            
            begin_pos = 0;
            end_pos = res.length();
            count = 0;
            for(int i=res.length()-1; i>=0; i--)
            {
                if(res.charAt(i) == '_') count++;
                if(count == 1) { begin_pos = i+1; break; }
            }
            m_SUFFIX = res.substring(begin_pos, end_pos);
            
            return resStr;
        }
        catch(IOException e)
        {
            return "";
        }  
	}
	
	private String CutPrefixGranuleTileFileName(String granuleName)
    {
        int begin_pos = 0;
        int count = 0;
        while(count<3)
        {
            if(granuleName.charAt(begin_pos) == '_') count++;
            begin_pos++;
        }
        return granuleName.substring(begin_pos, granuleName.length());
    }
	
	public String getGranuleXMLFileName(String granuleName)
	{
		return "S2A_OPER_MTD_" + CutPrefixGranuleTileFileName(granuleName)+ "_T" + m_UTMTILE + ".xml";
	}
	
	private String getGranuleTileFileName(String granuleName)
    {
        return granuleName + "_T" + m_UTMTILE;
    }
	
	public String getBandName(String granuleName, int index)
	{
		if(index < 10) return getGranuleTileFileName(granuleName) + "_B0" + Integer.toString(index) + ".jp2";
        else if(index < 13) return getGranuleTileFileName(granuleName) + "_B" + Integer.toString(index) + ".jp2";
        else return getGranuleTileFileName(granuleName) + "_B8A.jp2";
	}
	
	public int[] getDownloadBandsIndexes()
	{
		return band_indexes;
	}
	
	protected String m_WorkDir = "";
	protected String m_UserNameAndPasword = ""; // "username:password"
	protected String m_SUFFIX = "";
	protected String m_UTMTILE = "";
	protected String m_ProductName = "";
	protected String m_ProductID = "";
	protected int [] band_indexes = null;
}
