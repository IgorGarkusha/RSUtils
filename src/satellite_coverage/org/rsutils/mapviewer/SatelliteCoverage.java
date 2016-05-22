/*
 * Project: Satellite Coverage
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
 * *********************************************************************
 * 
 * Satellite Coverage based on JMapViewer (License GPL):
 * 
 * Copyright (C) 2007, Tim Haussmann
 * Copyright (C) 2008-2012, Jan Peter Stotz
 * Copyright (C) 2009-2015, Dirk Stöcker
 * Copyright (C) 2009, Stefan Zeller
 * Copyright (C) 2009, Karl Guggisberg
 * Copyright (C) 2009, Dave Hansen
 * Copyright (C) 2010-2011, Ian Dees
 * Copyright (C) 2010-2011, Michael Vigovsky
 * Copyright (C) 2011-2015, Paul Hartmann
 * Copyright (C) 2011-2014, Gleb Smirnoff
 * Copyright (C) 2011-2015, Vincent Privat
 * Copyright (C) 2011, Jason Huntley
 * Copyright (C) 2012, Simon Legner
 * Copyright (C) 2012, Teemu Koskinen
 * Copyright (C) 2012, Jiri Klement
 * Copyright (C) 2013, Matt Hoover
 * Copyright (C) 2013, Alexei Kasatkin
 * Copyright (C) 2013, Galo Higueras
 * 
 * JMapViewer is a Java Swing component for integrating OSM maps 
 * in to your Java application
 * 
*/

package org.rsutils.mapviewer;

import org.openstreetmap.gui.jmapviewer.*;
import org.openstreetmap.gui.jmapviewer.interfaces.*;
import org.openstreetmap.gui.jmapviewer.events.*;

import java.awt.BorderLayout;
import java.awt.Rectangle;
import java.awt.Cursor;
import java.awt.Point;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.awt.event.ComponentAdapter;
import java.awt.FlowLayout;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseListener;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Color;
import java.awt.Stroke;
import java.awt.BasicStroke;

import javax.swing.SwingUtilities;
import javax.swing.JComponent;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JFileChooser;
import javax.swing.filechooser.FileNameExtensionFilter;

import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;
import java.util.NoSuchElementException;
import java.util.Locale;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.InputStream;
import java.io.PrintStream;

public class SatelliteCoverage extends Thread
{
	public static final String PROG_VERSION = "1";
	public static final String DATE_VERSION = "22.05.2016";
	public String ProgramTitle = "Coverage Scheme of Satellites. Version " + PROG_VERSION +"."+DATE_VERSION;
	
	public static void main(String[] args)
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
        
		SwingUtilities.invokeLater(new SatelliteCoverage());
	}
	
	public SatelliteCoverage()
	{
		frm = new JFrame();
		frm.setTitle(ProgramTitle);
		frm.setLayout(new BorderLayout());
		panel = new JPanel(new BorderLayout());
		topPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
		statusBar = new JPanel(new FlowLayout(FlowLayout.LEFT));
		status = new JLabel("");
		btnAbout = new JButton("About...");
		btnCreateAOI = new JButton("Create AOI");
		btnSaveAOI = new JButton("Save AOI...");
		cbSatellite = new JComboBox<String>(new String[]{
						"WRS-2 Path/Row boundaries (Landsat-8, Landsat-7)", 
						"UTM/WGS-84 Tiles Grid (Sentinel-2A Level-1C)", 
						"MODIS Sinusoidal Tiles (Terra MODIS)"});
	}
	
	@Override public void run()
	{
		topPanel.add(new JLabel("Coverage scheme: "));
		topPanel.add(cbSatellite);
		topPanel.add(btnCreateAOI);
		topPanel.add(btnSaveAOI);
		topPanel.add(btnAbout);
		
		statusBar.add(status);
		
		JMapViewer mapViewer = new JMapViewer();
		
		panel.add(topPanel, BorderLayout.NORTH);
		panel.add(mapViewer, BorderLayout.CENTER);
		panel.add(statusBar, BorderLayout.SOUTH);
		
		frm.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frm.setBounds(new Rectangle(800, 600));
		frm.add(panel, BorderLayout.CENTER);
		frm.setVisible(true);
		
		mv_event = new JMapViewerDoEvent();
		mapViewer.addJMVListener(mv_event);
		mapViewer.addComponentListener(mv_event);
		mapViewer.addMouseMotionListener(mv_event);
		mapViewer.addMouseListener(mv_event);
	
		frm.pack();
		frm.setLocationRelativeTo(null);
	
		btnCreateAOI.addActionListener(new ActionListener()
		{
			public void actionPerformed(ActionEvent e)
			{
				SatelliteCoverage.flagStartCreateAOI = true;
				
				cbSatellite.setEnabled(false);
				btnCreateAOI.setEnabled(false);
				btnSaveAOI.setEnabled(false);
				btnAbout.setEnabled(false);
			}
		}
		);
		
		btnSaveAOI.addActionListener(new ActionListener()
		{
			public void actionPerformed(ActionEvent e)
			{
				JFileChooser chooser = new JFileChooser();
				FileNameExtensionFilter filter = new FileNameExtensionFilter("Keyhole Markup Language (*.kml)", "kml");
				chooser.setFileFilter(filter);
				chooser.setCurrentDirectory(new File( System.getProperty("user.dir") ));
				int res = chooser.showSaveDialog(frm);
				if(res == JFileChooser.APPROVE_OPTION) 
				{
					String fileName = chooser.getSelectedFile().getAbsolutePath();
					String ext = fileName.substring(fileName.length()-4, fileName.length());
					if( ext.compareToIgnoreCase(".kml") != 0 ) fileName += ".kml";
					saveAOItoKML(fileName);
				}
			}
			
			public void saveAOItoKML(String fileName)
			{
				String  coords = String.format(new Locale("en"), "%f,%f %f,%f %f,%f %f,%f %f,%f", 
											aoiLon1, aoiLat1,
											aoiLon2, aoiLat1,
											aoiLon2, aoiLat2,
											aoiLon1, aoiLat2,
											aoiLon1, aoiLat1
											);
				
				String  kmlText =  "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n";
						kmlText += "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n";
						kmlText += "<Document id=\"root_doc\">\n";
						kmlText += "<Schema name=\"area\" id=\"area\">\n";
						kmlText += "<SimpleField name=\"id\" type=\"int\"></SimpleField>\n";
						kmlText += "</Schema>\n";
						kmlText += "<Folder><name>area</name>\n";
						kmlText += "<Placemark>\n";
						kmlText += "<Style><LineStyle><color>ff0000ff</color></LineStyle><PolyStyle><fill>0</fill></PolyStyle></Style>\n";
						kmlText += "<ExtendedData><SchemaData schemaUrl=\"#area\">\n";
						kmlText += "<SimpleData name=\"id\">1</SimpleData>\n";
						kmlText += "</SchemaData></ExtendedData>\n";
						kmlText += "<Polygon><altitudeMode>relativeToGround</altitudeMode><outerBoundaryIs><LinearRing><altitudeMode>relativeToGround</altitudeMode>\n";
						kmlText += "<coordinates>";
						kmlText += coords;
						kmlText += "</coordinates></LinearRing></outerBoundaryIs></Polygon>\n";
						kmlText += "</Placemark>\n";
						kmlText += "</Folder>\n";
						kmlText += "</Document></kml>\n";

				PrintStream fout = null;
				try
				{
					fout = new PrintStream(fileName);
					fout.println(kmlText);
					fout.close();
				} catch (FileNotFoundException ex) {}
			}
		}
		);
		
		btnAbout.addActionListener(new ActionListener()
		{
			public void actionPerformed(ActionEvent e)
			{
				AboutDialog about = new AboutDialog(frm, "About...", ProgramTitle);
				about.setVisible(true);
			}
		}
		);		
	}

	private JFrame frm = null;
	private JPanel panel = null;
	private JMapViewer mapViewer = null;
	private JMapViewerDoEvent mv_event = null;
	private JPanel topPanel = null;
	private JPanel statusBar = null;
	private JButton btnAbout = null;
	private JButton btnCreateAOI = null;
	private JButton btnSaveAOI = null;
	private JLabel status = null;
	private JComboBox cbSatellite = null;
	private static boolean flagStartCreateAOI = false;
	private double aoiLat1 = 0, aoiLon1 = 0, aoiLat2 = 0, aoiLon2 = 0;
	private int aoiY1 = 0, aoiX1 = 0;
	
	public class XYFileParts
	{
		public int fromX;
		public int toX;
		public Scanner fin = null;
	}
	
	private class ComboBoxSatelliteScheme implements ActionListener
	{
		private JMapViewerDoEvent m_me = null;
		
		public ComboBoxSatelliteScheme(JMapViewerDoEvent me) { m_me = me; }
		
		public void actionPerformed(ActionEvent e)
		{
			m_me.setCurrentSchemeIndex( ((JComboBox)(e.getSource())).getSelectedIndex() );
		}
	}
	
	private class JMapViewerDoEvent extends ComponentAdapter implements JMapViewerEventListener, MouseMotionListener, MouseListener
	{
		private int m_x1 = 1, m_y1 = 1, m_x2 = 0, m_y2 = 0;
		private int mode = 0;
		private XYFileParts [] fileParts = null;
		private JLabel m_status = null;
		private JComboBox m_cbSatellite = null;
		private int currentSchemeIndex = 0;
		private int MAX_FILES_COUNT = 0;
		private String fileNameSchemePrefix = "";
		private int zoomBorder = 6;
		private Cursor oldCursor = null;
		private boolean startFlagSelection = false;
		private boolean mapViewerStarted = false;

		public JMapViewerDoEvent()
		{
			initSatelliteScheme();
			cbSatellite.addActionListener(new ComboBoxSatelliteScheme(this));
		}
		
		private void initSatelliteScheme()
		{
			int delta = 0;
			
			switch(currentSchemeIndex)
			{
				// WRS-2 (Landsat)
				case 0:
					mode = 2; // Landsat
					MAX_FILES_COUNT = 6;
					delta = 60;
					zoomBorder = 6;
					fileNameSchemePrefix = "data/wrs2";
					break;
					
				// UTM Tiles (Sentinel-2A)
				case 1:
					mode = 1; // Terra MODIS, S2A
					MAX_FILES_COUNT = 18;
					delta = 20;
					zoomBorder = 6;
					fileNameSchemePrefix = "data/s2a";
					break;
					
				// Terra MODIS Tiles
				case 2:
					mode = 1; // Terra MODIS, S2A
					MAX_FILES_COUNT = 4;
					delta = 90;
					zoomBorder = 5;
					fileNameSchemePrefix = "data/mod";
					break;
			}
			
			if(null != fileParts) close();
			
			fileParts = new XYFileParts[MAX_FILES_COUNT];
			for(int i=0; i<fileParts.length; i++) fileParts[i] = new XYFileParts();
					
			int startx = -180;
			int endx = 180;
			
			int j = 0;
			for(int i=startx; i<=(endx-delta); i+=delta)
			{
				String fromX = Integer.toString(i);
				String toX   = Integer.toString(i+delta);
				fileParts[j].fromX = i;
				fileParts[j].toX   = i+delta;
				doOpenData(j, fromX, toX);
				j++;
			}
			
			startLoadCoverageData();
		}
		
		private void rewindCoverageFile(int index)
		{
			fileParts[index].fin.close();
			String fromX = Integer.toString(fileParts[index].fromX);
			String toX 	 = Integer.toString(fileParts[index].toX);	
			doOpenData(index, fromX, toX);
		}
		
		private void doOpenData(int index, String fromX, String toX)
		{
			InputStream is = this.getClass().getClassLoader().getResourceAsStream(fileNameSchemePrefix + fromX + "_" + toX + ".poly");
			fileParts[index].fin = new Scanner(is);
		}
		
		private void close()
		{
			for(int i=0; i<fileParts.length; i++) { fileParts[i].fin.close(); fileParts[i] = null; }
			fileParts = null;
		}
		
		private void setCurrentSchemeIndex(int csi)
		{
			currentSchemeIndex = csi;
			initSatelliteScheme();
		}
		
		private void startLoadCoverageData()
		{
			if(mapViewer != null)
			{
				double lat1 = mapViewer.getPosition(new Point(m_x1, m_y1)).getLat();
				double lon1 = mapViewer.getPosition(new Point(m_x1, m_y1)).getLon();
				double lat2 = mapViewer.getPosition(new Point(m_x2, m_y2)).getLat();
				double lon2 = mapViewer.getPosition(new Point(m_x2, m_y2)).getLon();
				if(mapViewer.getZoom() >= zoomBorder)
				{
					loadCoverageData(mode, lon1, lat1, lon2, lat2);
				}
				else mapViewer.removeAllMapPolygons();
			}
		}
		
		private void loadCoverageData(int mode, double x1, double y1, double x2, double y2)
		{
			mapViewer.removeAllMapPolygons();
			
			for(int i=0; i<fileParts.length; i++) 
			{ 
				if( ((fileParts[i].fromX<=x1)&&(fileParts[i].toX>=x1)) || 
					((fileParts[i].fromX<=x2)&&(fileParts[i].toX>=x2)) || 
					((fileParts[i].fromX>x1)&&(fileParts[i].toX<x2))
				  )
				{
					if(fileParts[i].fin != null)
					{
						rewindCoverageFile(i);
						
						while(fileParts[i].fin.hasNextLine())
						{
							List<Coordinate>coords = new ArrayList<Coordinate>();
							try
							{
								String [] str = fileParts[i].fin.nextLine().split(" ");
								String polyName = str[0];
								int start = 0;
								if(mode == 1) start = 1;
								if(mode == 2) { polyName += "/"; polyName += str[1]; start = 2; }
								for(int j=start; j<str.length-1; j+=2)
								{
									double y = java.lang.Double.parseDouble(str[j]);
									double x = java.lang.Double.parseDouble(str[j+1]);									
									coords.add(new Coordinate(x, y));
								}
								if(coords.size()>0)
								{
									MapPolygon poly = new MapPolygonImpl(polyName, coords);
									mapViewer.addMapPolygon(poly);
								}
							}catch(NoSuchElementException e)
							{
								e.printStackTrace();
							}
						}
					}
					else System.out.println("Error open data file!");
				}
			}
		}

		/////////////////////////////
		// Events processing
		/////////////////////////////

		public void processCommand(JMVCommandEvent command)
		{
			if (command.getCommand().equals(JMVCommandEvent.COMMAND.ZOOM) ||
                command.getCommand().equals(JMVCommandEvent.COMMAND.MOVE))
			{
				startLoadCoverageData();
			}
		}
		
		public void componentResized(ComponentEvent e)
		{
			mapViewer = (JMapViewer)e.getComponent();			
			m_x2 = mapViewer.getWidth();
			m_y2 = mapViewer.getHeight();
			mapViewerStarted = true;
		}
		
		public void	mouseDragged(MouseEvent e)
		{
			if(startFlagSelection)
			{
				mapViewer.updateUI();
				
				int x = e.getX();
				int y = e.getY();
				
				Stroke dash = new BasicStroke(2, BasicStroke.CAP_BUTT, BasicStroke.JOIN_BEVEL, 0, new float[]{9}, 0);
				Graphics2D gr = (Graphics2D)(mapViewer.getGraphics().create());
				gr.setStroke(dash);
				gr.setColor(new Color(0, 0, 255));
				gr.drawRect(aoiX1, aoiY1, x-aoiX1, y-aoiY1);				
			}
		}

		public void mouseMoved(MouseEvent e)
		{
			if(mapViewerStarted)
			{
				int x = e.getX();
				int y = e.getY();
				double lat = mapViewer.getPosition(new Point(x, y)).getLat();
				double lon = mapViewer.getPosition(new Point(x, y)).getLon();
				status.setText( String.format("Lat (Y): %.4f\u00B0 , Long (X): %.4f\u00B0      ", lat, lon) );
			}
		}
		
		public void mousePressed(MouseEvent e)
		{
			if(SatelliteCoverage.flagStartCreateAOI)
			{
				oldCursor = Cursor.getDefaultCursor();
				
				mapViewer.setCursor(new Cursor(Cursor.CROSSHAIR_CURSOR));
				
				startFlagSelection = true;
				
				int x = e.getX();
				int y = e.getY();
				aoiX1 = x; aoiY1 = y;
				aoiLat1 = mapViewer.getPosition(new Point(x, y)).getLat();
				aoiLon1 = mapViewer.getPosition(new Point(x, y)).getLon();
			}
		}
		
		public void mouseReleased(MouseEvent e)
		{
			if(SatelliteCoverage.flagStartCreateAOI)
			{
				int x = e.getX();
				int y = e.getY();
				aoiLat2 = mapViewer.getPosition(new Point(x, y)).getLat();
				aoiLon2 = mapViewer.getPosition(new Point(x, y)).getLon();
				
				MapRectangle rectAOI = new MapRectangleImpl( new Coordinate(aoiLat1, aoiLon1), new Coordinate(aoiLat2, aoiLon2) );
				
				mapViewer.removeAllMapRectangles();
				mapViewer.addMapRectangle(rectAOI);
				
				startFlagSelection = false;
				SatelliteCoverage.flagStartCreateAOI = false;
				mapViewer.setCursor(oldCursor);
				
				cbSatellite.setEnabled(true);
				btnCreateAOI.setEnabled(true);
				btnSaveAOI.setEnabled(true);
				btnAbout.setEnabled(true);
			}
		}
				
		public void mouseClicked(MouseEvent e){}		
		public void mouseEntered(MouseEvent e){}
		public void mouseExited(MouseEvent e){}
	}
}
