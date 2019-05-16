package json.env.model;

import java.awt.Point;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.ParseException;

public class Terrain extends AbstractMapObject {
	
	public List<Point> coords = new ArrayList<Point>();
	
	public Terrain() throws FileNotFoundException, IOException, ParseException {
		super();
		this.setCoords();
	}

	public List<Point> getCoords() {
		return coords;
	}
	
	/**
	 * Ajoute les coordonnées du terrain du fichier JSON a l'attribut coords
	 * @throws FileNotFoundException
	 * @throws IOException
	 * @throws ParseException
	 */
	public void setCoords() throws FileNotFoundException, IOException, ParseException {
		JSONObject jo = this.getJSONObject(); // fichier JSON complet
		JSONArray ja = (JSONArray) jo.get("terrain_evolution"); // terrain
		
		int cpt = 0;
		for (int i = 0; i < ja.size(); i++) {
			JSONObject c = (JSONObject) ja.get(i); // Obtention de l'objet JSON point
			Map point = ((Map)c.get("point")); // Chaque point du terrain
			Iterator<Map.Entry> itr1 = point.entrySet().iterator(); 
			
			int x = 0;
			int y = 0;
			while (itr1.hasNext()) {
				Map.Entry pair = itr1.next(); 
				if (pair.getKey().equals("y")) { // si la clé vaut y 
					y=((Long)pair.getValue()).intValue();
				}
				else {
					x=((Long)pair.getValue()).intValue();
				}
	        } 
			cpt++;
			if (cpt == 1) {
				this.coords.add(new Point(x,y)); // Ajout du point
			}
			if (cpt == 2) {
				cpt = 0;
			}
		}
 	}
	
	public int[] getX() {
		List<Point> coords = this.getCoords();
		int[] x = new int[coords.size()];
		for (int i = 0;i<coords.size();i++) {
			x[i]=((int)coords.get(i).getX());
		}	
		return x;
	}
	public int[] getY() {
		List<Point> coords = this.getCoords();
		int[] y = new int[coords.size()];
		for (int i = 0;i<coords.size();i++) {
			y[i]=((int)coords.get(i).getY());
		}
		return y;
	}
	
	public int getPtNb() {
		return coords.size();
	}
			
	
	public static void main(String args[]) throws FileNotFoundException, IOException, ParseException {
		Terrain t = new Terrain();
		
		t.setCoords();
		System.out.println(t.getCoords());
	}
	
	
}
