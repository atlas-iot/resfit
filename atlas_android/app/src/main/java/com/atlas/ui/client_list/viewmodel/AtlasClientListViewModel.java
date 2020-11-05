package com.atlas.ui.client_list.viewmodel;

import android.app.Application;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.lifecycle.AndroidViewModel;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;
import androidx.lifecycle.ViewModelProvider;

import com.atlas.database.AtlasDatabase;
import com.atlas.model.database.AtlasClient;

import java.util.List;
import java.util.concurrent.CompletableFuture;

public class AtlasClientListViewModel extends AndroidViewModel {

    private final MutableLiveData<List<AtlasClient>> clientList;
    private final String gatewayIdentity;

    public AtlasClientListViewModel(@NonNull Application application, String gatewayIdentity) {
        super(application);

        clientList = new MutableLiveData<>();
        this.gatewayIdentity = gatewayIdentity;

        fetchClients();
    }

    public void fetchClients() {
        CompletableFuture.runAsync(() -> {
            Log.d(AtlasClientListViewModel.class.getName(), "Fetch clients from database");

            List<AtlasClient> clients = AtlasDatabase.getInstance(getApplication()).clientDao().selectByGatewayIdentity(gatewayIdentity);
            clients.forEach((client) -> {
                client.setPendingCommands(AtlasDatabase.getInstance(getApplication()).clientDao().getPendingCommands(client.getIdentity()));
            });

            clientList.postValue(clients);
        });
    }

    public MutableLiveData<List<AtlasClient>> getClientList() {
        return clientList;
    }

    public static class Factory extends ViewModelProvider.AndroidViewModelFactory {
        @NonNull
        Application application;
        private final String gatewayIdentity;

        public Factory(@NonNull Application application, String gatewayIdentity) {
            super(application);
            this.application = application;
            this.gatewayIdentity = gatewayIdentity;
        }

        @NonNull
        @Override
        public <T extends ViewModel> T create(@NonNull Class<T> modelClass) {
            return (T) new AtlasClientListViewModel(application, gatewayIdentity);
        }
    }
}
