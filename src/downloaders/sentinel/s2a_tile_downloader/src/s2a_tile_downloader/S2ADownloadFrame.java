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

import org.rsutils.*;
import org.rsutils.downloader.*;
import java.io.*;
import java.net.*;
import java.nio.file.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.filechooser.*;

public class S2ADownloadFrame extends ConsoleS2ADownloadFrame implements Runnable
{
    public S2ADownloadFrame()
    {
		super(null);
		
		appPath = Utils.getApplicationName(this);
		
        frm = new JFrame();
        panel = new JPanel();

		buttonGroup = new ButtonGroup();
		buttonGroup.add(rbSCIHUB);
		buttonGroup.add(rbAWS);
		
		txtAWSLink = new JTextField(50);
        txtProductID = new JTextField(50);
        txtProductName = new JTextField(50);
        txtUTM_TILE = new JTextField(7);
        txtUsername = new JTextField(20);
        txtPassword = new JPasswordField(10);
        
        txtInfoArea = new JTextArea();
        scrollArea = new JScrollPane(txtInfoArea);

        txtInfoArea.setEditable(false);
        txtInfoArea.setColumns(20);
        txtInfoArea.setRows(5);
                
        pBar = new JProgressBar();
        pBar.setStringPainted(true);

		btnShowSatelliteCoverage = new JButton("Satellite Coverage...");

		btnShowAWSS2Browser = new JButton("Sentinel-2 on AWS in Browser...");

        btnDownload = new JButton("Download");
        
        btnSaveParams = new JButton("Save Parameters");
		btnLoadParams = new JButton("Load Parameters");
    
		btnExit = new JButton("Exit");
		btnAbout = new JButton("About");
		
		cbBands = new JComboBox<String>(new String[]{"all", "10 m - 2, 3, 4, 8", "20 m - 5, 6, 7, 8b(13), 11, 12", "60 m - 1, 9, 10"});
		
		txtWorkDir = new JTextField(20);
		txtWorkDir.setEditable(false);
		
		btnSetWorkDir = new JButton("...");
				
		cbDecompress = new JCheckBox("Decompress to GeoTIFF and set projection");
		cbDecompress.setSelected(false);		
		cbCheckReflectance = new JCheckBox("Check and correction TOA spectral reflectance (with default method 1 for the program check_reflectance!)");
		cbCheckReflectance.setSelected(false);
		cbCheckReflectance.setEnabled(false);
		cbGdalMerge = new JCheckBox("Execute GDAL_MERGE (ignore, if downloading all bands!)");
		cbGdalMerge.setSelected(false);
		cbGdalMerge.setEnabled(false);
    }
    
    public void InitDownloadVariable()
    {
		m_WorkDir = txtWorkDir.getText();
		
		if( false == flAWS )
		{
			m_ProductID = txtProductID.getText();
			m_ProductName = txtProductName.getText();
			m_UTMTILE = txtUTM_TILE.getText();
			m_UserNameAndPasword = this.txtUsername.getText()+":"+new String(this.txtPassword.getPassword());
		}
		else
		{
			m_ProductID = "";
			m_ProductName = txtAWSLink.getText(); // THIS IS AWS LINK!
			m_UTMTILE = "";
			m_UserNameAndPasword = "";
		}
		
		switch(cbBands.getSelectedIndex())
		{
			case 0: // all bands
					band_indexes = null;
					break;
			case 1: // 10 m - 2, 3, 4, 8
					band_indexes = new int[4];
					band_indexes[0] = 2;
					band_indexes[1] = 3;
					band_indexes[2] = 4;
					band_indexes[3] = 8;
					break;
			case 2: // 20 m - 5, 6, 7, 8b(13), 11, 12
					band_indexes = new int[6];
					band_indexes[0] = 5;
					band_indexes[1] = 6;
					band_indexes[2] = 7;
					band_indexes[3] = 11;
					band_indexes[4] = 12;
					band_indexes[5] = 13;
					break;
			case 3: // 60 m - 1, 9, 10
					band_indexes = new int[3];
					band_indexes[0] = 1;
					band_indexes[1] = 9;
					band_indexes[2] = 10;
		}
	}

    public void CreatePanel()
    {
		JLabel jLabelWorkDir = new JLabel("Work Directory:");
		jLabelWorkDir.setFont(new Font("Dialog", 0, 12));
		
		JLabel jLabelAWSLink = new JLabel("AWS Direct Link:");
		jLabelAWSLink.setFont(new Font("Dialog", 0, 12));
		
		JLabel jLabelProdID = new JLabel("Product ID:");
		jLabelProdID.setFont(new Font("Dialog", 0, 12));
		
		JLabel jLabelProdName = new JLabel("Product Name:");
		jLabelProdName.setFont(new Font("Dialog", 0, 12));
		
		JLabel jLabelUTMTile = new JLabel("UTM Tile:");
		jLabelUTMTile.setFont(new Font("Dialog", 0, 12));
		
		JLabel jLabelUsername = new JLabel("Username:");
		jLabelUsername.setFont(new Font("Dialog", 0, 12));
		
		JLabel jLabelPassword = new JLabel("Password:");
		jLabelPassword.setFont(new Font("Dialog", 0, 12));
		
		JLabel jLabelBands = new JLabel("Download bands:");
		jLabelBands.setFont(new Font("Dialog", 0, 12));
		
		JLabel jLabelPostProcessing = new JLabel("Post downloading process:");
		jLabelPostProcessing.setFont(new Font("Dialog", 0, 12));
		
		
		Box hbox11 = Box.createHorizontalBox(); hbox11.add(rbSCIHUB); hbox11.add(Box.createHorizontalStrut(10)); hbox11.add(rbAWS);
		
		Box hbox1 = Box.createHorizontalBox(); hbox1.add(jLabelWorkDir); hbox1.add(txtWorkDir); hbox1.add(btnSetWorkDir);
		
		Box hbox10 = Box.createHorizontalBox(); hbox10.add(jLabelAWSLink); hbox10.add(txtAWSLink); hbox10.add(btnShowAWSS2Browser);
		
		Box hbox2 = Box.createHorizontalBox(); hbox2.add(jLabelProdID); hbox2.add(txtProductID);
		Box hbox3 = Box.createHorizontalBox(); hbox3.add(jLabelProdName); hbox3.add(txtProductName);
		Box hbox4 = Box.createHorizontalBox(); hbox4.add(jLabelUTMTile); hbox4.add(txtUTM_TILE); hbox4.add(btnShowSatelliteCoverage);
		
		Box hbox5 = Box.createHorizontalBox(); hbox5.add(jLabelBands); hbox5.add(cbBands); hbox5.add(Box.createHorizontalGlue());
		
		Box hbox6 = Box.createHorizontalBox(); 
		hbox6.add(jLabelUsername); hbox6.add(txtUsername); hbox6.add(Box.createHorizontalStrut(10)); hbox6.add(jLabelPassword); hbox6.add(txtPassword);
		
		Box hbox12 = Box.createHorizontalBox(); hbox12.add(jLabelPostProcessing); hbox12.add(Box.createHorizontalGlue());
		Box hbox13 = Box.createHorizontalBox(); hbox13.add(cbDecompress); hbox13.add(Box.createHorizontalGlue());
		Box hbox14 = Box.createHorizontalBox(); hbox14.add(cbCheckReflectance); hbox14.add(Box.createHorizontalGlue());
		Box hbox15 = Box.createHorizontalBox(); hbox15.add(cbGdalMerge); hbox15.add(Box.createHorizontalGlue());
		
		Box hbox7 = Box.createHorizontalBox();
		hbox7.add(pBar);
		
		Box hbox8 = Box.createHorizontalBox();
		hbox8.add(scrollArea);
		
		Box hbox9 = Box.createHorizontalBox();
		hbox9.add(btnSaveParams); hbox9.add(btnLoadParams); hbox9.add(Box.createHorizontalStrut(40)); hbox9.add(btnDownload); hbox9.add(Box.createHorizontalStrut(40));
		hbox9.add(btnExit); hbox9.add(Box.createHorizontalStrut(20)); hbox9.add(btnAbout);
				
		Box vbox = Box.createVerticalBox();
		
		vbox.add(hbox11);
		vbox.add(Box.createVerticalStrut(10));
        vbox.add(hbox1);
        vbox.add(Box.createVerticalStrut(10));
        vbox.add(hbox10);
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
        vbox.add(hbox12);
        vbox.add(Box.createVerticalStrut(10));
        vbox.add(hbox13);
        vbox.add(Box.createVerticalStrut(10));
        vbox.add(hbox14);
        vbox.add(Box.createVerticalStrut(10));
        vbox.add(hbox15);
        
        vbox.add(Box.createVerticalStrut(10));
        vbox.add(hbox7);
        vbox.add(Box.createVerticalStrut(10));
        vbox.add(hbox8);
        vbox.add(Box.createVerticalStrut(10));
        vbox.add(hbox9);
        
        panel.add(vbox);
        frm.pack();
        
        controlStateComponents(false);
	}
	
	private void setParameter(int paramIndex, String str)
    {
        switch(paramIndex)
        {
            case 1: txtWorkDir.setText(str); break;
            case 2: txtProductID.setText(str); break;
            case 3: txtProductName.setText(str); break;
            case 4: txtUsername.setText(str); break;
            case 5: txtUTM_TILE.setText(str); break;
            case 6: cbBands.setSelectedIndex(Integer.parseInt(str)); break;
        }
    }
    
    private void setAWSParameter(int paramIndex, String str)
    {
		switch(paramIndex)
        {
            case 1: txtWorkDir.setText(str); break;
            case 2: txtAWSLink.setText(str); break;
            case 3: if( Integer.parseInt(str.split(" ")[0]) == 1 ) cbDecompress.setSelected(true);
					else cbDecompress.setSelected(false);
					if( Integer.parseInt(str.split(" ")[1]) == 1 ) cbCheckReflectance.setSelected(true);
					else cbCheckReflectance.setSelected(false);
					if( Integer.parseInt(str.split(" ")[2]) == 1 ) cbGdalMerge.setSelected(true);
					else cbGdalMerge.setSelected(false);
					break;
            case 4: cbBands.setSelectedIndex(Integer.parseInt(str)); break;
        }
	}

	private void controlStateComponents(boolean state)
	{
		flAWS = state;
		
		txtAWSLink.setEnabled(flAWS);
		btnShowAWSS2Browser.setEnabled(flAWS);
		cbDecompress.setSelected(flAWS);
		cbDecompress.setEnabled(flAWS);
        //cbCheckReflectance.setEnabled(flAWS);
        //cbGdalMerge.setEnabled(flAWS);
        
        txtProductID.setEnabled(!flAWS);
        txtProductName.setEnabled(!flAWS);
        txtUTM_TILE.setEnabled(!flAWS);
        txtUsername.setEnabled(!flAWS);
        txtPassword.setEnabled(!flAWS);
	}

	private boolean getAWSConfigFile(String fileName)
	{
		boolean res = false;
		Scanner fin = null;
		try
		{
			fin = new Scanner(new File(fileName));
			int countLines = 0;
			while(fin.hasNextLine())
			{
				String str = fin.nextLine();
				countLines++;
			}			
			if( 4 == countLines ) res = true;
		}
		catch(FileNotFoundException ex)
		{
		}
		finally
		{
			if( null != fin ) { fin.close(); fin = null; }
		}
		
		return res;
	}

	public boolean isAWSMode()
	{
		return flAWS;
	}

	public boolean isDecompress()
	{
		return cbDecompress.isSelected();
	}
	
	public boolean isCheckReflectance()
	{
		return cbCheckReflectance.isSelected();
	}
	
	public boolean isGdalMerge()
	{
		return cbGdalMerge.isSelected();
	}
	
	public void selectCheckReflectance(boolean state)
	{
		if( false == state ) cbCheckReflectance.setSelected(state);
		cbCheckReflectance.setEnabled(state);
	}
	
	public void selectGdalMerge(boolean state)
	{
		if( false == state ) cbGdalMerge.setSelected(state);
		cbGdalMerge.setEnabled(state);
	}

    public void CreateListener()
    {
		rbSCIHUB.addChangeListener(new RadioButtonListener(this, false));
		rbAWS.addChangeListener(new RadioButtonListener(this, true));
		
		cbDecompress.addChangeListener(new CheckBoxListener(this));
		cbCheckReflectance.addChangeListener(new CheckBoxListener(this));
		cbGdalMerge.addChangeListener(new CheckBoxListener(this));
		
		btnShowAWSS2Browser.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e)
			{
				String search_page = "file://" + Utils.extractDirName(appPath) + "/html/s2_aws_search_page.html";
				Desktop desktop = Desktop.isDesktopSupported()?Desktop.getDesktop():null;
				if( (null != desktop) && (desktop.isSupported(Desktop.Action.BROWSE)) )
				{
					try{ 
						desktop.browse(new URL(search_page).toURI()); }
					catch(Exception ex){}
				}
				else Utils.openWebPage(search_page);
			}
		});
		
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
				FileNameExtensionFilter filter = new FileNameExtensionFilter("Parameters S2A CFG", "s2a.cfg");
				chooser.setFileFilter(filter);
				chooser.setCurrentDirectory(new File(curDir));
				int res = chooser.showSaveDialog(frm);
				if(res == JFileChooser.APPROVE_OPTION) 
				{
					String fileName = chooser.getSelectedFile().getAbsolutePath();
					
					String ext = fileName.substring(fileName.length()-8, fileName.length());
					if( ext.compareToIgnoreCase(".s2a.cfg") != 0 ) fileName += ".s2a.cfg";
										
					PrintStream fout = null;
					try
					{
						fout = new PrintStream(fileName);
						
						fout.println(txtWorkDir.getText());
						
						if(false == flAWS)
						{
							fout.println(txtProductID.getText());
							fout.println(txtProductName.getText());
							fout.println(txtUsername.getText());
							fout.println(txtUTM_TILE.getText());
						}
						else 
						{
							fout.println(txtAWSLink.getText());
							
							String tmpStr = "";
														
							if(cbDecompress.isSelected()) tmpStr += "1 ";
							else tmpStr += "0 ";
														
							if(cbCheckReflectance.isSelected()) tmpStr += "1 ";
							else tmpStr += "0 ";
							
							if(cbGdalMerge.isSelected()) tmpStr += "1";
							else tmpStr += "0";
							
							fout.println(tmpStr);
						}
						
						fout.println(cbBands.getSelectedIndex());
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
				FileNameExtensionFilter filter = new FileNameExtensionFilter("Parameters S2A CFG", "cfg");
				chooser.setFileFilter(filter);
				chooser.setCurrentDirectory(new File(curDir));
				int res = chooser.showOpenDialog(frm);
				if(res == JFileChooser.APPROVE_OPTION) 
				{				
					String fileName = chooser.getSelectedFile().getAbsolutePath();

					if( getAWSConfigFile(fileName) ) rbAWS.setSelected(true);
					//controlStateComponents(true);
					else rbSCIHUB.setSelected(true);
					//controlStateComponents(false);

					Scanner fin = null;
					try
					{
						fin = new Scanner(new File(fileName));
						int i = 0;
						while(fin.hasNextLine())
						{
							String str = fin.nextLine();
							i++;
							if(false == flAWS) setParameter(i, str);
							else setAWSParameter(i, str);
						}
						fin.close();
					} catch(FileNotFoundException ex) {}
				}
			}
		}
		);
		
		
		btnDownload.addActionListener(new DownloadButtonListener(this, pBar, txtInfoArea));
		
		btnExit.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent e) {
				System.exit(0);
			} 
		});
		
		btnAbout.addActionListener(new AboutButtonListener(frm, "About...", ProgramTitle));
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
    private JTextField txtAWSLink;
    private JTextField txtWorkDir;
    private JTextField txtProductID, txtProductName, txtUTM_TILE, txtUsername;
    private JPasswordField txtPassword;
    private JTextArea txtInfoArea;
    private JScrollPane scrollArea;
    private JProgressBar pBar;
    private JComboBox<String> cbBands;
    private ButtonGroup buttonGroup;
    private JRadioButton rbSCIHUB = new JRadioButton("Scientific Data Hub", true);
    private JRadioButton rbAWS = new JRadioButton("Sentinel-2 on AWS cloud", false);

    private JCheckBox cbDecompress;
    private JCheckBox cbCheckReflectance;
    private JCheckBox cbGdalMerge;
    
    private JButton btnSetWorkDir;
    private JButton btnSaveParams;
    private JButton btnLoadParams;
    private JButton btnDownload;
    private JButton btnExit;
    private JButton btnAbout;
    private JButton btnShowSatelliteCoverage;
    private JButton btnShowAWSS2Browser;
    
    private String ProgramTitle = "Sentinel-2A Tile Downloader. Version " + 
								  s2_tile_download.PROG_VERSION +"."+s2_tile_download.DATE_VERSION;
								  
    private String curDir = System.getProperty("user.dir");
       
    private class DownloadButtonListener implements ActionListener
    {
		public DownloadButtonListener(S2ADownloadFrame mainFrame, JProgressBar prBar, JTextArea txtInfo)
		{
			m_mainFrame = mainFrame;
			m_prBar = prBar;
			m_txtInfo = txtInfo;
		}
		
		public void actionPerformed(ActionEvent e)
		{
			m_mainFrame.InitDownloadVariable();
			if( m_mainFrame.isAWSMode() == false )
				new Thread(new S2ADownloadProcessRunner(m_mainFrame, m_prBar, m_txtInfo)).start();
			else
				new Thread(new AWS_S2_DownloadProcessRunner(m_mainFrame, m_prBar, m_txtInfo, 
							cbDecompress.isSelected(),
							cbCheckReflectance.isSelected(),
							cbGdalMerge.isSelected() )).start();
		}
		
		private S2ADownloadFrame m_mainFrame = null;
		private JProgressBar m_prBar = null;
		private JTextArea m_txtInfo = null;
	}
	
	private class AboutButtonListener implements ActionListener
    {
		public AboutButtonListener(JFrame parent, String title, String programName)
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
	
	private class RadioButtonListener implements ChangeListener
    {
		public RadioButtonListener(S2ADownloadFrame parent, boolean flRbAWS)
		{
			m_parent = parent;
			m_flRbAWS = flRbAWS;
		}
		
		public void stateChanged(ChangeEvent e)
		{
			if( ( ((JRadioButton)(e.getSource())).isSelected() ) && (m_flRbAWS) )
                controlStateComponents(true);
            else 
            if( ( ((JRadioButton)(e.getSource())).isSelected() ) && (!m_flRbAWS) )
				controlStateComponents(false);
		}
		
		private S2ADownloadFrame m_parent = null;
		private boolean m_flRbAWS = false;
	}
	
	private class CheckBoxListener implements ChangeListener
    {
		public CheckBoxListener(S2ADownloadFrame parent)
		{
			m_parent = parent;
		}
		
		public void stateChanged(ChangeEvent e)
		{
			if( m_parent.isDecompress() == false )
			{
				m_parent.selectCheckReflectance(false);
			}
			
			if( m_parent.isDecompress() == true )
			{
				m_parent.selectCheckReflectance(true);
			}
						
			
			if( m_parent.isCheckReflectance() == false )
			{
				m_parent.selectGdalMerge(false);
			}
						
			
			if( m_parent.isCheckReflectance() == true )
			{
				m_parent.selectGdalMerge(true);
			}
		}
		
		private S2ADownloadFrame m_parent = null;
	}
}
