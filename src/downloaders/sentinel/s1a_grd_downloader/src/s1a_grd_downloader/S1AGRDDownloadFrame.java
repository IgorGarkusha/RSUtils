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

import org.rsutils.*;
import org.rsutils.downloader.*;
import java.io.*;
import java.nio.file.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.filechooser.*;

public class S1AGRDDownloadFrame extends ConsoleS1AGRDDownloadFrame implements Runnable
{
    public S1AGRDDownloadFrame()
    {
		super(null);
		
        frm = new JFrame();
        panel = new JPanel();

        txtProductID = new JTextField(50);
        txtProductName = new JTextField(50);
        cbPolarization = new JComboBox<String>(new String[]{"all", "VV", "VH"});
        txtUsername = new JTextField(20);
        txtPassword = new JPasswordField(10);
        
        txtInfoArea = new JTextArea();
        scrollArea = new JScrollPane(txtInfoArea);

        txtInfoArea.setEditable(false);
        txtInfoArea.setColumns(20);
        txtInfoArea.setRows(5);
                
        pBar = new JProgressBar();
        pBar.setStringPainted(true);

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
		m_ProductID = txtProductID.getText();
		m_ProductName = txtProductName.getText();
		m_UserNameAndPasword = this.txtUsername.getText()+":"+new String(this.txtPassword.getPassword());
		
		switch(cbPolarization.getSelectedIndex())
		{
			case 0: // all polarization
					band_indexes = null;
					m_POLARIZATION = "all";
					break;
			case 1: // VV
					band_indexes = new int[1];
					band_indexes[0] = 0;
					m_POLARIZATION = "vv";
					break;
			case 2: // VH
					band_indexes = new int[1];
					band_indexes[0] = 1;
					m_POLARIZATION = "vh";
		}
	}

    public void CreatePanel()
    {
		JLabel jLabelWorkDir = new JLabel("Work Directory:");
		jLabelWorkDir.setFont(new Font("Dialog", 0, 12));
		
		JLabel jLabelProdID = new JLabel("Product ID:");
		jLabelProdID.setFont(new Font("Dialog", 0, 12));
		
		JLabel jLabelProdName = new JLabel("Product Name:");
		jLabelProdName.setFont(new Font("Dialog", 0, 12));
		
		JLabel jLabelBands = new JLabel("Download bands:");
		jLabelBands.setFont(new Font("Dialog", 0, 12));
		
		JLabel jLabelUsername = new JLabel("Username:");
		jLabelUsername.setFont(new Font("Dialog", 0, 12));
		
		JLabel jLabelPassword = new JLabel("Password:");
		jLabelPassword.setFont(new Font("Dialog", 0, 12));

		Box hbox1 = Box.createHorizontalBox(); hbox1.add(jLabelWorkDir); hbox1.add(txtWorkDir); hbox1.add(btnSetWorkDir);
		Box hbox2 = Box.createHorizontalBox(); hbox2.add(jLabelProdID); hbox2.add(txtProductID);
		Box hbox3 = Box.createHorizontalBox(); hbox3.add(jLabelProdName); hbox3.add(txtProductName);
		Box hbox4 = Box.createHorizontalBox(); hbox4.add(jLabelBands); hbox4.add(cbPolarization); hbox4.add(Box.createHorizontalGlue());
		
		Box hbox5 = Box.createHorizontalBox(); 
		hbox5.add(jLabelUsername); hbox5.add(txtUsername); hbox5.add(Box.createHorizontalStrut(10)); hbox5.add(jLabelPassword); hbox5.add(txtPassword);
		
		Box hbox6 = Box.createHorizontalBox();
		hbox6.add(pBar);
		
		Box hbox7 = Box.createHorizontalBox();
		hbox7.add(scrollArea);
		
		Box hbox8 = Box.createHorizontalBox();
		hbox8.add(btnSaveParams); hbox8.add(btnLoadParams); hbox8.add(Box.createHorizontalStrut(40)); hbox8.add(btnDownload); hbox8.add(Box.createHorizontalStrut(40));
		hbox8.add(btnExit); hbox8.add(Box.createHorizontalStrut(20)); hbox8.add(btnAbout);
				
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
        vbox.add(Box.createVerticalStrut(10));
        vbox.add(hbox8);
                
        panel.add(vbox);
        frm.pack();
	}

	private void setParameter(int paramIndex, String str)
    {
        switch(paramIndex)
        {
            case 1: txtWorkDir.setText(str); break;
            case 2: txtProductID.setText(str); break;
            case 3: txtProductName.setText(str); break;
            case 4: txtUsername.setText(str); break;
            case 5: cbPolarization.setSelectedIndex(Integer.parseInt(str)); break;
        }
    }

    public void CreateListener()
    {
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
				FileNameExtensionFilter filter = new FileNameExtensionFilter("Parameters S1A CFG", "s1a.cfg");
				chooser.setFileFilter(filter);
				chooser.setCurrentDirectory(new File(curDir));
				int res = chooser.showSaveDialog(frm);
				if(res == JFileChooser.APPROVE_OPTION) 
				{
					String fileName = chooser.getSelectedFile().getAbsolutePath();
					
					String ext = fileName.substring(fileName.length()-8, fileName.length());
					if( ext.compareToIgnoreCase(".s1a.cfg") != 0 ) fileName += ".s1a.cfg";

					PrintStream fout = null;
					try
					{
						fout = new PrintStream(fileName);
						fout.println(txtWorkDir.getText());
						fout.println(txtProductID.getText());
						fout.println(txtProductName.getText());
						fout.println(txtUsername.getText());
						fout.println(cbPolarization.getSelectedIndex());
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
				FileNameExtensionFilter filter = new FileNameExtensionFilter("Parameters S1A CFG", "cfg");
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
        frm.setBounds(new Rectangle(300,120));
        frm.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frm.add(panel);
        frm.setResizable(false);
        frm.setVisible(true);
        frm.pack();
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
	
    private JFrame frm;
    private JPanel panel;
    private JTextField txtWorkDir;
    private JTextField txtProductID, txtProductName, txtUsername;
    private JPasswordField txtPassword;
    private JTextArea txtInfoArea;
    private JScrollPane scrollArea;
    private JProgressBar pBar;
    private JComboBox<String> cbPolarization;
    
    private JButton btnSetWorkDir;
    private JButton btnSaveParams;
    private JButton btnLoadParams;
    private JButton btnDownload;
    private JButton btnExit;
    private JButton btnAbout;
    
    private String ProgramTitle = "Sentinel-1A GRD-Product Downloader. Version " + 
								  s1a_grd_download.PROG_VERSION +"."+s1a_grd_download.DATE_VERSION;
								  
    private String curDir = System.getProperty("user.dir");
       
    private class DownloadButtonLister implements ActionListener
    {
		public DownloadButtonLister(S1AGRDDownloadFrame mainFrame, JProgressBar prBar, JTextArea txtInfo)
		{
			m_mainFrame = mainFrame;
			m_prBar = prBar;
			m_txtInfo = txtInfo;
		}
		
		public void actionPerformed(ActionEvent e)
		{
			m_mainFrame.InitDownloadVariable();
			new Thread(new S1AGRDDownloadProcessRunner(m_mainFrame, m_prBar, m_txtInfo)).start();
		}
		
		private S1AGRDDownloadFrame m_mainFrame = null;
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
