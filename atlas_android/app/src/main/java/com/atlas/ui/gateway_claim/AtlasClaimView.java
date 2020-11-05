package com.atlas.ui.gateway_claim;

import android.content.DialogInterface;
import android.os.Bundle;
import android.util.Log;
import android.util.Patterns;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.URLUtil;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import com.atlas.BuildConfig;
import com.atlas.R;
import com.atlas.ui.main.MainActivity;
import com.atlas.utils.AtlasConstants;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.Lifecycle;
import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProvider;

public class AtlasClaimView extends Fragment {

    private final String ATLAS_CLAIM_REQUEST_PROTOCOL = "https";

    /* Claim view model */
    private AtlasClaimViewModel viewModel;
    /* UI edit texts */
    private EditText gatewayHostname;
    private EditText shortCode;
    private EditText alias;
    /* Edit text values */
    private String gatewayHostnameValue;
    private String shortCodeValue;
    private String aliasValue;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.claim_gateway, container, false);

        Button claimButton = view.findViewById(R.id.claimButton);
        gatewayHostname = view.findViewById(R.id.gateway_ip_port_edit_text);
        shortCode = view.findViewById(R.id.gateway_short_code_edit_text);
        alias = view.findViewById(R.id.gateway_alias_edit_text);

        claimButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (!Patterns.IP_ADDRESS.matcher(gatewayHostname.getText().toString()).matches()) {
                    Toast.makeText(getContext(), "Gateway IP address is not valid!", Toast.LENGTH_SHORT).show();
                    return;
                }
                if (shortCode.getText().toString().matches("")) {
                    Toast.makeText(getContext(), "Enter short code", Toast.LENGTH_SHORT).show();
                    return;
                }
                if (alias.getText().toString().matches("")) {
                    Toast.makeText(getContext(), "Enter alias", Toast.LENGTH_SHORT).show();
                    return;
                }
                gatewayHostnameValue = gatewayHostname.getText().toString();
                shortCodeValue = shortCode.getText().toString();
                aliasValue = alias.getText().toString();

                viewModel.validateAlias(aliasValue);
            }
        });

        viewModel = new ViewModelProvider(this).get(AtlasClaimViewModel.class);
        observeViewModel();

        return view;
    }

    private void observeViewModel() {
        viewModel.getAliasLiveData().observe(getViewLifecycleOwner(), new Observer<Boolean>() {
            @Override
            public void onChanged(Boolean aliasValid) {
                Log.d(AtlasClaimView.class.getName(), "Alias value changed from view model");

                if (aliasValid) {
                    Log.d(AtlasClaimView.class.getName(), "Alias value is valid. Claiming the gateway now...");
                    viewModel.claimGateway(gatewayHostnameValue, shortCodeValue, aliasValue);
                } else {
                    Log.d(AtlasClaimView.class.getName(), "Alias value is not valid");
                }
            }
        });

        viewModel.getClaimedLiveData().observe(getViewLifecycleOwner(), new Observer<Boolean>() {
            @Override
            public void onChanged(Boolean claimStatus) {
                Log.d(AtlasClaimView.class.getName(), "Claim status value changed from view model");

                /* Clear UI edit texts */
                gatewayHostname.getText().clear();
                shortCode.getText().clear();
                alias.getText().clear();

                /* Show alert dialog with gateway atlas_claim operation status */
                AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(AtlasClaimView.this.getContext());
                alertDialogBuilder.setMessage(claimStatus ? R.string.gateway_claim_success : R.string.gateway_claim_error);
                alertDialogBuilder.setPositiveButton(R.string.gateway_claim_button_ok, null);
                AlertDialog alertDialog = alertDialogBuilder.create();
                alertDialog.show();
            }
        });
    }

    @Override
    public void onResume() {
        super.onResume();
        ((AppCompatActivity) getActivity()).getSupportActionBar().setTitle(getString(R.string.gateway_claim_title));
    }
}