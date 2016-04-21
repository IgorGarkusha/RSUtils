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

import javax.swing.*;

public class s1a_grd_download
{
	public static final String PROG_VERSION = "1";
	public static final String DATE_VERSION = "21.04.2016";
	
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
       
		SwingUtilities.invokeLater(new S1AGRDDownloadFrame());
	}
	
	public static void startConsole(String[] args)
	{
		// args[0] - WorkDir
		// args[1] - ProductID
		// args[2] - ProductName
		// args[3] - POLARIZATION
		// args[4] - username:password
		ConsoleS1AGRDDownloadFrame params = new ConsoleS1AGRDDownloadFrame(args);
		new Thread(new S1AGRDDownloadProcessRunner(params)).start();
	}
	
	public static void printHelp()
	{
		System.out.printf("Sentinel-1A GRD-Product Downloader\nVersion %s.%s. Free software. GNU General Public License, version 3\n", PROG_VERSION, DATE_VERSION);
		System.out.println("Copyright (C) 2016 Igor Garkusha.\nUkraine, Dnipropetrovsk\n\n");
		
		System.out.println("Start with GUI:");
		System.out.println("\ts1a_grd_download");
		System.out.println("Console parameters:");
		System.out.println("\ts1a_grd_downloader \"<WorkDirectory> <ProductID> <ProductName> <POLARIZATION> <Username:Password>\"");
		
		System.out.println("\nPOLARIZATION: all");
		System.out.println("              vv");
		System.out.println("              vh");
        		
		System.out.println("Console parameters examples:");
		System.out.println("\ts1a_grd_downloader \"/home/username/processing/data c1cc2fd3-7aff-460c-8041-d344e0726d4b S1A_IW_GRDH_1SDV_20160214T035518_20160214T035543_009937_00E967_4E23.SAFE all Username:Password\"");
		System.out.println("\ts1a_grd_downloader \"/Users/username/processing/data c1cc2fd3-7aff-460c-8041-d344e0726d4b S1A_IW_GRDH_1SDV_20160214T035518_20160214T035543_009937_00E967_4E23.SAFE vv Username:Password\"");
		System.out.println("\ts1a_grd_downloader C:\\Processing\\Data c1cc2fd3-7aff-460c-8041-d344e0726d4b S1A_IW_GRDH_1SDV_20160214T035518_20160214T035543_009937_00E967_4E23.SAFE vh Username:Password");
		
		System.out.println();
	}
}
