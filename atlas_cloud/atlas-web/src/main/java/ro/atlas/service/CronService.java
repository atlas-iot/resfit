package ro.atlas.service;

import org.springframework.stereotype.Service;

@Service
public interface CronService {
    public void keepaliveTask();

    /**
     * Update reputation samples with a sampling period of 5 min
     */
    public void updateReputationSamplesTask();
    
    /**
     * Firebase retry task
     */
    public void firebaseRetryTask();
}
