package json.env.model;

import java.io.FileNotFoundException;
import java.io.IOException;

import org.json.simple.parser.ParseException;

public class Circle extends Obstacle{
	
	private int xcentre;
	private int ycentre;
	private int rayon;
	public Circle(String type,int xcentre, int ycentre, int rayon) throws FileNotFoundException, IOException, ParseException {
		super(type);
		this.xcentre = xcentre;
		this.ycentre = ycentre;
		this.rayon = rayon;
	}

}
