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

package org.rsutils;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

public class AboutDialog extends JDialog
{
	public AboutDialog(JFrame owner, String title, String programName)
	{
		super(owner, title);
		
		this.setBounds(new Rectangle(300,120));
        this.setResizable(false);
		
		JLabel jLabelName = new JLabel(programName);
		jLabelName.setFont(new Font("Dialog", Font.BOLD, 12));
		Box hbox1 = Box.createHorizontalBox();
		hbox1.add(jLabelName);
				
		JLabel jLabelCopyright = new JLabel("Copyright (C) 2016,  Igor Garkusha");
		jLabelCopyright.setFont(new Font("Dialog", Font.BOLD, 12));
		Box hbox2 = Box.createHorizontalBox();
		hbox2.add(jLabelCopyright);
				
		JLabel jLabelCountry = new JLabel("Ukraine, Dnipropetrovsk");
		jLabelCountry.setFont(new Font("Dialog", Font.BOLD, 12));
		Box hbox3 = Box.createHorizontalBox();
		hbox3.add(jLabelCountry);
		
		JLabel GPLV3Logo = new JLabel();
		GPLV3Logo.setIcon(new ImageIcon(getClass().getResource("/org/rsutils/gplv3-127x51.png")));
		Box hbox4 = Box.createHorizontalBox();
		hbox4.add(GPLV3Logo);
		
		JTextArea txtLicense = new JTextArea();
        txtLicense.setEditable(false);
        txtLicense.setColumns(43);
        txtLicense.setRows(15);
        JScrollPane scrollLicenseArea = new JScrollPane(txtLicense);
        Box hbox5 = Box.createHorizontalBox();
        hbox5.add(Box.createHorizontalStrut(5));
		hbox5.add(scrollLicenseArea);
		hbox5.add(Box.createHorizontalStrut(5));
		
		JButton btnOk = new JButton("OK");
        Box hbox6 = Box.createHorizontalBox();
		hbox6.add(btnOk);
		
		btnOk.addActionListener(new ActionListener(){ public void actionPerformed(ActionEvent e){ setVisible(false); } });
        
		String strLicense = "This program is free software: you can redistribute it and/or modify\n"+
							"it under the terms of the GNU General Public License as published by\n"+
							"the Free Software Foundation, either version 3 of the License, or\n"+
							"(at your option) any later version.\n\n"+
							"This program is distributed in the hope that it will be useful,\n"+
							"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"+
							"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"+
							"GNU General Public License for more details.\n\n"+
							"You should have received a copy of the GNU General Public License\n"+
							"along with this program.  If not, see http://www.gnu.org/licenses/.\n";							
	
		txtLicense.append(strLicense);

		Box vbox = Box.createVerticalBox();
		vbox.add(Box.createVerticalStrut(20));
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
		vbox.add(Box.createVerticalStrut(20));
        
        add(vbox);
        
        this.pack();
        
        setLocationRelativeTo(null);
	}
    
    private static final long serialVersionUID = 1L;
}
