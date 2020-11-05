package ro.atlas.service;

import java.util.List;
import org.springframework.stereotype.Service;
import ro.atlas.dto.AtlasUsernamePassDto;

@Service
public interface AtlasMqttService {
	/**
	 * Start MQTT service
	 */
	void start();
	
	/**
	 * Sync a set of username and password set (allow only this credentials to
	 * connect to the cloud MQTT broker)
	 * 
	 * @param usernamePassList Username/password list
	 */
	void syncUsernamePass(List<AtlasUsernamePassDto> usernamePassList);
	
	/**
	 * Add subscribe topic
	 * @param topic Gateway topic
	 */
	void addSubscribeTopic(String topic);
	
	/**
	 * Publish a message to a topic
	 * @param topic Publish-subscribe topic
	 * @param message Message payload
	 */
	void publish(String topic, String message);
	
	/**
	 * Indicates if the underlying connection was broken and if all the topics should
	 * be subscribed again
	 * @return True if the topics should be subscribed again, false otherwise
	 */
	boolean shouldSubscribeAllTopics();
}
