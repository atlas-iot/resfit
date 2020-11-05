package com.atlas.worker;

import android.content.Context;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.work.Worker;
import androidx.work.WorkerParameters;

import com.atlas.firebase.AtlasFirebaseUtils;

import io.reactivex.Scheduler;

public class AtlasFirebaseWorker extends Worker {

    public AtlasFirebaseWorker(@NonNull Context context, @NonNull WorkerParameters workerParams) {
        super(context, workerParams);
    }

    @NonNull
    @Override
    public Result doWork() {
        Log.i(AtlasFirebaseWorker.class.getName(), "Start firebase token update work " + Thread.currentThread().getName());

        boolean result = AtlasFirebaseUtils.updateFirebaseTokenToCloud(getApplicationContext());

        return result ? Result.success() : Result.retry();
    }
}
