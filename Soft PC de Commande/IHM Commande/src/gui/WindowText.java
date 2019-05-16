package gui;

import java.awt.Color;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Enumeration;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JTabbedPane;
import javax.swing.JTextField;

import org.json.simple.parser.ParseException;

import gnu.io.CommPortIdentifier;
import serial.SerialSender;

public class WindowText extends JFrame  {
	private JTabbedPane onglet;
	
	
	//Port Série
	private Enumeration ports = CommPortIdentifier.getPortIdentifiers();
	private CommPortIdentifier port = (CommPortIdentifier) ports.nextElement();
	private JLabel serialPort = new JLabel("Port utilisé :" + port.getName());
	//Onglet 1 : envoi commandes
	private JLabel enterText = new JLabel("Entrez la commande désirée :");
	private JTextField cmdInput = new JTextField();
	private JButton sendButton = new JButton("Send");
	
	//Onglet 2 : Terrain
	
	
	//Onglets
	private Panneau[] tPan = {   new Panneau(Color.RED), new Panneau(Color.GREEN),new Panneau(Color.GRAY)};
	
	
	private int cpt = 1;
	
	  public WindowText() throws FileNotFoundException, IOException, ParseException{
		  
		
	 	Panneau drawpan = this.getPanelDraw(Color.BLACK);
		
	    this.setLocationRelativeTo(null);
	    this.setTitle("Gérer vos conteneurs");
	    this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	    this.setSize(400, 200);
	      
	    //Création de plusieurs Panneau
	    
	    
	    //Ajout éléments premier onglet
	    cmdInput.setColumns(10);
	    tPan[0].setLayout(new GridLayout(4,1,5,5));
	    tPan[0].add(serialPort);
	    tPan[0].add(enterText);
	    tPan[0].add(cmdInput);
	    tPan[0].add(sendButton);
	    
	    // Ajout 2eme onglet 
	    
	    //Ajout 3eme onglet
	    tPan[2].setLayout(new GridLayout(cpt,1,5,5));
	    
	    // Gestion bouton
	    sendButton.addActionListener(new ActionListener() {  
			public void actionPerformed(ActionEvent e) {  
				
				SerialSender.send(port.getName(), cmdInput.getText());
				JLabel cmdSent =  new JLabel(cmdInput.getText());
				tPan[2].add(cmdSent);
				cpt++;
				
				
			}  
		});
	    //Création de notre conteneur d'onglets
	    onglet = new JTabbedPane();
	    
	    onglet.add("Envoi de commandes",tPan[0]);
	    
	    onglet.add("Map",drawpan);
	    onglet.add("Commandes envoyées",tPan[2]);
	    //On passe ensuite les onglets au content pane
	    this.getContentPane().add(onglet);
	    this.setVisible(true);
	  }
	  
	  public PanelDrawMap getPanelDraw(Color color) throws FileNotFoundException, IOException, ParseException {
		  return new PanelDrawMap(color);
	  }
	  public static void main (String args[]) throws FileNotFoundException, IOException, ParseException {
		  WindowText wt = new WindowText();
	  }

}

