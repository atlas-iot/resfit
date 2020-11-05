package ro.atlas.entity;

import java.util.Date;
import java.util.HashMap;
import java.util.LinkedList;

import org.springframework.data.annotation.Id;
import org.springframework.data.mongodb.core.index.Indexed;
import org.springframework.data.mongodb.core.mapping.Document;

import ro.atlas.dto.AtlasClientCommandDto;

@Document
public class AtlasOwner {
	/* Document id */
	@Id
	private String id;

	/* Gateway unique identifier */
	@Indexed(unique = true)
	private String ownerIdentity;
	
	/* Owner Android firebase token (used to notify the Android application) */
	private String firebaseToken;
	
	/* Indicates if the user should be notified or not */
	private boolean notifyUser;
	
	/* Last user notification time */
	private Date lastNotificationTime;
	
	/* Holds the pending commands for the owner.
	 * The key is the gateway identity and the value is the gateway identity. */
	private HashMap<String, LinkedList<AtlasClientCommandDto>> ownerCommands;

	public String getOwnerIdentity() {
		return ownerIdentity;
	}

	public void setOwnerIdentity(String ownerIdentity) {
		this.ownerIdentity = ownerIdentity;
	}
	
	public HashMap<String, LinkedList<AtlasClientCommandDto>> getOwnerCommands() {
		return ownerCommands;
	}

	public void setOwnerCommands(HashMap<String, LinkedList<AtlasClientCommandDto>> ownerCommands) {
		this.ownerCommands = ownerCommands;
	}

	public Date getLastNotificationTime() {
		return lastNotificationTime;
	}

	public void setLastNotificationTime(Date lastNotificationTime) {
		this.lastNotificationTime = lastNotificationTime;
	}

	public boolean isNotifyUser() {
		return notifyUser;
	}

	public void setNotifyUser(boolean notifyUser) {
		this.notifyUser = notifyUser;
	}

	public String getFirebaseToken() {
		return firebaseToken;
	}

	public void setFirebaseToken(String firebaseToken) {
		this.firebaseToken = firebaseToken;
	}

}
