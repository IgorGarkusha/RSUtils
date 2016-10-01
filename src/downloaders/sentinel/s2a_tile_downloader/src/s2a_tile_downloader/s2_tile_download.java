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

package s2a_tile_downloader;

import javax.swing.*;

public class s2_tile_download
{
	public static final String PROG_VERSION = "1";
	public static final String DATE_VERSION = "24.09.2016";
	
	public static void main(String[] args)
	{
		if(args.length == 2) startConsole(args);
		else
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
       
		SwingUtilities.invokeLater(new S2ADownloadFrame());
	}
	
	public static void startConsole(String[] args)
	{
		// Scientific Data Hub
		// args[0] - WorkDir
		// args[1] - ProductID
		// args[2] - ProductName
		// args[3] - UTM_TILE
		// args[4] - username:password
		// args[5] - 1
		// args[6] - 2 
		
		// 6 7 8 9 10 11 12 13 14 15 16
		// 3 4 5 6 7   8  9 10 11 12 8a
		// . . .
		// args[16] - 13 (8a)
		
		// Sentinel-2 on AWS cloud
		// args[0] - WorkDir
		// args[1] - AWS Direct Link
		
		if( args.length == 2 )
		{
			ConsoleS2ADownloadFrame params = new ConsoleS2ADownloadFrame(args);
			new Thread(new AWS_S2_DownloadProcessRunner(params)).start();
		}
		else
		{
			ConsoleS2ADownloadFrame params = new ConsoleS2ADownloadFrame(args);
			new Thread(new S2ADownloadProcessRunner(params)).start();
		}
	}
	
	public static void printHelp()
	{
		System.out.printf("Sentinel-2A Tile Downloader\nVersion %s.%s. Free software. GNU General Public License, version 3\n", PROG_VERSION, DATE_VERSION);
		System.out.println("Copyright (C) 2016 Igor Garkusha.\nUkraine, Dnipro (Dnipropetrovsk)\n\n");
		
		System.out.println("Start with GUI:");
		System.out.println("\ts2a_tile_downloader");
		System.out.println("Console parameters for Scientific Data Hub:");
		System.out.println("\ts2_tile_downloader \"<WorkDirectory> <ProductID> <ProductName> <UTM_TILE> <Username:Password> [band numbers: 1 - 13]\"");
		
		System.out.println("\nResolution Bands S2A MSI: 10 -- 2, 3, 4, 8");
        System.out.println("                          20 -- 5, 6, 7, 8b(13), 11, 12");
		System.out.println("                          60 -- 1, 9, 10");
		
		System.out.println("\nConsole parameters for Sentinel-2 on AWS cloud:");
		System.out.println("\ts2_tile_downloader \"<WorkDirectory> <AWS_Direct_Link>\"\n");
		System.out.println("\tBy default for AWS-mode, after the download process all bands,\n\texecutes additional processing (decompress, set projection,\n\tcheck and correction TOA spectral reflectance),\n\twith the exception of the command GDAL_MERGE!\n");
				
		System.out.println("Examples for Scientific Data Hub:");
		System.out.println("\ts2a_tile_downloader \"/home/username/processing/data 92643e16-9b9e-484c-ab5b-c8a9b4f1c772 S2A_OPER_PRD_MSIL1C_PDMC_20160104T193934_R064_V20160103T084718_20160103T084718.SAFE 36UXV Username:Password\"");
		System.out.println("\ts2a_tile_downloader C:\\Processing\\Data 92643e16-9b9e-484c-ab5b-c8a9b4f1c772 S2A_OPER_PRD_MSIL1C_PDMC_20160104T193934_R064_V20160103T084718_20160103T084718.SAFE 36UXV Username:Password 1 9 10");
		System.out.println("\ts2a_tile_downloader \"/home/username/processing/data 92643e16-9b9e-484c-ab5b-c8a9b4f1c772 S2A_OPER_PRD_MSIL1C_PDMC_20160104T193934_R064_V20160103T084718_20160103T084718.SAFE 36UXV Username:Password\"");
		
		System.out.println("\nExamples for Sentinel-2 on AWS cloud:");
		System.out.println("\ts2a_tile_downloader \"/Users/username/processing/Paris http://sentinel-s2-l1c.s3-website.eu-central-1.amazonaws.com/#tiles/31/U/DQ/2016/5/1/0/\"");
		
		System.out.println();
	}
}
