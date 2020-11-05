package com.atlas.utils;

public interface AtlasConstants {
    /* Shared preferences constants */
    public static final String ATLAS_SHARED_PREF_NAME = "ATLAS.SHARED_PREFERENCES";
    public static final String ATLAS_SHARED_PREF_OWNER_ID = "ATLAS.SHARED_PREFERENCES.OWNER_ID";
    public static final String ATLAS_SHARED_PREF_FIREBASE_TOKEN = "ATLAS.SHARED_PREFERENCES.FIREBASE_TOKEN";

    /* Command approval HMAC signature */
    public static final String ATLAS_HMAC_ALGO = "HmacSHA512";
    public static int ATLAS_GATEWAY_SECRET_KEY_SIZE_BITS = 256;

    public static final String ATLAS_GATEWAY_HTTPS_SCHEMA = "https://";

    /* Broadcast receiver */
    public static final String ATLAS_CLIENT_COMMANDS_BROADCAST = "ATLAS.CLIENT_COMMANDS_BROADCAST";

    /* Time interval to execute command worker task (fetch pending commands from cloud) */
    public static final int ATLAS_COMMAND_WORKER_TIME_MIN = 15;

    /* Time interval in which splash screen is displayed */
    public static final int ATLAS_SPLASH_DISPLAY_TIME = 1000;

    /* Notifications */
    public static final String ATLAS_NOTIFICATION_CHANNEL_ID = "ATLAS_NOTIFICATION_CHANNEL_ID";
    public static final String ATLAS_NOTIFICATION_CHANNEL_NAME = "ATLAS_NOTIFICATION_CHANNEL_NAME";
}
