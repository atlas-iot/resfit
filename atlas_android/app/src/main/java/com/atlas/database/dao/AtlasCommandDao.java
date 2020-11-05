package com.atlas.database.dao;

import androidx.room.Dao;
import androidx.room.Delete;
import androidx.room.Insert;
import androidx.room.Query;
import androidx.room.Update;

import com.atlas.model.database.AtlasCommand;

import java.util.List;

@Dao
public interface AtlasCommandDao {
    @Query("SELECT * FROM AtlasCommand")
    List<AtlasCommand> selectAll();

    @Query("SELECT * FROM AtlasCommand WHERE command_sequence_number = :seqNo")
    AtlasCommand selectBySeqNo(Long seqNo);

    @Query("SELECT * FROM AtlasCommand INNER JOIN AtlasClient ON AtlasCommand.command_client_id = AtlasClient.client_id WHERE AtlasClient.client_identity = :clientIdentity")
    List<AtlasCommand> selectByClientIdentity(String clientIdentity);

    @Insert
    void insertCommand(AtlasCommand clientCommand);

    @Update
    void updateCommand(AtlasCommand clientCommand);

    @Delete
    void deleteCommand(AtlasCommand clientCommand);

    @Query("DELETE FROM AtlasCommand")
    void deleteAll();
}
