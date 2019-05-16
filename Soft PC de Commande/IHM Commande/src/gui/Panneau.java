package gui;
import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;
import javax.swing.JPanel;

public class Panneau extends JPanel{
  private Color color = Color.white;
  private static int COUNT = 0;
  
   
  public Panneau(){}
  public Panneau(Color color){
    this.color = color;
    
  }
  public void paintComponent(Graphics g){
    g.setColor(this.color);
    g.fillRect(0, 0, this.getWidth(), this.getHeight());
    g.setColor(Color.white);
    g.setFont(new Font("Arial", Font.BOLD, 15));
  }
}
