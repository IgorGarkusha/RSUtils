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

package modis_downloader;

import org.rsutils.*;
import org.rsutils.downloader.*;
import java.io.*;
import java.nio.file.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.filechooser.*;

public class ModisDownloadFrame extends ConsoleModisDownloadFrame implements Runnable
{
    public ModisDownloadFrame()
    {
		super(null);
		
		appPath = Utils.getApplicationName(this);
				
        frm = new JFrame();
        panel = new JPanel();
        
        cbProductName = new JComboBox<String>(products);
        
        int year  = Calendar.getInstance().get(Calendar.YEAR);
        int start_year = 2002;
        int count_years = (year-start_year)+1;
        String [] years = new String[count_years];
        for(int i=0; i<count_years; i++) years[i] = Integer.toString(start_year+i);
        
        String [] month = new String[12];
        for(int i=1; i<=12; i++) 
        {
			month[i-1] = "";
			if(i<10) month[i-1] = "0"; 
			month[i-1] += Integer.toString(i);
		}
        
        cbYear = new JComboBox<String>(years);
        
        cbFromMonth = new JComboBox<String>(month);
		cbToMonth = new JComboBox<String>(month);
        
        txtTiles = new JTextField(20);
        
                
        txtInfoArea = new JTextArea();
        scrollArea = new JScrollPane(txtInfoArea);

        txtInfoArea.setEditable(false);
        txtInfoArea.setColumns(20);
        txtInfoArea.setRows(5);
                
        pBar = new JProgressBar();
        pBar.setStringPainted(true);

		btnShowSatelliteCoverage = new JButton("Satellite Coverage...");

        btnDownload = new JButton("Download");
        
        btnSaveParams = new JButton("Save Parameters");
		btnLoadParams = new JButton("Load Parameters");
    
		btnExit = new JButton("Exit");
		btnAbout = new JButton("About");
		
		txtWorkDir = new JTextField(20);
		txtWorkDir.setEditable(false);
		
		btnSetWorkDir = new JButton("...");
    }
    
    public void InitDownloadVariable()
    {
		m_WorkDir = txtWorkDir.getText();
		m_ProductName = ((String)cbProductName.getSelectedItem()).split(" ")[0];
		m_Year = (String)cbYear.getSelectedItem();
		m_From_Month = (String)cbFromMonth.getSelectedItem();
		m_To_Month = (String)cbToMonth.getSelectedItem();
		
		m_tile_name = txtTiles.getText().split(" ");
		band_indexes = new int[m_tile_name.length];
		for(int i=4; i<band_indexes.length; i++) band_indexes[i-4] = i-4;
	}

    public void CreatePanel()
    {
		JLabel jLabelWorkDir = new JLabel("Work Directory:");
		jLabelWorkDir.setFont(new Font("Dialog", 0, 12));
		
		JLabel jLabelProdName = new JLabel("Product Name:");
		jLabelProdName.setFont(new Font("Dialog", 0, 12));
		
		JLabel jLabelYear = new JLabel("Year:");
		jLabelYear.setFont(new Font("Dialog", 0, 12));
		
		JLabel jLabelFromMonth = new JLabel("From Month:");
		jLabelFromMonth.setFont(new Font("Dialog", 0, 12));
		
		JLabel jLabelToMonth = new JLabel("To Month:");
		jLabelToMonth.setFont(new Font("Dialog", 0, 12));
				
		JLabel jLabelTiles = new JLabel("Tiles:");
		jLabelTiles.setFont(new Font("Dialog", 0, 12));
				
				
		Box hbox1 = Box.createHorizontalBox(); hbox1.add(jLabelWorkDir); hbox1.add(txtWorkDir); hbox1.add(btnSetWorkDir);
		Box hbox2 = Box.createHorizontalBox(); hbox2.add(jLabelProdName); hbox2.add(cbProductName);
		
		Box hbox3 = Box.createHorizontalBox(); 
		hbox3.add(jLabelYear); hbox3.add(cbYear); hbox3.add(Box.createHorizontalStrut(10));
		hbox3.add(jLabelFromMonth); hbox3.add(cbFromMonth); hbox3.add(Box.createHorizontalStrut(10)); 
		hbox3.add(jLabelToMonth); hbox3.add(cbToMonth); hbox3.add(Box.createHorizontalGlue());
		
		Box hbox4 = Box.createHorizontalBox(); hbox4.add(jLabelTiles); hbox4.add(txtTiles); hbox4.add(btnShowSatelliteCoverage);
				
		Box hbox5 = Box.createHorizontalBox();
		hbox5.add(pBar);
		
		Box hbox6 = Box.createHorizontalBox();
		hbox6.add(scrollArea);
		
		Box hbox7 = Box.createHorizontalBox();
		hbox7.add(btnSaveParams); hbox7.add(btnLoadParams); hbox7.add(Box.createHorizontalStrut(40)); hbox7.add(btnDownload); hbox7.add(Box.createHorizontalStrut(40));
		hbox7.add(btnExit); hbox7.add(Box.createHorizontalStrut(20)); hbox7.add(btnAbout);
				
		Box vbox = Box.createVerticalBox();
		vbox.add(hbox1);
        vbox.add(Box.createVerticalStrut(10));
        vbox.add(hbox2);
        vbox.add(Box.createVerticalStrut(10));
        vbox.add(hbox3);
        vbox.add(Box.createVerticalStrut(10));
        vbox.add(hbox4);
        vbox.add(Box.createVerticalStrut(10));
        vbox.add(hbox5);
        vbox.add(Box.createVerticalStrut(10));
        vbox.add(hbox6);
        vbox.add(Box.createVerticalStrut(10));
        vbox.add(hbox7);
                
        panel.add(vbox);
        frm.pack();
	}

	private void setParameter(int paramIndex, String str)
    {
        switch(paramIndex)
        {
            case 1: txtWorkDir.setText(str); break;
            case 2: cbProductName.setSelectedIndex(Integer.parseInt(str)); break;
            case 3: cbYear.setSelectedIndex(Integer.parseInt(str)); break;
            case 4: cbFromMonth.setSelectedIndex(Integer.parseInt(str)); break;
            case 5: cbToMonth.setSelectedIndex(Integer.parseInt(str)); break;
            case 6: txtTiles.setText(str);
        }
    }

    public void CreateListener()
    {
		btnShowSatelliteCoverage.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e)
			{
				String cmd = "java -jar ";
				cmd += Utils.extractDirName(appPath);
				cmd += File.separator;
				cmd += "SatelliteCoverage.jar";
				try{ Runtime.getRuntime().exec(cmd);
				}catch(Exception ex){}
			}
		});
		
		btnSetWorkDir.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e)
			{
				JFileChooser dirChooser = new JFileChooser();
				dirChooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY); 
				int res = dirChooser.showSaveDialog(frm);
				if(res == JFileChooser.APPROVE_OPTION) 
				{
					txtWorkDir.setText( dirChooser.getSelectedFile().getAbsolutePath() );
				}
			}
		});
		
		btnSaveParams.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e)
			{
				JFileChooser chooser = new JFileChooser();
				FileNameExtensionFilter filter = new FileNameExtensionFilter("Parameters Terra MODIS CFG", "modis.cfg");
				chooser.setFileFilter(filter);
				chooser.setCurrentDirectory(new File(curDir));
				int res = chooser.showSaveDialog(frm);
				if(res == JFileChooser.APPROVE_OPTION) 
				{
					String fileName = chooser.getSelectedFile().getAbsolutePath();
					
					String ext = fileName.substring(fileName.length()-10, fileName.length());
					if( ext.compareToIgnoreCase(".modis.cfg") != 0 ) fileName += ".modis.cfg";

					PrintStream fout = null;
					try
					{
						fout = new PrintStream(fileName);
						
						fout.println(txtWorkDir.getText());
						fout.println(cbProductName.getSelectedIndex());
						fout.println(cbYear.getSelectedIndex());
						fout.println(cbFromMonth.getSelectedIndex());
						fout.println(cbToMonth.getSelectedIndex());
						fout.println(txtTiles.getText());
						
						fout.close();
					} catch (FileNotFoundException ex) {}
				}
				
			}
		}
		);
		
		btnLoadParams.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e)
			{
				JFileChooser chooser = new JFileChooser();
				FileNameExtensionFilter filter = new FileNameExtensionFilter("Parameters Terra MODIS CFG", "cfg");
				chooser.setFileFilter(filter);
				chooser.setCurrentDirectory(new File(curDir));
				int res = chooser.showOpenDialog(frm);
				if(res == JFileChooser.APPROVE_OPTION) 
				{				
					String fileName = chooser.getSelectedFile().getAbsolutePath();

					Scanner fin = null;
					try
					{
						fin = new Scanner(new File(fileName));
						int i = 0;
						while(fin.hasNextLine())
						{
							String str = fin.nextLine();
							i++;
							setParameter(i, str);
						}
						fin.close();
					} catch(FileNotFoundException ex) {}
				}
			}
		}
		);
		
		
		btnDownload.addActionListener(new DownloadButtonLister(this, pBar, txtInfoArea));
		
		btnExit.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent e) {
				System.exit(0);
			} 
		});
		
		btnAbout.addActionListener(new AboutButtonLister(frm, "About...", ProgramTitle));
    }

    public void CreateFrame()
    {
        frm.setTitle(ProgramTitle);
        frm.setBounds(new Rectangle(800, 600));
        frm.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frm.add(panel);
        frm.setResizable(true);
        //frm.setVisible(true);
        frm.pack();
        frm.setResizable(false);
        frm.setVisible(true);
        frm.setLocationRelativeTo(null);
    }

    public void Msg(Object message)
    {
        JOptionPane.showMessageDialog(panel, message);
    }

	@Override
    public void run()
    {
        CreatePanel();
        CreateListener();
        CreateFrame();
    }
	
	public JButton getUI_btnDownload() { return btnDownload; }
	
	private String appPath = "";
    private JFrame frm;
    private JPanel panel;
    private JTextField txtWorkDir;
    private JTextField txtTiles;
    private JTextArea txtInfoArea;
    private JScrollPane scrollArea;
    private JProgressBar pBar;
    private JComboBox<String> cbProductName;
    private JComboBox<String> cbYear;
    private JComboBox<String> cbFromMonth;
    private JComboBox<String> cbToMonth;
    
    private JButton btnSetWorkDir;    
    private JButton btnSaveParams;
    private JButton btnLoadParams;
    private JButton btnDownload;
    private JButton btnExit;
    private JButton btnAbout;
    private JButton btnShowSatelliteCoverage;
        
    private String ProgramTitle = "Terra MODIS Product Downloader. Version " + 
								  modis_download.PROG_VERSION +"."+modis_download.DATE_VERSION;
    private String curDir = System.getProperty("user.dir");
    private String [] products = {
		"MOD09A1.005 Reflectance 500m Composites",
		"MOD09A1.006 Reflectance 500m Composites",
		"MOD09GQ.005 Reflectance 250m Daily",
		"MOD09GQ.006 Reflectance 250m Daily",
		"MOD09Q1.005 Reflectance 250m Composites",
		"MOD09Q1.006 Reflectance 250m Composites",
		"MOD11A1.004 Temperature and Emissivity 1km Daily",
		"MOD11A1.041 Temperature and Emissivity 1km Daily",
		"MOD11A1.005 Temperature and Emissivity 1km Daily",
		"MOD11A1.006 Temperature and Emissivity 1km Daily",
		"MOD11A2.004 Temperature and Emissivity 1km Composites",
		"MOD11A2.005 Temperature and Emissivity 1km Composites",
		"MOD11A2.006 Temperature and Emissivity 1km Composites",
		"MOD13A1.005 Vegetation Indices 500m Composites",
		"MOD13A1.006 Vegetation Indices 500m Composites",
		"MOD13A2.005 Vegetation Indices 1km Composites",
		"MOD13A2.006 Vegetation Indices 1km Composites",
		"MOD13A3.005 Vegetation Indices 1km Monthly",
		"MOD13A3.006 Vegetation Indices 1km Monthly",
		"MOD13Q1.005 Vegetation Indices 250m Composites",
		"MOD13Q1.006 Vegetation Indices 250m Composites",
		"MOD14A1.005 Thermal Anomalies and Fire 1km Daily",
		"MOD14A1.006 Thermal Anomalies and Fire 1km Daily",
		"MOD14A2.005 Thermal Anomalies and Fire 1km Composites",
		"MOD14A2.006 Thermal Anomalies and Fire 1km Composites",
		"MOD15A2.005 Leaf Area Index and Fractional Photosynthetically Active Radiation 1km Composites",
		"MOD15A2H.006 Leaf Area Index and Fractional Photosynthetically Active Radiation 500m Composites",
		"MOD17A2.005 Gross Primary Productivity 1km Composites",
		"MOD17A2.055 Gross Primary Productivity 1km Composites",
		"MOD17A2H.006 Gross Primary Productivity 500m Composites"
	};
       
    private class DownloadButtonLister implements ActionListener
    {
		public DownloadButtonLister(ModisDownloadFrame mainFrame, JProgressBar prBar, JTextArea txtInfo)
		{
			m_mainFrame = mainFrame;
			m_prBar = prBar;
			m_txtInfo = txtInfo;
		}
		
		public void actionPerformed(ActionEvent e)
		{
			m_mainFrame.InitDownloadVariable();
			new Thread(new ModisDownloadProcessRunner(m_mainFrame, m_prBar, m_txtInfo)).start();
		}
		
		private ModisDownloadFrame m_mainFrame = null;
		private JProgressBar m_prBar = null;
		private JTextArea m_txtInfo = null;
	}
	
	private class AboutButtonLister implements ActionListener
    {
		public AboutButtonLister(JFrame parent, String title, String programName)
		{
			m_parent = parent;
			m_title = title;
			m_programName = programName;
		}
		
		public void actionPerformed(ActionEvent e)
		{
			AboutDialog about = new AboutDialog(m_parent, m_title, m_programName);
			about.setVisible(true);
		}
		
		private JFrame m_parent = null;
		private String m_title = null;
		private String m_programName = "";
	}
}
