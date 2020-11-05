package com.atlas.model.dto;

public class AtlasGatewayClaimReq {
    /* Short code */
    private String shortCode;
    /* Secret key */
    private String secretKey;
    /* Owner identity */
    private String ownerIdentity;

    public AtlasGatewayClaimReq(String shortCode, String secretKey, String ownerIdentity) {
        this.shortCode = shortCode;
        this.secretKey = secretKey;
        this.ownerIdentity = ownerIdentity;
    }

    public String getShortCode() {
        return shortCode;
    }

    public String getSecretKey() {
        return secretKey;
    }

    public String getOwnerIdentity() {
        return ownerIdentity;
    }

    void setShortCode(String shortCode) {
        this.shortCode = shortCode;
    }

    void setSecretKey(String secretKey) {
        this.secretKey = secretKey;
    }

    void setOwnerIdentity(String ownerIdentity) {
        this.ownerIdentity = ownerIdentity;
    }
}
