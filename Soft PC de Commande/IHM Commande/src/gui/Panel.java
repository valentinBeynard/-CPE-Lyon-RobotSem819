package gui;

import java.awt.Graphics;
import javax.swing.JPanel;
 
public class Panel extends JPanel { 
  /**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	
	public void paintComponent(Graphics g){
		g.drawString("Déplacement :", 10, 20);
		int x1 = this.getWidth()/4;
	    int y1 = this.getHeight()/4;                      
	    g.fillOval(x1, y1, this.getWidth()/2, this.getHeight()/2);
	  }               
}