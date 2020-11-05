package com.atlas.utils;

import android.app.Application;
import android.content.Context;
import android.content.SharedPreferences;

public class AtlasSharedPreferences {
    private static AtlasSharedPreferences instance;
    private static Object lock = new Object();
    private SharedPreferences sharedPref;

    public static AtlasSharedPreferences getInstance(Context context) {
        if (instance == null) {
            synchronized (lock) {
                if (instance == null) {
                    instance = new AtlasSharedPreferences(context);
                }
            }
        }

        return instance;
    }

    public String getOwnerID() {
        return sharedPref.getString(AtlasConstants.ATLAS_SHARED_PREF_OWNER_ID, null);
    }

    public void saveOwnerID(String ownerID) {
        SharedPreferences.Editor editor = sharedPref.edit();
        editor.putString(AtlasConstants.ATLAS_SHARED_PREF_OWNER_ID, ownerID);
        editor.commit();
    }

    public String getFirebaseToken() {
        return sharedPref.getString(AtlasConstants.ATLAS_SHARED_PREF_FIREBASE_TOKEN, "");
    }

    public void saveFirebaseToken(String firebaseToken) {
        SharedPreferences.Editor editor = sharedPref.edit();
        editor.putString(AtlasConstants.ATLAS_SHARED_PREF_FIREBASE_TOKEN, firebaseToken);
        editor.commit();
    }

    private AtlasSharedPreferences(Context context) {
        sharedPref = context.getSharedPreferences(AtlasConstants.ATLAS_SHARED_PREF_NAME, Context.MODE_PRIVATE);
    }
}
