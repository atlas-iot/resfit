package com.atlas.ui.home.view;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.appcompat.app.AppCompatActivity;
import androidx.databinding.DataBindingUtil;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProvider;

import com.atlas.R;
import com.atlas.databinding.HomeBinding;
import com.atlas.ui.home.viewmodel.HomeViewModel;
import com.google.android.material.tabs.TabLayout;

import static com.atlas.utils.AtlasConstants.ATLAS_CLIENT_COMMANDS_BROADCAST;

public class Home extends Fragment {

    private HomeBinding binding;
    private HomeViewModel viewModel;
    private BroadcastReceiver commandsReceiver;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {

        binding = DataBindingUtil.inflate(inflater, R.layout.home, container, false);
        binding.setCallback(callback);
        viewModel = new ViewModelProvider(this).get(HomeViewModel.class);

        observeListViewModel();

        return binding.getRoot();
    }

    @Override
    public void onResume() {
        super.onResume();
        ((AppCompatActivity) getActivity()).getSupportActionBar().setTitle("Home");

        if (viewModel != null) {
            viewModel.fetchTotalPendingCommands();
        }

        if (commandsReceiver == null) {
            commandsReceiver = new BroadcastReceiver() {
                @Override
                public void onReceive(Context context, Intent intent) {
                    if (intent.getAction().equalsIgnoreCase(ATLAS_CLIENT_COMMANDS_BROADCAST) && viewModel != null) {
                        viewModel.fetchTotalPendingCommands();
                    }
                }
            };
            getContext().registerReceiver(commandsReceiver, new IntentFilter(ATLAS_CLIENT_COMMANDS_BROADCAST));
        }
    }

    @Override
    public void onPause() {
        super.onPause();

        if (commandsReceiver != null) {
            getContext().unregisterReceiver(commandsReceiver);
            commandsReceiver = null;
        }
    }

    private void observeListViewModel() {
        viewModel.getTotalPendingCommands().observe(getViewLifecycleOwner(), new Observer<Long>() {
            @Override
            public void onChanged(Long totalPendingCommands) {
                if (totalPendingCommands != null) {
                    binding.totalPendingCommandsTextView.setText(totalPendingCommands.toString());
                    if (totalPendingCommands == 1) {
                        binding.totalPendingCommandsLabelTextView.setText(R.string.home_total_command_waiting);
                    } else {
                        binding.totalPendingCommandsLabelTextView.setText(R.string.home_total_commands_waiting);
                    }
                }
            }
        });
    }

    private final OpenCommandListCallback callback = new OpenCommandListCallback() {
        @Override
        public void onClick() {
            Log.w(Home.class.toString(), "Click on open commands!");
            TabLayout tabs = getActivity().findViewById(R.id.tabLayout);
            tabs.getTabAt(2).select();
        }
    };
}