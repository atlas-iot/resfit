package ro.atlas.service.impl;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.security.SecureRandom;
import java.util.Base64;
import java.util.List;
import java.util.UUID;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.IMqttMessageListener;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.MqttPersistenceException;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import ro.atlas.dto.AtlasUsernamePassDto;
import ro.atlas.properties.AtlasProperties;
import ro.atlas.service.AtlasGatewayService;
import ro.atlas.service.AtlasMqttService;

@Component
public class AtlasMqttServiceImpl implements AtlasMqttService, IMqttMessageListener, MqttCallback {
	private static final String MOSQUITTO_PASSWD_TMP = "mosquitto.passwd.tmp";
	private static final String DUMMY_CREDENTIAL = "dummy-";
	private static final int DUMMY_PASSWORD_LEN = 64;
	private static final Logger LOG = LoggerFactory.getLogger(AtlasMqttServiceImpl.class);
	private static final int KEEPALIVE_SEC = 600;
	private MqttClient client;
	private String clientId;
	private MemoryPersistence persistence = new MemoryPersistence();
	private SecureRandom random = new SecureRandom();
	private @Autowired AtlasGatewayService gatewayService;
	private @Autowired AtlasProperties properties;

	@Override
	public void start() {
		LOG.info("Start MQTT service...");

		/* Generate unique client id */
		clientId = UUID.randomUUID().toString();

		/* Create connection options */
		MqttConnectOptions options = new MqttConnectOptions();
		options.setAutomaticReconnect(false);
		options.setCleanSession(true);
		options.setConnectionTimeout(properties.getMqttTimeout());
		options.setKeepAliveInterval(KEEPALIVE_SEC);

		/* Create MQTT client */
		try {
			LOG.info("Connecting to broker: " + properties.getBroker() + " with client id " + clientId);

			client = new MqttClient(properties.getBroker(), clientId, persistence);
			client.setCallback(this);
			client.connect(options);
		} catch (MqttException e) {
			e.printStackTrace();
		}
	}

	@Override
	public void messageArrived(String topic, MqttMessage message) throws Exception {
		LOG.info("Received message on topic " + topic);

		gatewayService.messageReceived(topic, message.getPayload());
	}

	@Override
	public void connectionLost(Throwable cause) {
		LOG.info("Connection to server is lost: " + cause.getMessage());
	}

	@Override
	public void deliveryComplete(IMqttDeliveryToken token) {
		LOG.info("Message delivery is complete");
	}

	@Override
	public void addSubscribeTopic(String topic) {
		if (client.isConnected()) {
			LOG.info("Subscribe to gateway topic: " + topic);
			try {
				client.subscribe(topic);
			} catch (MqttException e) {
				e.printStackTrace();
			}
		}
	}

	@Override
	public void publish(String topic, String message) {
		try {
			client.publish(topic, message.getBytes(), properties.getMqttQos(), false);
		} catch (MqttPersistenceException e) {
			e.printStackTrace();
		} catch (MqttException e) {
			e.printStackTrace();
		}
	}

	public boolean shouldSubscribeAllTopics() {
		/* If the client is already connected, there is nothing to do */
		if (client == null || !client.isConnected()) {
			/* Try to connect to MQTT broker again */
			start();
			/* If connection is successful, then signal a subscribe all state */
			if (client != null && client.isConnected())
				return true;
		}
		
		return false;
	}

	@Override
	public void syncUsernamePass(List<AtlasUsernamePassDto> usernamePassList) {
		
		try {
			BufferedWriter writer = new BufferedWriter(new FileWriter(properties.getTmpDir() + "/" + MOSQUITTO_PASSWD_TMP));
			
			/*
			 * Mosquitto requires at least one entry in the credentials file, otherwise it
			 * will allow any device to log in
			 */
			byte[] bytes = new byte[DUMMY_PASSWORD_LEN];
			random.nextBytes(bytes);
			writer.append(DUMMY_CREDENTIAL + UUID.randomUUID().toString() + ":"
					+ Base64.getEncoder().encodeToString(bytes) + "\n");
			
			
			for (AtlasUsernamePassDto usernamePass : usernamePassList) {
				LOG.info("Allow the following MQTT username/password: " + usernamePass.getUsername() + ":"
						+ usernamePass.getPassword());
				writer.append(usernamePass.getUsername() + ":" + usernamePass.getPassword() + "\n");
			}
			
			writer.close();

			/* Transform the credentials file into the mosquitto format */
			Process process = Runtime.getRuntime().exec(String.format("%s -U %s", properties.getPasswordTool(),
					properties.getTmpDir() + "/" + MOSQUITTO_PASSWD_TMP));

			if (process.waitFor() != 0)
				LOG.error("Error in setting the MQTT credentials");
			else
				LOG.info("Password file generated succesfully in the temporary directory!");

			/* Reload MQTT credentials */
			process = Runtime.getRuntime().exec(String.format("%s %s", properties.getCredentialsReloadExec(),
					properties.getTmpDir() + "/" + MOSQUITTO_PASSWD_TMP));

			if (process.waitFor() != 0)
				LOG.error("Error in reloading the MQTT credentials");
			else
				LOG.info("MQTT credentials reloaded succesfully!");
		} catch (IOException | InterruptedException e) {
			e.printStackTrace();
		}
		
		
	}
}
