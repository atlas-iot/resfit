package com.atlas.ui.main;

import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;

import com.atlas.R;
import com.atlas.firebase.AtlasFirebaseUtils;
import com.atlas.model.database.AtlasClient;
import com.atlas.model.database.AtlasGateway;
import com.atlas.ui.home.view.Home;
import com.atlas.ui.client_list.view.AtlasClientListView;
import com.atlas.ui.command_list.view.AtlasCommandListView;
import com.atlas.ui.gateway_claim.AtlasClaimView;
import com.atlas.ui.gateway_list.view.AtlasGatewayListView;
import com.atlas.ui.gateway_list.view.BackStackFragment;
import com.atlas.utils.AtlasConstants;
import com.atlas.utils.AtlasSharedPreferences;
import com.atlas.worker.AtlasCommandWorker;
import com.atlas.worker.AtlasFirebaseWorker;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.android.material.tabs.TabLayout;
import com.google.firebase.iid.FirebaseInstanceId;
import com.google.firebase.iid.InstanceIdResult;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.viewpager.widget.ViewPager;
import androidx.work.BackoffPolicy;
import androidx.work.ExistingPeriodicWorkPolicy;
import androidx.work.OneTimeWorkRequest;
import androidx.work.PeriodicWorkRequest;
import androidx.work.WorkManager;

import java.time.Duration;
import java.util.UUID;
import java.util.concurrent.TimeUnit;

import static com.atlas.utils.AtlasConstants.ATLAS_COMMAND_WORKER_TIME_MIN;

public class MainActivity extends AppCompatActivity {

    private TabLayout tabLayout;
    private ViewPager viewPager;
    private PageAdapter adapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        setTheme(R.style.AppTheme);

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        executeUtils();

        viewPager = findViewById(R.id.viewPager);
        setupViewPager(viewPager);

        tabLayout = findViewById(R.id.tabLayout);
        tabLayout.setupWithViewPager(viewPager);

        tabLayout.getTabAt(0).setIcon(R.drawable.ic_baseline_home_24);
        tabLayout.getTabAt(1).setIcon(R.drawable.ic_baseline_edit_24);
        tabLayout.getTabAt(2).setIcon(R.drawable.ic_outline_ballot_24);

        /* Notification setup */
        NotificationManager manager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
        manager.createNotificationChannel(new NotificationChannel(AtlasConstants.ATLAS_NOTIFICATION_CHANNEL_ID, AtlasConstants.ATLAS_NOTIFICATION_CHANNEL_NAME, NotificationManager.IMPORTANCE_HIGH));
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == R.id.force_sync_commands) {
            Log.d(MainActivity.class.getName(), "Force sync commands");
            fetchCommandsFromCloud();
        }

        return(super.onOptionsItemSelected(item));
    }

    @Override
    public void onResume() {
        super.onResume();
        /* Fetch commands from cloud */
        fetchCommandsFromCloud();
    }

    private void fetchCommandsFromCloud() {
        PeriodicWorkRequest commandWorker = new PeriodicWorkRequest.Builder(AtlasCommandWorker.class, ATLAS_COMMAND_WORKER_TIME_MIN, TimeUnit.MINUTES)
                .setBackoffCriteria(BackoffPolicy.LINEAR, Duration.ofMinutes(1))
                .build();
        WorkManager.getInstance().enqueueUniquePeriodicWork(AtlasCommandWorker.class.getName(),
                ExistingPeriodicWorkPolicy.REPLACE,
                commandWorker);
    }

    private void executeUtils() {
        /* Generate owner ID if necessary */
        String ownerID = AtlasSharedPreferences.getInstance(getApplication()).getOwnerID();
        if (ownerID == null) {
            Log.i(MainActivity.class.getName(), "Generating application owner UUID");
            ownerID = UUID.randomUUID().toString();
            AtlasSharedPreferences.getInstance(getApplication()).saveOwnerID(UUID.randomUUID().toString());
            Log.i(MainActivity.class.getName(), "Owner UUID is: " + ownerID);
        }
        Log.i(MainActivity.class.getName(), "Owner UUID is: " + ownerID);

        FirebaseInstanceId.getInstance().getInstanceId().addOnCompleteListener(
                new OnCompleteListener<InstanceIdResult>() {
                    @Override
                    public void onComplete(@NonNull Task<InstanceIdResult> task) {
                        if (task.isSuccessful()) {
                            Log.i(AtlasFirebaseUtils.class.getName(), "Firebase token obtained");
                            AtlasSharedPreferences.getInstance(getApplicationContext()).saveFirebaseToken(task.getResult().getToken().toString());

                            /* Update firebase token to cloud */
                            OneTimeWorkRequest firebaseUpdateWorker = new OneTimeWorkRequest.Builder(AtlasFirebaseWorker.class)
                                    .setInitialDelay(Duration.ZERO)
                                    .setBackoffCriteria(BackoffPolicy.LINEAR, Duration.ofMinutes(1))
                                    .build();
                            WorkManager.getInstance().enqueue(firebaseUpdateWorker);
                        } else {
                            Log.e(AtlasFirebaseUtils.class.getName(), "Firebase token error", task.getException());
                        }
                    }
                });
    }

    private void setupViewPager(ViewPager viewPager) {
        adapter = new PageAdapter(getSupportFragmentManager());
        adapter.addFragment(new Home(), getString(R.string.main_home_tab_title));
        adapter.addFragment(new AtlasClaimView(), getString(R.string.main_claim_tab_title));
        adapter.addFragment(new AtlasGatewayListView(), getString(R.string.main_commands_tab_title));
        viewPager.setOffscreenPageLimit(2);
        viewPager.setAdapter(adapter);
    }

    @Override
    public void onBackPressed() {
        if (!BackStackFragment.handleBackPressed(getSupportFragmentManager())) {
            super.onBackPressed();
        }
    }

    public void openAtlasClientListFragment(AtlasGateway gateway) {
        AtlasGatewayListView gatewayListFragment = (AtlasGatewayListView) adapter.getItem(viewPager.getCurrentItem());
        gatewayListFragment.replaceFragment(AtlasClientListView.getInstance(gateway.getIdentity()));
    }

    public void openAtlasClientCommandListFragment(AtlasClient client) {
        AtlasGatewayListView gatewayListFragment = (AtlasGatewayListView) adapter.getItem(viewPager.getCurrentItem());
        gatewayListFragment.replaceFragment(AtlasCommandListView.getInstance(client.getIdentity()));
    }
}