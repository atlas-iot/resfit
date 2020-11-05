package com.atlas.database;

import android.content.Context;

import androidx.room.Database;
import androidx.room.Room;
import androidx.room.RoomDatabase;

import com.atlas.database.dao.AtlasCommandDao;
import com.atlas.database.dao.AtlasClientDao;
import com.atlas.database.dao.AtlasGatewayDao;
import com.atlas.model.database.AtlasClient;
import com.atlas.model.database.AtlasCommand;
import com.atlas.model.database.AtlasGateway;

@Database(entities = {AtlasGateway.class, AtlasClient.class, AtlasCommand.class}, exportSchema = false, version = 1)
public abstract class AtlasDatabase extends RoomDatabase {
    private static final String DB_NAME = "ATLAS_DATABASE";
    private static AtlasDatabase instance;
    private static Object lock = new Object();

    public static AtlasDatabase getInstance(Context context) {
        if (instance == null) {
            synchronized (lock) {
                if (instance == null) {
                    instance = Room.databaseBuilder(context.getApplicationContext(),
                            AtlasDatabase.class,
                            DB_NAME)
                            .fallbackToDestructiveMigration()
                            .build();
                }
            }
        }
        return instance;
    }

    public abstract AtlasGatewayDao gatewayDao();

    public abstract AtlasClientDao clientDao();

    public abstract AtlasCommandDao commandDao();
}
