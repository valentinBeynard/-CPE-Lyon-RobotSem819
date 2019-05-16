package gui;

import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Point;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.json.simple.parser.ParseException;

import json.env.model.Terrain;

public class PanelDrawMap extends Panneau {
	
	private Terrain terrain ;
	public PanelDrawMap(Color color) throws FileNotFoundException, IOException, ParseException{
	    super(color);
	    this.terrain = new Terrain();
	}
	
	public void paintComponent(Graphics g){
	    super.paintComponent(g);
	    
	    g.drawPolygon(terrain.getX(), terrain.getY(),terrain.getPtNb());
	}
	
	
	
}
