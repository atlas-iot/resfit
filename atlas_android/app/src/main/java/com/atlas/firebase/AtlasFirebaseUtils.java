package com.atlas.firebase;

import android.content.Context;
import android.os.AsyncTask;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.atlas.BuildConfig;
import com.atlas.model.dto.AtlasOwnerFirebase;
import com.atlas.networking.AtlasFirebaseAPI;
import com.atlas.networking.AtlasNetworkAPIFactory;
import com.atlas.ui.main.MainActivity;
import com.atlas.utils.AtlasSharedPreferences;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.iid.FirebaseInstanceId;
import com.google.firebase.iid.InstanceIdResult;

import org.jetbrains.annotations.NotNull;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class AtlasFirebaseUtils {
    public static Boolean updateFirebaseTokenToCloud(@NotNull Context context) {
        String firebaseToken = AtlasSharedPreferences.getInstance(context).getFirebaseToken();
        Log.d(AtlasFirebaseUtils.class.getName(), "Send Firebase token to cloud: " + firebaseToken);

        try {
            String ownerID = AtlasSharedPreferences.getInstance(context).getOwnerID();
            AtlasOwnerFirebase ownerFirebase = new AtlasOwnerFirebase(firebaseToken);
            /* Update firebase token to cloud service */
            AtlasFirebaseAPI firebaseAPI = AtlasNetworkAPIFactory.createFirebaseAPI(BuildConfig.ATLAS_CLOUD_BASE_URL + ":" + BuildConfig.ATLAS_CLOUD_PORT);
            Response<Void> response = firebaseAPI.updateFirebaseToken(ownerID, ownerFirebase).execute();

            if (response.isSuccessful()) {
                Log.i(AtlasFirebaseUtils.class.getName(), "Firebase token updated successfully to cloud");
                return true;
            } else {
                Log.e(AtlasFirebaseUtils.class.getName(), "An error occurred while updating the firebase token to cloud. HTTP status: " + response.code());
                return false;
            }
        } catch (Exception e) {
            Log.e(AtlasFirebaseUtils.class.getName(), "Exception occurred when updating the firebase token: " + e.getMessage());
        }

        return false;
    }
}
