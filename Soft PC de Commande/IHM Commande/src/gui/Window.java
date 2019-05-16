package gui;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Font;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
 
public class Window extends JFrame{
 
	private static final long serialVersionUID = 1L;
	private Panel pan = new Panel();
	
	// Commande D (début épreuve)
	private JButton Dbutton = new JButton("Debut de l'épreuve");
	
	private JComboBox Dlist;
	//Commande E
	private JButton Ebutton = new JButton("Fin de l'épreuve");
	
	//Conteneur
	//private JPanel container = new JPanel();
	
	private JLabel label = new JLabel("LABEL");
  
	public Window(){
		this.setTitle("Centrale de commande");
		this.setSize(800, 1200);
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		this.setLocationRelativeTo(null);
 
		//On crée nos différents conteneurs de couleur différente
	    JPanel cell1 = new JPanel();
	    cell1.setPreferredSize(new Dimension(150, 50));	
	    //Dbutton.addActionListener(this);
	    cell1.add(Dbutton);
	    
	    JPanel cell2 = new JPanel();
	    cell2.setPreferredSize(new Dimension(60, 50));
	    Object[] elements = new Object[]{"1", "2", "3", "4", "5","6","7","8"};
		Dlist = new JComboBox(elements);
	    cell2.add(Dlist);
	    
	    JPanel cell3 = new JPanel();
	    cell3.setBackground(Color.green);
	    cell3.setPreferredSize(new Dimension(60, 40));
	    cell3.add(label);
	    
	    
	    JPanel cell4 = new JPanel();
	    cell4.setBackground(Color.black);
	    cell4.setPreferredSize(new Dimension(60, 40));
	    JPanel cell5 = new JPanel();
	    cell5.setBackground(Color.cyan);
	    cell5.setPreferredSize(new Dimension(60, 40));
	    JPanel cell6 = new JPanel();
	    cell6.setBackground(Color.BLUE);
	    cell6.setPreferredSize(new Dimension(60, 40));
	    JPanel cell7 = new JPanel();
	    cell7.setBackground(Color.orange);
	    cell7.setPreferredSize(new Dimension(60, 40));
	    JPanel cell8 = new JPanel();
	    cell8.setBackground(Color.DARK_GRAY);
	    cell8.setPreferredSize(new Dimension(60, 40));
			
	    //Le conteneur principal
	    JPanel content = new JPanel();
	    content.setPreferredSize(new Dimension(800, 1200));
	    content.setBackground(Color.WHITE);
	    //On définit le layout manager
	    content.setLayout(new GridBagLayout());
			
	    //L'objet servant à positionner les composants
	    GridBagConstraints gbc = new GridBagConstraints();
			
	    //On positionne la case de départ du composant
	    gbc.gridx = 0;
	    gbc.gridy = 0;
	    //La taille en hauteur et en largeur
	    gbc.gridheight = 1;
	    gbc.gridwidth = 1;
	    content.add(cell1, gbc);
	    //---------------------------------------------
	    gbc.gridx = 1;
	    content.add(cell2, gbc);
	    //---------------------------------------------
	    gbc.gridx = 2;		
	    content.add(cell3, gbc);		
	    //---------------------------------------------
	    //Cette instruction informe le layout que c'est une fin de ligne
	    gbc.gridwidth = GridBagConstraints.REMAINDER;
	    gbc.gridx = 3;	
	    content.add(cell4, gbc);
	    //---------------------------------------------
	    gbc.gridx = 0;
	    gbc.gridy = 1;
	    gbc.gridwidth = 1;
	    gbc.gridheight = 2;
	    //Celle-ci indique que la cellule se réplique de façon verticale
	    gbc.fill = GridBagConstraints.VERTICAL;
	    content.add(cell5, gbc);
	    //---------------------------------------------
	    gbc.gridx = 1;
	    gbc.gridheight = 1;
	    //Celle-ci indique que la cellule se réplique de façon horizontale
	    gbc.fill = GridBagConstraints.HORIZONTAL;
	    gbc.gridwidth = GridBagConstraints.REMAINDER;
	    content.add(cell6, gbc);
	    //---------------------------------------------
	    gbc.gridx = 1;
	    gbc.gridy = 2;
	    gbc.gridwidth = 2;
	    content.add(cell7, gbc);
	    //---------------------------------------------
	    gbc.gridx = 3;
	    gbc.gridwidth = GridBagConstraints.REMAINDER;
	    content.add(cell8, gbc);
	    //---------------------------------------------
	    //On ajoute le conteneur
	    this.setContentPane(content);
	    this.setVisible(true);		
  }
      
	private void go(){
		
	}
      
	//Classe écoutant notre premier bouton
	class BoutonListener implements ActionListener{
    //Redéfinition de la méthode actionPerformed()
		public void actionPerformed(ActionEvent arg0) {
			label.setText(Dlist.getActionCommand());   
		}
	}
      
  //Classe écoutant notre second bouton
	class Bouton2Listener implements ActionListener{
		//Redéfinition de la méthode actionPerformed()
		public void actionPerformed(ActionEvent e) {
			label.setText("ok");    
		}
	}      
}



 
