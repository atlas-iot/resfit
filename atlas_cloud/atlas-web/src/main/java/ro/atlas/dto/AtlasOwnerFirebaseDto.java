package ro.atlas.dto;

public class AtlasOwnerFirebaseDto {
	
	/* Holds the owner firebase token */
	private String firebaseToken;

	public AtlasOwnerFirebaseDto() {
		this.firebaseToken = "";
	}
	
	public AtlasOwnerFirebaseDto(String firebaseToken) {
		this.firebaseToken = firebaseToken;
	}
	
	public String getFirebaseToken() {
		return firebaseToken;
	}

	public void setFirebaseToken(String firebaseToken) {
		this.firebaseToken = firebaseToken;
	}
}
