package com.atlas.ui.command_list.viewmodel;

import android.app.Application;
import android.content.Intent;
import java.util.Base64;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.lifecycle.AndroidViewModel;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;
import androidx.lifecycle.ViewModelProvider;

import com.atlas.BuildConfig;
import com.atlas.database.AtlasDatabase;
import com.atlas.model.database.AtlasCommand;
import com.atlas.model.database.AtlasGateway;
import com.atlas.model.dto.AtlasOwnerCommandReq;
import com.atlas.networking.AtlasClientCommandAPI;
import com.atlas.networking.AtlasNetworkAPIFactory;
import com.atlas.utils.AtlasSharedPreferences;
import com.atlas.utils.Crypto;

import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.util.List;
import java.util.concurrent.CompletableFuture;

import okhttp3.ResponseBody;
import retrofit2.Response;

import static com.atlas.utils.AtlasConstants.ATLAS_CLIENT_COMMANDS_BROADCAST;
import static com.atlas.utils.AtlasConstants.ATLAS_HMAC_ALGO;

public class AtlasCommandListViewModel extends AndroidViewModel {

    private MutableLiveData<List<AtlasCommand>> commandList;
    private MutableLiveData<Boolean> commandStatus;
    private final String clientIdentity;

    public AtlasCommandListViewModel(@NonNull Application application, String clientIdentity) {
        super(application);

        this.clientIdentity = clientIdentity;
        this.commandList = new MutableLiveData<>();
        this.commandStatus = new MutableLiveData<>();

        fetchCommands();
    }

    public void fetchCommands() {
        CompletableFuture.runAsync(() -> {
            Log.d(AtlasCommandListViewModel.class.getName(), "Fetch commands from database");
            commandList.postValue(AtlasDatabase.getInstance(getApplication()).commandDao().selectByClientIdentity(clientIdentity));
        });
    }

    public MutableLiveData<List<AtlasCommand>> getCommandList() {
        return commandList;
    }

    public MutableLiveData<Boolean> getCommandStatus() {
        return commandStatus;
    }

    public void sendCommandStatus(AtlasCommand command, Boolean approveCommand) throws Exception {
        new CompletableFuture<Boolean>().supplyAsync(() -> {
            try {
                return sendOwnerCommandRequest(approveCommand, command);
            } catch (Exception e) {
                e.printStackTrace();
            }
            return false;
        }).thenApply(result -> {
            commandStatus.postValue(result);
            return result;
        });
    }

    private String getCommandSignature(AtlasGateway gateway, AtlasCommand command) {
        /* Get gateway secret key */
        byte[] gatewaySecretKey = Base64.getDecoder().decode(gateway.getSecretKey());
        if (gatewaySecretKey == null || gatewaySecretKey.length == 0) {
            return null;
        }

        /* HMAC SHA512: (gatewayId + clientId + clientCommandType + clientCommandPayload + seqNo) */
        String hmacPayload = gateway.getIdentity() + clientIdentity + command.getType() + command.getPayload() + command.getSeqNo();

        try {
            byte[] hmacResult = Crypto.computeHMAC(ATLAS_HMAC_ALGO, gatewaySecretKey, hmacPayload.getBytes());
            Log.d(AtlasCommandListViewModel.class.getName(), "HMAC res:" + hmacResult.length);

            return Base64.getEncoder().encodeToString(hmacResult);
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        } catch (InvalidKeyException e) {
            e.printStackTrace();
        }

        return null;
    }

    private boolean sendOwnerCommandRequest(boolean commandStatus, AtlasCommand command) {
        try {
            AtlasGateway gateway = AtlasDatabase.getInstance(getApplication()).clientDao().selectGatewayByClientIdentity(clientIdentity);
            String ownerID = AtlasSharedPreferences.getInstance(getApplication()).getOwnerID();
            String signature = getCommandSignature(gateway, command);
            if (signature == null) {
                Log.d(AtlasCommandListViewModel.class.getName(), "Cannot compute command signature!");
                return false;
            }

            Log.d(AtlasCommandListViewModel.class.getName(), "Approve command seq.nr. " + String.valueOf(command.getSeqNo()) + " for " + ownerID + " with signature:" + signature);

            AtlasOwnerCommandReq ownerCommandReq = new AtlasOwnerCommandReq(gateway.getIdentity(), clientIdentity, command.getSeqNo().intValue(), commandStatus, signature);
            AtlasClientCommandAPI clientCommandAPI = AtlasNetworkAPIFactory.createClientCommandAPI(BuildConfig.ATLAS_CLOUD_BASE_URL + ":" + BuildConfig.ATLAS_CLOUD_PORT);
            Response<ResponseBody> response = clientCommandAPI.sendCommandStatus(ownerID, ownerCommandReq).execute();

            if (!response.isSuccessful()) {
                Log.e(AtlasCommandListViewModel.class.getName(), "Owner command update FAILED! Response code is: " + response.code());
                return false;
            }

            /* Delete command from database */
            AtlasDatabase.getInstance(getApplication()).commandDao().deleteCommand(command);
            Log.d(AtlasCommandListViewModel.class.getName(), "Command has been DELETED from the local database!");

            /* Notify UI about the client/commands change */
            getApplication().getApplicationContext().sendBroadcast(new Intent(ATLAS_CLIENT_COMMANDS_BROADCAST));

            return true;
        } catch (Exception e) {
            e.printStackTrace();
        }

        return false;
    }

    public static class Factory extends ViewModelProvider.AndroidViewModelFactory {

        @NonNull
        Application application;
        private List<AtlasCommand> commandList;
        private String clientIdentity;

        public Factory(@NonNull Application application, String clientIdentity) {
            super(application);
            this.application = application;
            this.clientIdentity = clientIdentity;
        }

        @NonNull
        @Override
        public <T extends ViewModel> T create(@NonNull Class<T> modelClass) {
            return (T) new AtlasCommandListViewModel(application, clientIdentity);
        }
    }
}
