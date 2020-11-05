package com.atlas.ui.home.viewmodel;

import android.app.Application;

import androidx.annotation.NonNull;
import androidx.lifecycle.AndroidViewModel;
import androidx.lifecycle.MutableLiveData;

import com.atlas.database.AtlasDatabase;
import com.atlas.model.database.AtlasGateway;

import java.util.List;
import java.util.concurrent.CompletableFuture;

public class HomeViewModel extends AndroidViewModel {
    private final MutableLiveData<Long> totalPendingCommands = new MutableLiveData<>(0L);

    public HomeViewModel(@NonNull Application application) {
        super(application);
        fetchTotalPendingCommands();
    }

    public void fetchTotalPendingCommands() {
        CompletableFuture.runAsync(() -> {
            List<AtlasGateway> gateways = AtlasDatabase.getInstance(getApplication()
                    .getApplicationContext())
                    .gatewayDao()
                    .selectAll();

            Long pendingCommands = gateways.stream().map((gateway) -> {
                return AtlasDatabase.getInstance(getApplication()
                        .getApplicationContext())
                        .gatewayDao()
                        .getPendingCommands(gateway.getIdentity());
            }).reduce(0L, Long::sum);

            totalPendingCommands.postValue(pendingCommands);
        });
    }

    public MutableLiveData<Long> getTotalPendingCommands() {
        return totalPendingCommands;
    }
}
