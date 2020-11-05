package com.atlas.model.dto;

public class AtlasClientCommandsResp {

    /* Client identity */
    private String clientIdentity;

    /* Client alias */
    private String clientAlias;

    /* Command type */
    private String type;

    /* Command payload */
    private String payload;

    /* Command sequence number */
    private Long seqNo;

    public AtlasClientCommandsResp(String clientIdentity, String clientAlias, String type, String payload, Long seqNo) {
        this.clientIdentity = clientIdentity;
        this.clientAlias = clientAlias;
        this.type = type;
        this.payload = payload;
        this.seqNo = seqNo;
    }

    public String getClientIdentity() {
        return clientIdentity;
    }

    public void setClientIdentity(String clientIdentity) {
        this.clientIdentity = clientIdentity;
    }

    public String getType() {
        return type;
    }

    public void setType(String type) {
        this.type = type;
    }

    public String getPayload() {
        return payload;
    }

    public void setPayload(String payload) {
        this.payload = payload;
    }

    public Long getSeqNo() {
        return seqNo;
    }

    public void setSeqNo(Long seqNo) {
        this.seqNo = seqNo;
    }

    public String getClientAlias() { return clientAlias; }

    public void setClientAlias(String clientAlias) { this.clientAlias = clientAlias; }
}
