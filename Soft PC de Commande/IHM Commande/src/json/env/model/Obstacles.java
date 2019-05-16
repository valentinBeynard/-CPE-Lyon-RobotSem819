package json.env.model;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.List;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.ParseException;

public class Obstacles extends AbstractMapObject {
	
	public Obstacles() throws FileNotFoundException, IOException,
			ParseException {
		super();
	}

	private List<Obstacle> obstacles;
	
	public void setObstacles() throws FileNotFoundException, IOException, ParseException {
		JSONObject env = this.getJSONObject();
		JSONArray obstacles = (JSONArray) env.get("obstacles");
			
			
		
	}
}
