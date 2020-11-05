package ro.atlas.service.impl;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Component;

import ro.atlas.service.AtlasGatewayService;
import ro.atlas.service.CronService;
import ro.atlas.service.FirebaseService;

@Component
public class CronServiceImpl implements CronService {
    private static final int KEEPALIVE_INITIAL_DELAY_MS = 10000;
    private static final Logger LOG = LoggerFactory.getLogger(CronServiceImpl.class);
    private @Autowired AtlasGatewayService gatewayService;
    private @Autowired FirebaseService firebaseService;
	
    @Scheduled(initialDelay = KEEPALIVE_INITIAL_DELAY_MS, fixedRateString = "${atlas-cloud.keepalive-task-interval-min}")
    @Override
    public void keepaliveTask() {
        LOG.info("Run keep-alive task for gateways");
		
        gatewayService.keepaliveTask();
    }

    @Scheduled(initialDelay = KEEPALIVE_INITIAL_DELAY_MS, fixedRateString = "${atlas-cloud.samples-update-min}")
    @Override
    public void updateReputationSamplesTask() {
        LOG.info("Update reputation samples for clients");

        gatewayService.updateReputationSamples();
    }

    @Scheduled(initialDelay = KEEPALIVE_INITIAL_DELAY_MS, fixedRateString = "${atlas-cloud.firebase-retry-task-interval-min}")
	@Override
	public void firebaseRetryTask() {
		LOG.info("Firebase retry task");
		
		firebaseService.sendRetryNotifications();
	}
}
