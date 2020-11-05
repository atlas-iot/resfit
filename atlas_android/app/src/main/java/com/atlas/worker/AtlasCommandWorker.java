package com.atlas.worker;

import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.TaskStackBuilder;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.core.app.NotificationCompat;
import androidx.core.app.NotificationManagerCompat;
import androidx.work.Worker;
import androidx.work.WorkerParameters;

import com.atlas.BuildConfig;
import com.atlas.R;
import com.atlas.database.AtlasDatabase;
import com.atlas.model.database.AtlasClient;
import com.atlas.model.database.AtlasCommand;
import com.atlas.model.database.AtlasGateway;
import com.atlas.model.dto.AtlasClientCommandsResp;
import com.atlas.networking.AtlasClientCommandAPI;
import com.atlas.networking.AtlasNetworkAPIFactory;
import com.atlas.ui.main.MainActivity;
import com.atlas.utils.AtlasConstants;
import com.atlas.utils.AtlasSharedPreferences;

import java.util.Collection;
import java.util.Date;
import java.util.List;
import java.util.Map;

import retrofit2.Response;

import static com.atlas.utils.AtlasConstants.ATLAS_CLIENT_COMMANDS_BROADCAST;

public class AtlasCommandWorker extends Worker {
    private Context context;

    public AtlasCommandWorker(@NonNull Context context, @NonNull WorkerParameters workerParams) {
        super(context, workerParams);
        this.context = context;
    }

    @NonNull
    @Override
    public Result doWork() {
        Log.i(AtlasCommandWorker.class.getName(), "Execute command worker");

        try {
            String ownerID = AtlasSharedPreferences.getInstance(getApplicationContext()).getOwnerID();
            Log.d(AtlasCommandWorker.class.getName(), "Get commands for owner " + ownerID);

            AtlasClientCommandAPI clientCommandAPI = AtlasNetworkAPIFactory.createClientCommandAPI(BuildConfig.ATLAS_CLOUD_BASE_URL + ":" + BuildConfig.ATLAS_CLOUD_PORT);
            Response<Map<String, List<AtlasClientCommandsResp>>> response = clientCommandAPI
                    .getClientCommands(ownerID)
                    .execute();

            if (!response.isSuccessful()) {
                Log.e(AtlasCommandWorker.class.getName(), "Command list fetch from cloud FAILED!");
                return Result.retry();
            }

            Map<String, List<AtlasClientCommandsResp>> commandList = response.body();
            if (commandList.isEmpty()) {
                return Result.success();
            }

            boolean newCommandAdded = false;
            for (String gateway : commandList.keySet()) {
                Log.d(AtlasCommandWorker.class.getName(), "Commands from gateway: " + gateway);

                /* If gateway does not exist locally (it was not claimed) drop the info */
                AtlasGateway gatewayEntity = AtlasDatabase.getInstance(getApplicationContext()).gatewayDao().selectByIdentity(gateway);
                if (gatewayEntity != null) {
                    newCommandAdded |= updateClientCommands(gatewayEntity, commandList.values());
                }
            };

            /* Notify UI about the client/commands change */
            getApplicationContext().sendBroadcast(new Intent(ATLAS_CLIENT_COMMANDS_BROADCAST));

            /* Show notification when at least one new command is fetched */
            if (newCommandAdded) {
                NotificationManager manager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);

                Intent resultIntent = new Intent(context, MainActivity.class);
                TaskStackBuilder stackBuilder = TaskStackBuilder.create(context);
                stackBuilder.addNextIntentWithParentStack(resultIntent);
                PendingIntent resultPendingIntent =
                        stackBuilder.getPendingIntent(0, PendingIntent.FLAG_UPDATE_CURRENT);

                NotificationCompat.Builder builder = new NotificationCompat.Builder(context, AtlasConstants.ATLAS_NOTIFICATION_CHANNEL_ID)
                        .setSmallIcon(R.drawable.atlas_app_icon)
                        .setContentTitle(context.getString(R.string.notification_title))
                        .setContentText(context.getString(R.string.notification_text))
                        .setContentIntent(resultPendingIntent)
                        .setPriority(NotificationCompat.PRIORITY_HIGH);
                manager.notify(0, builder.build());
            }

            return Result.success();

        } catch (Exception e) {
            e.printStackTrace();
        }

        return Result.retry();
    }

    private boolean updateClientCommands(AtlasGateway gateway, Collection<List<AtlasClientCommandsResp>> clientCommands) {
        boolean newCommandAdded = false;

        for (List<AtlasClientCommandsResp> commands : clientCommands) {
            Log.d(AtlasCommandWorker.class.getName(), "Number of commands is: " + commands.size());
            for (AtlasClientCommandsResp command : commands) {
                Log.d(AtlasCommandWorker.class.getName(),
                        "Processing command with sequence number " + command.getSeqNo() + " from client " + command.getClientIdentity());

                /* Verify if client exists. If not create client. */
                AtlasClient client = AtlasDatabase.getInstance(getApplicationContext()).clientDao().selectByIdentity(command.getClientIdentity());
                Long clientId;
                if (client == null) {
                    Log.d(AtlasCommandWorker.class.getName(), "Client does not exist. Create it.");
                    client = new AtlasClient();
                    client.setIdentity(command.getClientIdentity());
                    client.setAlias(command.getClientAlias());
                    client.setGatewayId(gateway.getId());
                    clientId = AtlasDatabase.getInstance(getApplicationContext()).clientDao().insertClient(client);
                } else {
                    clientId = client.getId();
                    /* If client alias changed, update it locally */
                    if (!client.getAlias().equalsIgnoreCase(command.getClientAlias())) {
                        client.setAlias(command.getClientAlias());
                        AtlasDatabase.getInstance(getApplicationContext()).clientDao().updateClient(client);
                    }
                }

                /* Verify if command exists. If not create command. */
                AtlasCommand cmd = AtlasDatabase.getInstance(getApplicationContext()).commandDao().selectBySeqNo(command.getSeqNo());
                if (cmd == null) {
                    Log.d(AtlasCommandWorker.class.getName(), "Command does not exist. Create it.");
                    cmd = new AtlasCommand();
                    cmd.setCreateTime(new Date().toString());
                    cmd.setSeqNo(command.getSeqNo());
                    cmd.setType(command.getType());
                    cmd.setClientId(clientId);
                    cmd.setPayload(command.getPayload());

                    AtlasDatabase.getInstance(getApplicationContext()).commandDao().insertCommand(cmd);
                    newCommandAdded = true;
                }
            }
        }

        return newCommandAdded;
    }
}
