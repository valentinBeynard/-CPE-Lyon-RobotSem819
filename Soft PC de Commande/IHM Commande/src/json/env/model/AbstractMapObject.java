package json.env.model;

import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

public abstract class AbstractMapObject {
	
	private String filePath = "/fs03/share/users/mathieu.lai-king/home/Bureau/"
    		+ "Projet transversal/CentraleCommande_FCT17/"
    		+ "Description_Environnement_SansAccent_V10-05-17.js" ;
	private JSONObject jo;
	
	public AbstractMapObject() throws FileNotFoundException, IOException, ParseException{
		this.jo = this.getJSONObject();
	}
	
	public JSONObject getJSONObject() throws FileNotFoundException, IOException, ParseException {
		// parsing file "JSONExample.json" 
		Object obj = new JSONParser().parse(new FileReader(filePath)); 
	     // typecasting obj to JSONObject 
		JSONObject jo = (JSONObject) obj;
		return jo;
	}
	
}
