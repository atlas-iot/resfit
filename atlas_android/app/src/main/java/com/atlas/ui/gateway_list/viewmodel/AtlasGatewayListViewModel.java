package com.atlas.ui.gateway_list.viewmodel;

import android.app.Application;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.lifecycle.AndroidViewModel;
import androidx.lifecycle.MutableLiveData;

import com.atlas.database.AtlasDatabase;
import com.atlas.model.database.AtlasGateway;

import java.util.List;
import java.util.concurrent.CompletableFuture;


public class AtlasGatewayListViewModel extends AndroidViewModel {

    private final MutableLiveData<List<AtlasGateway>> gatewayList = new MutableLiveData<>();
    private final MutableLiveData<Boolean> gatewayDeleteStatus = new MutableLiveData<>();

    public AtlasGatewayListViewModel(@NonNull Application application) {
        super(application);
        fetchGatewayList();
    }

    public void fetchGatewayList() {
        CompletableFuture.runAsync(() -> {
            List<AtlasGateway> gateways = AtlasDatabase.getInstance(getApplication()
                    .getApplicationContext())
                    .gatewayDao()
                    .selectAll();

            gateways.forEach((gateway) -> {
                gateway.setPendingCommands(AtlasDatabase.getInstance(getApplication()
                        .getApplicationContext())
                        .gatewayDao()
                        .getPendingCommands(gateway.getIdentity()));
            });

            gatewayList.postValue(gateways);
        });
    }

    public void deleteGateway(AtlasGateway gateway) {
        Log.w(AtlasGatewayListViewModel.class.toString(), "Gateway with identity:" + gateway.getIdentity() + " is being deleted from the db!");

        new CompletableFuture<Boolean>().supplyAsync(() -> {
            try {
                AtlasDatabase.getInstance(getApplication()
                        .getApplicationContext())
                        .gatewayDao()
                        .deleteGateway(gateway);

                fetchGatewayList();
                return true;
            } catch (Exception e) {
                e.printStackTrace();
            }

            return false;
        }).thenAccept(result -> gatewayDeleteStatus.postValue(result));
    }

    public MutableLiveData<List<AtlasGateway>> getGatewayList() {
        return gatewayList;
    }

    public MutableLiveData<Boolean> getGatewayDeleteStatus() {
        return gatewayDeleteStatus;
    }
}
