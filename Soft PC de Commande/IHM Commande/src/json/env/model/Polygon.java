package json.env.model;

import java.io.FileNotFoundException;
import java.io.IOException;

import org.json.simple.parser.ParseException;

public class Polygon extends Obstacle {
	
	private int nb_cotes;
	private int[] xcoords = new int[nb_cotes];
	private int[] ycoords = new int[nb_cotes];
	
	public Polygon(String type, int nb_cotes, int[] xcoords, int[] ycoords)
			throws FileNotFoundException, IOException, ParseException {
		super(type);
		this.nb_cotes = nb_cotes;
		this.xcoords = xcoords;
		this.ycoords = ycoords;
	}
	
}
