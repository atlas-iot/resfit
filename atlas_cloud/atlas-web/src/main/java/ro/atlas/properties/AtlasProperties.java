package ro.atlas.properties;

import org.springframework.boot.context.properties.ConfigurationProperties;

@ConfigurationProperties(prefix = "atlas-cloud")
public class AtlasProperties {
	/* MQTT cloud broker */
	private String broker;
	
	/* Cloud MQTT password utility tool */
	private String passwordTool;
	
	/* Temporary directory */
	private String tmpDir;
	
	/* Credentials reload executable */
	private String credentialsReloadExec;
	
	/* Gateway keep-alive counter */
	private int keepaliveCounter;
	
	/* Cloud MQTT timeout in seconds */
	private int mqttTimeout;

	/* Cloud MQTT QoS */
	private int mqttQos;

	/* The max number of samples in history lists */
	private int maxHistorySamples;
	
	/* Holds the firebase configuration file */
	private String firebaseConfigurationFile;
	
	/* Holds the owner restricted port. If this port is accessed, the client can query only the owner web controller */
	private int ownerRestrictedPort;
	
	/* Holds the owner web controller path prefix */
	private String ownerRestrictedPathPrefix;

	public String getBroker() {
		return broker;
	}

	public void setBroker(String broker) {
		this.broker = broker;
	}

	public int getKeepaliveCounter() {
		return keepaliveCounter;
	}

	public void setKeepaliveCounter(int keepaliveCounter) {
		this.keepaliveCounter = keepaliveCounter;
	}

	public int getMqttTimeout() {
		return mqttTimeout;
	}

	public void setMqttTimeout(int mqttTimeout) {
		this.mqttTimeout = mqttTimeout;
	}

	public int getMqttQos() {
		return mqttQos;
	}

	public void setMqttQos(int mqttQos) {
		this.mqttQos = mqttQos;
	}

	public String getPasswordTool() {
		return passwordTool;
	}

	public void setPasswordTool(String passwordTool) {
		this.passwordTool = passwordTool;
	}

	public String getTmpDir() {
		return tmpDir;
	}

	public void setTmpDir(String tmpDir) {
		this.tmpDir = tmpDir;
	}
	
	public int getMaxHistorySamples() {
		return maxHistorySamples;
	}

	public void setMaxHistorySamples(int maxHistorySamples) {
		this.maxHistorySamples = maxHistorySamples;
	}

	public String getCredentialsReloadExec() {
		return credentialsReloadExec;
	}

	public void setCredentialsReloadExec(String credentialsReloadExec) {
		this.credentialsReloadExec = credentialsReloadExec;
	}

	public String getFirebaseConfigurationFile() {
		return firebaseConfigurationFile;
	}

	public void setFirebaseConfigurationFile(String firebaseConfigurationFile) {
		this.firebaseConfigurationFile = firebaseConfigurationFile;
	}

	public int getOwnerRestrictedPort() {
		return ownerRestrictedPort;
	}

	public void setOwnerRestrictedPort(int ownerRestrictedPort) {
		this.ownerRestrictedPort = ownerRestrictedPort;
	}

	public String getOwnerRestrictedPathPrefix() {
		return ownerRestrictedPathPrefix;
	}

	public void setOwnerRestrictedPathPrefix(String ownerRestrictedPathPrefix) {
		this.ownerRestrictedPathPrefix = ownerRestrictedPathPrefix;
	}
}
