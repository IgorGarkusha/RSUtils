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

package org.rsutils.downloader;

import java.io.*;
import javax.swing.*;

public class ProcessRunner 
{
	public ProcessRunner(DownLoadMainFrameInterface mainFrameDownloadProgram, JProgressBar prBar, JTextArea txtInfo)
	{
		m_mainFrame = mainFrameDownloadProgram;
		m_prBar = prBar;
		m_txtInfo = txtInfo;
	}
	
	public DownLoadMainFrameInterface getMainFrame()
	{
		return m_mainFrame;
	}
	
	public JProgressBar getProgressBar()
	{
		return m_prBar;
	}
	
	public JTextArea getTextInfoArea()
	{
		return m_txtInfo;
	}
		
    protected DownLoadMainFrameInterface m_mainFrame = null;
    private JProgressBar m_prBar = null;
    private JTextArea m_txtInfo = null;    
    private Thread thread = null;
}
