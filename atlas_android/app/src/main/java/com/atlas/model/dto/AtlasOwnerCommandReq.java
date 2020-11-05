package com.atlas.model.dto;

public class AtlasOwnerCommandReq {
    /* Holds the gateway identity */
    private String gatewayIdentity;
    /* Holds the client identity */
    private String clientIdentity;
    /* Holds the command sequence number */
    private int seqNo;
    /* Indicates if the command is approved or rejected */
    private boolean approved;
    /* Holds the command signature */
    private String signature;

    public AtlasOwnerCommandReq(String gatewayIdentity, String clientIdentity, int seqNo, boolean approved, String signature) {
        this.gatewayIdentity = gatewayIdentity;
        this.clientIdentity = clientIdentity;
        this.seqNo = seqNo;
        this.approved = approved;
        this.signature = signature;
    }

    public String getGatewayIdentity() {
        return gatewayIdentity;
    }

    public void setGatewayIdentity(String gatewayIdentity) {
        this.gatewayIdentity = gatewayIdentity;
    }

    public String getClientIdentity() {
        return clientIdentity;
    }

    public void setClientIdentity(String clientIdentity) {
        this.clientIdentity = clientIdentity;
    }

    public int getSeqNo() {
        return seqNo;
    }

    public void setSeqNo(int seqNo) {
        this.seqNo = seqNo;
    }

    public boolean isApproved() {
        return approved;
    }

    public void setApproved(boolean approved) {
        this.approved = approved;
    }

    public String getSignature() {
        return signature;
    }

    public void setSignature(String signature) {
        this.signature = signature;
    }
}
