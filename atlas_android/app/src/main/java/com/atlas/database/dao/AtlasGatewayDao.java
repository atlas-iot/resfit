package com.atlas.database.dao;

import androidx.room.Dao;
import androidx.room.Delete;
import androidx.room.Insert;
import androidx.room.Query;
import androidx.room.Update;

import com.atlas.model.database.AtlasGateway;

import java.util.List;

@Dao
public interface AtlasGatewayDao {
    @Query("SELECT * FROM AtlasGateway")
    List<AtlasGateway> selectAll();

    @Query("SELECT * FROM AtlasGateway WHERE gateway_alias = :gatewayAlias")
    AtlasGateway selectByAlias(String gatewayAlias);

    @Query("SELECT * FROM AtlasGateway WHERE gateway_identity = :gatewayIdentity")
    AtlasGateway selectByIdentity(String gatewayIdentity);

    @Query("SELECT COUNT(*) FROM AtlasGateway INNER JOIN AtlasClient ON AtlasGateway.gateway_id = AtlasClient.client_gateway_id INNER JOIN AtlasCommand ON AtlasClient.client_id = AtlasCommand.command_client_id WHERE AtlasGateway.gateway_identity = :gatewayIdentity")
    Long getPendingCommands(String gatewayIdentity);

    @Insert
    void insertGateway(AtlasGateway gateway);

    @Update
    void updateGateway(AtlasGateway gateway);

    @Delete
    void deleteGateway(AtlasGateway gateway);

    @Query("DELETE FROM AtlasGateway")
    void deleteAll();
}
