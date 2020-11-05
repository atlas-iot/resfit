package ro.atlas.entity;

import java.util.HashMap;
import java.util.LinkedList;

import org.springframework.data.annotation.Id;
import org.springframework.data.mongodb.core.index.Indexed;
import org.springframework.data.mongodb.core.mapping.Document;

import ro.atlas.commands.AtlasGatewayCommand;

@Document
public class AtlasGateway {
	/* Document id */
	@Id
	private String id;

	/* Gateway unique identifier */
	@Indexed(unique = true)
	private String identity;

	/* Gateway pre-shared key */
	@Indexed(unique = true)
	private String psk;

	/* Gateway's alias */
	@Indexed(unique = true)
	private String alias;

	/* Indicates if the gateway is registered */
	private boolean registered;

	/* Holds the last registration time */
	private String lastRegistertTime;

	/* Holds the last keep-alive time */
	private String lastKeepaliveTime;

	/* Holds the keep-alive counter to detect inactive gateways */
	private int keepaliveCounter;

	/* Client information */
	private HashMap<String, AtlasClient> clients;

	/* Pending gateway commands list */
	private LinkedList<AtlasGatewayCommand> gatewayPendingCmds;

	/* Client command global sequence number */
	private int globalCmdSeqNo;

	/* Holds the gateway information */
	private AtlasGatewayInfo gatewayInfo;

	public String getIdentity() {
		return identity;
	}

	public void setIdentity(String identity) {
		this.identity = identity;
	}

	public String getPsk() {
		return psk;
	}

	public void setPsk(String psk) {
		this.psk = psk;
	}

	public HashMap<String, AtlasClient> getClients() {
		return clients;
	}

	public void setClients(HashMap<String, AtlasClient> clients) {
		this.clients = clients;
	}

	public boolean isRegistered() {
		return registered;
	}

	public void setRegistered(boolean registered) {
		this.registered = registered;
	}

	public String getLastRegistertTime() {
		return lastRegistertTime;
	}

	public void setLastRegistertTime(String lastRegistertTime) {
		this.lastRegistertTime = lastRegistertTime;
	}

	public String getLastKeepaliveTime() {
		return lastKeepaliveTime;
	}

	public void setLastKeepaliveTime(String lastKeepaliveTime) {
		this.lastKeepaliveTime = lastKeepaliveTime;
	}

	public int getKeepaliveCounter() {
		return keepaliveCounter;
	}

	public void setKeepaliveCounter(int keepaliveCounter) {
		this.keepaliveCounter = keepaliveCounter;
	}

	public String getAlias() {
		return alias;
	}

	public void setAlias(String alias) {
		this.alias = alias;
	}

	public LinkedList<AtlasGatewayCommand> getGatewayPendingCmds() {
		return gatewayPendingCmds;
	}

	public void setGatewayPendingCmds(LinkedList<AtlasGatewayCommand> cmds) {
		gatewayPendingCmds = cmds;
	}

	public int getGlobalCommandSeqNo() {
		return globalCmdSeqNo;
	}

	public void setGlobalCommandSeqNo(int cmdSeqNo) {
		globalCmdSeqNo = cmdSeqNo;
	}

	public AtlasGatewayInfo getGatewayInfo() {
		return gatewayInfo;
	}

	public void setGatewayInfo(AtlasGatewayInfo gatewayInfo) {
		this.gatewayInfo = gatewayInfo;
	}
}
