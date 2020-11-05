package com.atlas.model.database;

import androidx.room.ColumnInfo;
import androidx.room.Entity;
import androidx.room.Ignore;
import androidx.room.Index;
import androidx.room.PrimaryKey;

@Entity(indices = {@Index(value = {"gateway_identity", "gateway_alias"},
        unique = true)})
public class AtlasGateway {
    @PrimaryKey(autoGenerate = true)
    @ColumnInfo(name = "gateway_id")
    private Long id;

    @ColumnInfo(name = "gateway_identity")
    private String identity;

    @ColumnInfo(name = "gateway_alias")
    private String alias;

    @ColumnInfo(name = "gateway_secret_key")
    private String secretKey;

    @ColumnInfo(name = "gateway_claim_time")
    private String claimTime;

    @Ignore
    private Long pendingCommands;

    public Long getId() {
        return id;
    }

    public String getIdentity() {
        return identity;
    }

    public String getAlias() {
        return alias;
    }

    public String getSecretKey() {
        return secretKey;
    }

    public void setId(Long id) {
        this.id = id;
    }

    public void setIdentity(String identity) {
        this.identity = identity;
    }

    public void setAlias(String alias) {
        this.alias = alias;
    }

    public void setSecretKey(String secretKey) {
        this.secretKey = secretKey;
    }

    public String getClaimTime() {
        return claimTime;
    }

    public void setClaimTime(String claimTime) {
        this.claimTime = claimTime;
    }

    public Long getPendingCommands() {
        return pendingCommands;
    }

    public void setPendingCommands(Long pendingCommands) {
        this.pendingCommands = pendingCommands;
    }

    @Override
    public boolean equals(Object gateway) {
        if (!(gateway instanceof AtlasGateway)) {
            return false;
        }

        AtlasGateway other = (AtlasGateway) gateway;
        if (!this.getIdentity().equalsIgnoreCase(other.getIdentity())) {
            return false;
        }
        if (!this.getAlias().equalsIgnoreCase(other.getAlias())) {
            return false;
        }
        if (!this.getSecretKey().equalsIgnoreCase(other.getSecretKey())) {
            return false;
        }
        if (this.getId() != other.getId()) {
            return false;
        }
        if (this.getPendingCommands() != other.getPendingCommands()) {
            return false;
        }

        return true;
    }
}