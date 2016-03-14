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

import javax.swing.*;
import org.rsutils.*;

public class modis_download
{
	public static final String PROG_VERSION = "1";
	public static final String DATE_VERSION = "09.03.2016";
	
	public static void main(String[] args)
	{
		if(args.length >= 5) startConsole(args);
		else if(args.length == 1) {
			if( (args[0].compareToIgnoreCase("-h") == 0) ||
				(args[0].compareToIgnoreCase("-help") == 0) ||
				(args[0].compareToIgnoreCase("help") == 0) ||
				(args[0].compareToIgnoreCase("--help") == 0) ||
				(args[0].compareToIgnoreCase("/h") == 0) ||
				(args[0].compareToIgnoreCase("/help") == 0) ||
				(args[0].compareToIgnoreCase("/?") == 0) ) printHelp();
		}
		else if(args.length == 0) startGUI();
	}
	
	public static void startGUI()
	{
		 try {
            for (javax.swing.UIManager.LookAndFeelInfo info : javax.swing.UIManager.getInstalledLookAndFeels()) {
                if ("Nimbus".equals(info.getName())) {
                    javax.swing.UIManager.setLookAndFeel(info.getClassName());
                    break;
                }
            }
        }
        catch(Exception ex){}
       
		SwingUtilities.invokeLater(new ModisDownloadFrame());
	}
	
	public static void startConsole(String[] args)
	{
		// args[0] - WorkDir
		// args[1] - ProductName
		// args[2] - Year
		// args[3] - From_Month
		// args[4] - To_Month
		// args[5] - TileName1
		// args[6] - TileName2
		// ...
		ConsoleModisDownloadFrame params = new ConsoleModisDownloadFrame(args);
		new Thread(new ModisDownloadProcessRunner(params)).start();
	}
	
	public static void printHelp()
	{
		System.out.printf("Terra MODIS Product Downloader. Version %s.%s. Free software. GNU General Public License, version 3\n", PROG_VERSION, DATE_VERSION);
		System.out.println("Copyright (C) 2016 Igor Garkusha.\nUkraine, Dnipropetrovsk\n\n");
		
		System.out.println("Start with GUI:");
		System.out.println("\tmodis_download");
		System.out.println("Console parameters:");
		System.out.println("\tmodis_download \"<WorkDirectory> <ProductName> <Year> <From_Month> <To_Month> <TileName1> [<TileName2> ... <TileNameN>]\"");
		System.out.println("Console parameters examples:");
		System.out.println("\tmodis_download \"/home/username/processing/data MOD17A2H.006 2015 04 09 h19v03 h19v04 h20v03 h20v04\"");
		System.out.println("\tmodis_download \"/Users/username/processing/data MOD17A2H.006 2015 04 09 h19v03 h19v04 h20v03 h20v04\"");
		System.out.println("\tmodis_download C:\\Processing\\Data MOD17A2H.006 2015 04 09 h19v03 h19v04 h20v03 h20v04");
		
		System.out.println();
	}
}
