package com.atlas.ui.client_list.view;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.databinding.DataBindingUtil;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.Lifecycle;
import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProvider;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.SimpleItemAnimator;

import com.atlas.R;
import com.atlas.databinding.FragmentListClientsBinding;
import com.atlas.model.database.AtlasClient;
import com.atlas.ui.client_list.viewmodel.AtlasClientListViewModel;
import com.atlas.ui.gateway_list.view.BackStackFragment;
import com.atlas.ui.main.MainActivity;
;

import java.util.List;

import static com.atlas.utils.AtlasConstants.ATLAS_CLIENT_COMMANDS_BROADCAST;

public class AtlasClientListView extends BackStackFragment {

    private AtlasClientAdapter atlasClientAdapter;
    private FragmentListClientsBinding binding;
    private AtlasClientListViewModel viewModel;
    private BroadcastReceiver clientsReceiver;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {

        binding = DataBindingUtil.inflate(inflater, R.layout.fragment_list_clients, container, false);

        atlasClientAdapter = new AtlasClientAdapter(clientClickCallback);

        ((SimpleItemAnimator) binding.clientsView.getItemAnimator()).setSupportsChangeAnimations(false);
        binding.clientsView.setLayoutManager(new LinearLayoutManager(inflater.getContext()));
        binding.clientsView.setHasFixedSize(true);

        binding.clientsView.setAdapter(atlasClientAdapter);
        binding.setIsLoading(true);

        return binding.getRoot();
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);

        AtlasClientListViewModel.Factory factory = new AtlasClientListViewModel
                .Factory(getActivity()
                .getApplication(), getArguments().getString("gateway_identity"));
        viewModel = new ViewModelProvider(this, factory)
                .get(AtlasClientListViewModel.class);

        binding.setIsLoading(true);

        observeListViewModel(viewModel);
    }
    @Override
    public void onResume() {
        super.onResume();
        ((AppCompatActivity) getActivity()).getSupportActionBar().setTitle(getString(R.string.client_list_title));

        if (viewModel != null) {
            viewModel.fetchClients();
        }

        if (clientsReceiver == null) {
            clientsReceiver = new BroadcastReceiver() {
                @Override
                public void onReceive(Context context, Intent intent) {
                    if (intent.getAction().equalsIgnoreCase(ATLAS_CLIENT_COMMANDS_BROADCAST) && viewModel != null) {
                        viewModel.fetchClients();
                    }
                }
            };
            getContext().registerReceiver(clientsReceiver, new IntentFilter(ATLAS_CLIENT_COMMANDS_BROADCAST));
        }
    }

    @Override
    public void onPause() {
        super.onPause();

        if (clientsReceiver != null) {
            getContext().unregisterReceiver(clientsReceiver);
            clientsReceiver = null;
        }
    }

    private void observeListViewModel(AtlasClientListViewModel viewModel) {

        viewModel.getClientList().observe(getViewLifecycleOwner(), new Observer<List<AtlasClient>>() {
            @Override
            public void onChanged(List<AtlasClient> clients) {
                Log.d(AtlasClientListView.class.getName(), "Client list changed!");
                if (clients != null) {
                    binding.setIsLoading(false);
                    atlasClientAdapter.setClientList(clients);
                    for (AtlasClient cl : clients) {
                        Log.d(AtlasClientListView.class.getName(), "Client list changed!" + cl.getPendingCommands());
                    }
                }
            }
        });
    }

    public static AtlasClientListView getInstance(String gatewayIdentity) {
        Log.w(AtlasClientListView.class.getName(), "Get client list fragment for gateway:" + gatewayIdentity);
        AtlasClientListView fragment = new AtlasClientListView();
        Bundle args = new Bundle();
        args.putString("gateway_identity", gatewayIdentity);
        fragment.setArguments(args);

        return fragment;
    }

    public void replaceFragment(Fragment fragment) {
        getChildFragmentManager()
                .beginTransaction()
                .replace(((ViewGroup) getView()).getId(), fragment)
                .addToBackStack(null)
                .commit();
    }

    @Override
    public void onStop() {
        super.onStop();
        ((AppCompatActivity) getActivity()).getSupportActionBar().setTitle(getString(R.string.gateway_list_title));
    }

    private final ClientClickCallback clientClickCallback = new ClientClickCallback() {
        @Override
        public void onCLick(AtlasClient client) {
            Log.w(this.getClass().toString(), "Click on client with identity " + client.getIdentity());
            if (client.getPendingCommands() > 0) {
                if (getLifecycle().getCurrentState().isAtLeast(Lifecycle.State.STARTED)) {
                    ((MainActivity) getActivity()).openAtlasClientCommandListFragment(client);
                }
            } else {
                Toast toast = Toast.makeText(getContext(), getString(R.string.client_list_empty_client), Toast.LENGTH_SHORT);
                toast.setGravity(Gravity.CENTER, 0, 0);
                toast.show();
            }
        }
    };
}
