package json.env.model;

import java.awt.Point;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.List;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.ParseException;

public abstract class Obstacle  {
	
	
	private String type;
	private Object[] coords;
	
	
	public Obstacle(String type) {
		this.type = type;
	}
	
	
	
	
}
