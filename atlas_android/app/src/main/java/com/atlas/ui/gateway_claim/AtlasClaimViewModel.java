package com.atlas.ui.gateway_claim;

import android.app.Application;

import java.security.NoSuchAlgorithmException;
import java.util.Base64;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.lifecycle.AndroidViewModel;
import androidx.lifecycle.MutableLiveData;

import com.atlas.BuildConfig;
import com.atlas.database.AtlasDatabase;
import com.atlas.model.database.AtlasGateway;
import com.atlas.model.dto.AtlasGatewayClaimReq;
import com.atlas.model.dto.AtlasGatewayClaimResp;
import com.atlas.networking.AtlasGatewayClaimAPI;
import com.atlas.networking.AtlasNetworkAPIFactory;
import com.atlas.utils.AtlasConstants;
import com.atlas.utils.AtlasSharedPreferences;

import java.security.SecureRandom;
import java.util.Date;
import java.util.concurrent.CompletableFuture;

import javax.crypto.KeyGenerator;
import javax.crypto.SecretKey;

import retrofit2.Response;

import static com.atlas.utils.AtlasConstants.ATLAS_GATEWAY_SECRET_KEY_SIZE_BITS;

public class AtlasClaimViewModel extends AndroidViewModel {
    /* Indicates if the gateway alias is valid */
    private MutableLiveData<Boolean> aliasLiveData = new MutableLiveData<>();

    /* Indicates if the gateway was claimed */
    private MutableLiveData<Boolean> claimedLiveData = new MutableLiveData<>();

    public AtlasClaimViewModel(@NonNull Application application) {
        super(application);
    }

    public final MutableLiveData<Boolean> getAliasLiveData() {
        return aliasLiveData;
    }

    public final MutableLiveData<Boolean> getClaimedLiveData() {
        return claimedLiveData;
    }

    public void validateAlias(String alias) {
        Log.d(AtlasClaimViewModel.class.getName(), "Validate alias");

        CompletableFuture.runAsync(() -> {
            AtlasGateway gatewayEntity = AtlasDatabase.getInstance(getApplication()
                    .getApplicationContext())
                    .gatewayDao()
                    .selectByAlias(alias);

            Log.d(AtlasClaimViewModel.class.getName(), "Send alias data validity");
            aliasLiveData.postValue(true ? gatewayEntity == null : false);
        });
    }

    private String generateSecretKey() throws NoSuchAlgorithmException {
        /* Generate gateway secret key */
        SecureRandom secureRandom = new SecureRandom();
        KeyGenerator keyGenerator = KeyGenerator.getInstance("AES");
        keyGenerator.init(ATLAS_GATEWAY_SECRET_KEY_SIZE_BITS, secureRandom);
        SecretKey secretKey = KeyGenerator.getInstance("AES").generateKey();

        return Base64.getEncoder().encodeToString(secretKey.getEncoded());
    }

    public void claimGateway(final String gatewayHostnameValue, final String shortCode, final String alias) {
        Log.d(AtlasClaimViewModel.class.getName(), "Start gateway claim process!");

        new CompletableFuture<Boolean>().supplyAsync(() -> {
            String ownerID = AtlasSharedPreferences.getInstance(getApplication()).getOwnerID();
            final String url = AtlasConstants.ATLAS_GATEWAY_HTTPS_SCHEMA + gatewayHostnameValue + ":" + BuildConfig.ATLAS_GATEWAY_CLAIM_PORT + "/";
            Log.d(AtlasClaimViewModel.class.getName(), "Execute claim request to URL:" + url);

            try {
                /* Execute REST API request to gateway */
                AtlasGatewayClaimAPI gatewayClaimAPI = AtlasNetworkAPIFactory.createGatewayClaimAPI(url);

                String secretKey = generateSecretKey();
                AtlasGatewayClaimReq claimReq = new AtlasGatewayClaimReq(shortCode, secretKey, ownerID);
                Response<AtlasGatewayClaimResp> claimResp = gatewayClaimAPI.claimGateway(claimReq).execute();
                if (!claimResp.isSuccessful()) {
                    Log.e(AtlasClaimViewModel.class.getName(), "Gateway claim REST API is not successful");
                    return false;
                }

                /* Insert gateway into database */
                AtlasGateway gateway = new AtlasGateway();
                gateway.setIdentity(claimResp.body().getIdentity());
                gateway.setAlias(alias);
                gateway.setSecretKey(secretKey);
                gateway.setClaimTime(new Date().toString());

                // TODO if gateway exists, secret key should be updated
                AtlasDatabase.getInstance(getApplication().getApplicationContext()).gatewayDao().insertGateway(gateway);

                Log.i(AtlasClaimViewModel.class.getName(),
                        String.format("Gateway claim REST API is successful. Gateway alias is %s and gateway identity is %s", alias, claimResp.body().getIdentity()));

                return true;
            } catch (Exception e) {
                Log.e(AtlasClaimViewModel.class.getName(), "Claim request exception: " + e.getMessage());
                e.printStackTrace();
            }

            return false;
        }).thenAccept(claimStatus -> claimedLiveData.postValue(claimStatus));
    }
}
