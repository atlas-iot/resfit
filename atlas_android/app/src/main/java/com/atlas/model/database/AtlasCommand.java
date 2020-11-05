package com.atlas.model.database;

import androidx.room.ColumnInfo;
import androidx.room.Entity;
import androidx.room.ForeignKey;
import androidx.room.Ignore;
import androidx.room.Index;
import androidx.room.PrimaryKey;

@Entity(indices = {@Index(value = {"command_sequence_number"},
        unique = true), @Index(value = {"command_client_id"})},
        foreignKeys = @ForeignKey(entity = AtlasClient.class,
                parentColumns = "client_id",
                childColumns = "command_client_id",
                onDelete = ForeignKey.CASCADE))
public class AtlasCommand {
    @PrimaryKey(autoGenerate = true)
    @ColumnInfo(name = "command_id")
    private Long id;

    @ColumnInfo(name = "command_type")
    private String type;

    @ColumnInfo(name = "command_payload")
    private String payload;

    @ColumnInfo(name = "command_sequence_number")
    private Long seqNo;

    @ColumnInfo(name = "command_create_time")
    private String createTime;

    @ColumnInfo(name = "command_client_id")
    private Long clientId;

    @Ignore
    private Boolean actionButtonDisplayed;

    @Ignore
    private Boolean actionButtonsEnabled;

    public Long getId() {
        return id;
    }

    public void setId(Long id) {
        this.id = id;
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

    public Long getClientId() {
        return clientId;
    }

    public void setClientId(Long clientId) {
        this.clientId = clientId;
    }

    public String getCreateTime() {
        return createTime;
    }

    public void setCreateTime(String createTime) {
        this.createTime = createTime;
    }

    @Ignore
    public Boolean getActionButtonDisplayed() {
        return actionButtonDisplayed;
    }

    @Ignore
    public void setActionButtonDisplayed(Boolean actionButtonDisplayed) {
        this.actionButtonDisplayed = actionButtonDisplayed;
    }

    @Ignore
    public Boolean getActionButtonsEnabled() {
        return actionButtonsEnabled;
    }

    @Ignore
    public void setActionButtonsEnabled(Boolean actionButtonsEnabled) {
        this.actionButtonsEnabled = actionButtonsEnabled;
    }
}
