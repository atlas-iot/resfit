package com.atlas.ui.gateway_list.view;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.databinding.DataBindingUtil;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.lifecycle.Lifecycle;
import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProvider;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.SimpleItemAnimator;

import com.atlas.R;
import com.atlas.databinding.FragmentListGatewaysBinding;
import com.atlas.model.database.AtlasGateway;
import com.atlas.ui.command_list.view.AtlasCommandListView;
import com.atlas.ui.gateway_list.viewmodel.AtlasGatewayListViewModel;
import com.atlas.ui.main.MainActivity;

import java.util.List;

import static com.atlas.utils.AtlasConstants.ATLAS_CLIENT_COMMANDS_BROADCAST;

public class AtlasGatewayListView extends BackStackFragment {

    private AtlasGatewayAdapter atlasGatewayAdapter;
    private FragmentListGatewaysBinding binding;
    private AtlasGatewayListViewModel viewModel;
    private BroadcastReceiver gatewaysReceiver;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {

        binding = DataBindingUtil.inflate(inflater, R.layout.fragment_list_gateways, container, false);

        atlasGatewayAdapter = new AtlasGatewayAdapter(gatewayClickCallback);

        ((SimpleItemAnimator) binding.gatewaysView.getItemAnimator()).setSupportsChangeAnimations(false);
        binding.gatewaysView.setLayoutManager(new LinearLayoutManager(inflater.getContext()));
        binding.gatewaysView.setHasFixedSize(true);

        binding.gatewaysView.setAdapter(atlasGatewayAdapter);
        binding.setIsLoading(true);

        return binding.getRoot();
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);

        viewModel = new ViewModelProvider(this).get(AtlasGatewayListViewModel.class);
        observeListViewModel();
    }

    @Override
    public void onResume() {
        super.onResume();
        ((AppCompatActivity) getActivity()).getSupportActionBar().setTitle(getString(R.string.gateway_list_title));

        if (viewModel != null) {
            viewModel.fetchGatewayList();
        }

        if (gatewaysReceiver == null) {
            gatewaysReceiver = new BroadcastReceiver() {
                @Override
                public void onReceive(Context context, Intent intent) {
                    if (intent.getAction().equalsIgnoreCase(ATLAS_CLIENT_COMMANDS_BROADCAST) && viewModel != null) {
                        viewModel.fetchGatewayList();
                    }
                }
            };
            getContext().registerReceiver(gatewaysReceiver, new IntentFilter(ATLAS_CLIENT_COMMANDS_BROADCAST));
        }
    }

    @Override
    public void onPause() {
        super.onPause();

        if (gatewaysReceiver != null) {
            getContext().unregisterReceiver(gatewaysReceiver);
            gatewaysReceiver = null;
        }
    }

    private void observeListViewModel() {
        viewModel.getGatewayList().observe(getViewLifecycleOwner(), new Observer<List<AtlasGateway>>() {
            @Override
            public void onChanged(List<AtlasGateway> atlasGateways) {
                Log.d(this.getClass().toString(), "Gateway list changed!");
                if (atlasGateways != null) {
                    binding.setIsLoading(false);
                    atlasGatewayAdapter.setGatewayList(atlasGateways);
                }
            }
        });
        viewModel.getGatewayDeleteStatus().observe(getViewLifecycleOwner(), new Observer<Boolean>() {
            @Override
            public void onChanged(Boolean gatewayDeleteStatus) {
                Log.d(AtlasCommandListView.class.getName(), "Delete gateway status changed!");

                Toast toast;
                if (gatewayDeleteStatus) {
                    toast = Toast.makeText(getActivity(), "Gateway has been deleted successfully!", Toast.LENGTH_SHORT);
                } else {
                    toast = Toast.makeText(getActivity(), "An error occurred while trying to delete gateway!", Toast.LENGTH_SHORT);
                }

                toast.setGravity(Gravity.CENTER, 0, 0);
                toast.show();
            }
        });
    }

    public void replaceFragment(Fragment fragment) {
        getChildFragmentManager()
                .beginTransaction()
                .replace(((ViewGroup) getView()).getId(), fragment)
                .addToBackStack(null)
                .commit();
    }

    private final GatewayClickCallback gatewayClickCallback = new GatewayClickCallback() {
        @Override
        public boolean onLongClick(AtlasGateway gateway) {
            Log.w(this.getClass().toString(), "Long click on gateway with identity: " + gateway.getIdentity());
            new AlertDialog.Builder(getContext())
                    .setTitle("Delete gateway")
                    .setMessage("Are you sure you want to delete this gateway?")
                    .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            viewModel.deleteGateway(gateway);
                        }
                    })
                    .setNegativeButton("No", null)
                    .setIcon(R.drawable.ic_baseline_warning_24)
                    .show();

            return true;
        }

        @Override
        public void onClick(AtlasGateway gateway) {
            Log.w(this.getClass().toString(), "Click on gateway with identity: " + gateway.getIdentity());
            if (gateway.getPendingCommands() > 0) {
                if (gateway.getPendingCommands() > 0 && getLifecycle().getCurrentState().isAtLeast(Lifecycle.State.STARTED)) {
                    ((MainActivity) getActivity()).openAtlasClientListFragment(gateway);
                }
            } else {
                Toast toast = Toast.makeText(getContext(), getString(R.string.gateway_list_empty_gateway), Toast.LENGTH_SHORT);
                toast.setGravity(Gravity.CENTER, 0, 0);
                toast.show();
            }
        }
    };
}
