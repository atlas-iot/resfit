package ro.atlas.entity.sample;

import java.util.Date;

public class AtlasReputationSample {
	/* Sample timestamp */
	private Date date;
	
	/* System reputation sample*/
	private double system;
	
	/* Temperature reputation sample */
	private double temperature;
	
	public AtlasReputationSample(Date date, double system, double temperature) {
		this.date = date;
		this.system = system;
		this.temperature = temperature;
	}
	
	public Date getDate() {
		return date;
	}
	
	public void setDate(Date date) {
		this.date = date;
	}
	
	public double getSystem() {
		return system;
	}
	
	public void setSystem(double system) {
		this.system = system;
	}
	
	public double getTemperature() {
		return temperature;
	}
	
	public void setTemperature(double temperature) {
		this.temperature = temperature;
	}
	
}
