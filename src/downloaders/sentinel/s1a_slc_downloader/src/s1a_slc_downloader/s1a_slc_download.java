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

import javax.swing.*;

public class s1a_slc_download
{
	public static final String PROG_VERSION = "1";
	public static final String DATE_VERSION = "22.05.2016";
	
	public static void main(String[] args)
	{
		if(args.length >= 4) startConsole(args);
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
       
		SwingUtilities.invokeLater(new S1ASLCDownloadFrame());
	}
	
	public static void startConsole(String[] args)
	{
		// args[0] - WorkDir
		// args[1] - ProductID
		// args[2] - ProductName
		// args[3] - POLARIZATION
		// --- args[4] - Interferometric Wide Swath Number (1-3, all)
		// args[4] - username:password
		ConsoleS1ASLCDownloadFrame params = new ConsoleS1ASLCDownloadFrame(args);
		new Thread(new S1ASLCDownloadProcessRunner(params)).start();
	}
	
	public static void printHelp()
	{
		System.out.printf("Sentinel-1A SLC-Product Downloader\nVersion %s.%s. Free software. GNU General Public License, version 3\n", PROG_VERSION, DATE_VERSION);
		System.out.println("Copyright (C) 2016 Igor Garkusha.\nUkraine, Dnipropetrovsk\n\n");
		
		System.out.println("Start with GUI:");
		System.out.println("\ts1a_slc_download");
		System.out.println("Console parameters:");
		
		System.out.println("\ts1a_slc_downloader \"<WorkDirectory> <ProductID> <ProductName> <POLARIZATION> <Username:Password>\"");
		
		System.out.println("\nPOLARIZATION: all");
		System.out.println("              vv");
		System.out.println("              vh");

		System.out.println("\nConsole parameters examples:");
		System.out.println("\ts1a_slc_downloader \"/home/username/processing/data 2d20e207-ec48-4221-8958-0fb50af618c2 S1A_IW_SLC__1SDV_20160508T035544_20160508T035611_011162_010D9D_4770.SAFE all Username:Password\"");
		System.out.println("\ts1a_slc_downloader \"/home/username/processing/data 2d20e207-ec48-4221-8958-0fb50af618c2 S1A_IW_SLC__1SDV_20160508T035544_20160508T035611_011162_010D9D_4770.SAFE vv Username:Password\"");
		
		/*
		System.out.println("\ts1a_slc_downloader \"<WorkDirectory> <ProductID> <ProductName> <POLARIZATION> <IW_number> <Username:Password>\"");
		
		System.out.println("\nPOLARIZATION: all");
		System.out.println("              vv");
		System.out.println("              vh");
		
		System.out.println("\nIW_number: all, 1, 2, 3");
		        		
		System.out.println("\nConsole parameters examples:");
		System.out.println("\ts1a_slc_downloader \"/home/username/processing/data 2d20e207-ec48-4221-8958-0fb50af618c2 S1A_IW_SLC__1SDV_20160508T035544_20160508T035611_011162_010D9D_4770.SAFE all all Username:Password\"");
		System.out.println("\ts1a_slc_downloader \"/home/username/processing/data 2d20e207-ec48-4221-8958-0fb50af618c2 S1A_IW_SLC__1SDV_20160508T035544_20160508T035611_011162_010D9D_4770.SAFE vv 1 Username:Password\"");
		*/
		System.out.println();
	}
}
