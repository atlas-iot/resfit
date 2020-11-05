package com.atlas.model.database;

import androidx.room.ColumnInfo;
import androidx.room.Entity;
import androidx.room.ForeignKey;
import androidx.room.Ignore;
import androidx.room.Index;
import androidx.room.PrimaryKey;

@Entity(indices = {@Index(value = {"client_identity"},
        unique = true)},
        foreignKeys = @ForeignKey(entity = AtlasGateway.class,
        parentColumns = "gateway_id",
        childColumns = "client_gateway_id",
        onDelete = ForeignKey.CASCADE))
public class AtlasClient {
    @PrimaryKey(autoGenerate = true)
    @ColumnInfo(name = "client_id")
    private Long id;

    @ColumnInfo(name = "client_identity")
    private String identity;

    @ColumnInfo(name = "client_alias")
    private String alias;

    @ColumnInfo(name = "client_gateway_id")
    private Long gatewayId;

    @Ignore
    private Long pendingCommands;

    public Long getId() {
        return id;
    }

    public String getIdentity() {
        return identity;
    }

    public void setId(Long id) {
        this.id = id;
    }

    public void setIdentity(String identity) {
        this.identity = identity;
    }

    public Long getGatewayId() {
        return gatewayId;
    }

    public void setGatewayId(Long gatewayId) {
        this.gatewayId = gatewayId;
    }

    public String getAlias() {
        return alias;
    }

    public void setAlias(String alias) {
        this.alias = alias;
    }

    public Long getPendingCommands() {
        return pendingCommands;
    }

    public void setPendingCommands(Long pendingCommands) {
        this.pendingCommands = pendingCommands;
    }

    public boolean equals(Object client) {
        if (!(client instanceof  AtlasClient)) {
            return false;
        }

        AtlasClient other = (AtlasClient) client;
        if (!this.getIdentity().equalsIgnoreCase(other.getIdentity())) {
            return false;
        }
        if (!this.getAlias().equalsIgnoreCase(other.getAlias())) {
            return false;
        }
        if (this.getPendingCommands() != other.getPendingCommands()) {
            return false;
        }
        if (this.getId() != other.getId()) {
            return false;
        }
        if (this.getGatewayId() != other.getGatewayId()) {
            return false;
        }

        return true;
    }
}
