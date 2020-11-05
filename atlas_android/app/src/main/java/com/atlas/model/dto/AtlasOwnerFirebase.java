package com.atlas.model.dto;

public class AtlasOwnerFirebase {
    /* Holds the firebase token */
    private String firebaseToken;

    public AtlasOwnerFirebase(String firebaseToken) {
        this.firebaseToken = firebaseToken;
    }

    public String getFirebaseToken() {
        return this.firebaseToken;
    }

    public void setFirebaseToken(String firebaseToken) {
        this.firebaseToken = firebaseToken;
    }
}
