package ro.atlas;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.ApplicationArguments;
import org.springframework.boot.ApplicationRunner;
import org.springframework.scheduling.annotation.EnableScheduling;
import org.springframework.stereotype.Component;

import ro.atlas.service.AtlasGatewayService;
import ro.atlas.service.AtlasMqttService;
import ro.atlas.service.AtlasOwnerService;


@Component
@EnableScheduling
public class ApplicationInit implements ApplicationRunner {
	private static final Logger LOG = LoggerFactory.getLogger(ApplicationInit.class);
 
	private @Autowired AtlasMqttService mqttService;
	private @Autowired AtlasGatewayService gatewayService;
	private @Autowired AtlasOwnerService ownerService;
	
    @Override
    public void run(ApplicationArguments args) throws Exception {
        LOG.info("Init ATLAS cloud application...");
    
        /* Init MQTT service */
        mqttService.start();
        
        /* Init gateways */
        gatewayService.initGateways();
        
        /* Init owners */
        ownerService.initOwners();
    }
}
