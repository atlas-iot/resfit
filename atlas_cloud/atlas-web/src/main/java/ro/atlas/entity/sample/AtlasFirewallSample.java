package ro.atlas.entity.sample;

import java.util.Date;

public class AtlasFirewallSample {
	/* Sample timestamp */
	private Date date;
	
	/* Number of passed packets */
	private int passed;
	
	/* Number of dropped packets */
	private int dropped;
	
	public AtlasFirewallSample(Date date, int passed, int dropped) {
		this.date = date;
		this.passed = passed;
		this.dropped = dropped;
	}
	
	public Date getDate() {
		return date;
	}
	
	public void setDate(Date date) {
		this.date = date;
	}
	
	public int getPassed() {
		return passed;
	}
	
	public void setPassed(int passed) {
		this.passed = passed;
	}
	
	public int getDropped() {
		return dropped;
	}
	
	public void setDropped(int dropped) {
		this.dropped = dropped;
	}
}
