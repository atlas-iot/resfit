package ro.atlas.dto;

public class AtlasClientSummaryDto {
	 /* Client device identity */
    private String identity;
    private String alias;
    
    /* Client device system info */
    private String registered;
    private String lastRegisterTime;
    private String lastKeepAliveTime;
    
    /* Client device hostname */
    private String hostname;
    
    /* Client device IP and port */
    private String ipPort;
    
    /* Reputation values */
    private String systemReputation;
    private String temperatureReputation;
    
	public String getIdentity() {
		return identity;
	}

	public void setIdentity(String identity) {
		this.identity = identity;
	}

	public String getRegistered() {
		return registered;
	}

	public void setRegistered(String registered) {
		this.registered = registered;
	}

	public String getLastRegisterTime() {
		return lastRegisterTime;
	}

	public void setLastRegisterTime(String lastRegisterTime) {
		this.lastRegisterTime = lastRegisterTime;
	}

	public String getLastKeepAliveTime() {
		return lastKeepAliveTime;
	}

	public void setLastKeepAliveTime(String lastKeepAliveTime) {
		this.lastKeepAliveTime = lastKeepAliveTime;
	}

	public String getHostname() {
		return hostname;
	}

	public void setHostname(String hostname) {
		this.hostname = hostname;
	}

	public String getSystemReputation() {
		return systemReputation;
	}

	public void setSystemReputation(String systemReputation) {
		this.systemReputation = systemReputation;
	}

	public String getTemperatureReputation() {
		return temperatureReputation;
	}

	public void setTemperatureReputation(String temperatureReputation) {
		this.temperatureReputation = temperatureReputation;
	}

	public String getIpPort() {
		return ipPort;
	}

	public void setIpPort(String ipPort) {
		this.ipPort = ipPort;
	}

	public String getAlias() {
		return alias;
	}

	public void setAlias(String alias) {
		this.alias = alias;
	}
}
