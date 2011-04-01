/*
 * AdminApplet.java
 *
 * Created on October 1, 2002, 13:53 PM
 */

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.applet.*;
import java.net.URL;
import java.net.MalformedURLException;
/**
 *
 * @author  donn morrison
 */
public class AdminApplet extends Applet {
	private Label lPassword = new Label("Enter admin password:");
	private TextField tfPassword = new TextField(25);
	private Panel pMain = new Panel();

	public AdminApplet() {}
	public void init()
	{
		try
		{            
			int width = Integer.parseInt(this.getParameter("width"));
			int height = Integer.parseInt(this.getParameter("height"));
			URL hostURL = new URL(getParameter("URL"));

			setLayout(new FlowLayout());
			//add(lPassword, BorderLayout.WEST);
			//add(tfPassword, BorderLayout.CENTER);
			add(lPassword);
			add(tfPassword);

			tfPassword.setEchoChar('*');

			tfPassword.addKeyListener(new KeyListener() {
				public void keyTyped(KeyEvent e){}
				public void keyPressed(KeyEvent e)
				{
					if(e.getKeyCode() == KeyEvent.VK_ENTER && !tfPassword.getText().equals(""))
					{
						doAdminConnect(tfPassword.getText());
						tfPassword.setText("");
					}
				}
				public void keyReleased(KeyEvent e){}
			});
		}
		catch(NumberFormatException e)
		{
			e.printStackTrace();
		}
		catch(MalformedURLException e)
		{
			e.printStackTrace();
		}
	}

	public void stop()
	{
	}

	public void doAdminConnect(String password)
	{

	}
}
