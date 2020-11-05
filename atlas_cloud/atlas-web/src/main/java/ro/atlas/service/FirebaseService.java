package ro.atlas.service;

import org.springframework.stereotype.Service;

@Service
public interface FirebaseService {

	/**
	 * Send push notification to android device
	 * @param firebaseToken Firebase token
	 */
	void sendPushNotification(String firebaseToken);
	
	/**
	 * Send retry notifications to android devices
	 */
	void sendRetryNotifications();
}

